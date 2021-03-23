#include "timer.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "socket.h"

#define GLOB(v) ZEND_MODULE_GLOBALS_ACCESSOR(php_profiler, v)

static zend_always_inline function_frame* allocate_frame()
{
    if (GLOB(free_frames_list) != NULL) {
        function_frame *frame = GLOB(free_frames_list);
        GLOB(free_frames_list) = frame->previous_frame;
        frame->previous_frame = NULL;
        return frame;
    }
    return (function_frame *)emalloc(sizeof(function_frame));
}

static zend_always_inline void lazy_free_function_frame(function_frame *frame)
{
    if (frame->class_name != NULL) {
        zend_string_release(frame->class_name);
        frame->class_name = NULL;
    }

    if (frame->func_name != NULL) {
        zend_string_release(frame->func_name);
        frame->func_name = NULL;
    }

    frame->w_start = 0;
    frame->w_end = 0;
    frame->recursive_level = 0;
    frame->previous_frame = GLOB(free_frames_list);
    GLOB(free_frames_list) = frame->previous_frame;
}

static zend_always_inline zend_string* get_function_name(zend_execute_data *execute_data)
{
    zend_function *curr_func;
    if (!execute_data) {
        return NULL;
    }
    curr_func = execute_data->func;
    if (!curr_func->common.function_name) {
        return NULL;
    }
    zend_string_addref(curr_func->common.function_name);
    return curr_func->common.function_name;
}

static zend_always_inline zend_string* get_class_name(zend_execute_data *execute_data)
{
    zend_function *curr_func;
    if (!execute_data) {
        return NULL;
    }
    curr_func = execute_data->func;
    if (curr_func->common.scope != NULL) {
        zend_string_addref(curr_func->common.scope->name);
        return curr_func->common.scope->name;
    }
    return NULL;
}

static zend_always_inline int start_profiling_function(zend_execute_data *execute_data)
{
    zend_string *function_name = get_function_name(execute_data);
    if (function_name == NULL) {
        return 0;
    }

    function_frame* frame = allocate_frame();
    frame->previous_frame = GLOB(current_frame);
    GLOB(current_frame) = frame;

    frame->func_name = function_name;
    frame->class_name = get_class_name(execute_data);
    frame->recursive_level = ++GLOB(current_recursive_level);
    frame->w_start = time_milliseconds(GLOB(clock_source), GLOB(timebase_factor));
    return 1;
}

static zend_always_inline void enqueue()
{
    function_frame *frame = GLOB(stack_frame);
    char *c = GLOB(function_chunk);
    int i = GLOB(chunk_length);
    int len;
    while (frame != NULL) {
        len = snprintf(c + i, 32768 - i, "[%s::%s(),%llu,%llu,%d],",
                        frame->class_name == NULL ? "" : ZSTR_VAL(frame->class_name),
                        ZSTR_VAL(frame->func_name),
                        frame->w_start,
                        frame->w_end-frame->w_start,
                        frame->recursive_level
                        );
        i += len;
        frame = frame->previous_frame;
    }
    GLOB(chunk_length) = i;
    GLOB(stack_frame) = NULL;

    if (GLOB(chunk_length) > 30000) {
        send_data();
    }
}

static zend_always_inline void end_profiling_function()
{
    function_frame* frame = GLOB(current_frame);
    GLOB(current_frame) = frame->previous_frame;
    frame->w_end = time_milliseconds(GLOB(clock_source), GLOB(timebase_factor));
    --GLOB(current_recursive_level);

    if (frame->w_end - frame->w_start >= GLOB(threshold)) {
        frame->previous_frame = GLOB(stack_frame);
        GLOB(stack_frame) = frame;
        if (frame->recursive_level == 1) {
            enqueue();
        }
    } else {
        lazy_free_function_frame(frame);
    }
}