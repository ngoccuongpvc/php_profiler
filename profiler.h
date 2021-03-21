#include "timer.h"

#define GLOB(v) ZEND_MODULE_GLOBALS_ACCESSOR(php_profiler, v)
static zend_always_inline void print_debug(function_frame *frame);

static zend_always_inline function_frame* allocate_frame()
{
    return (function_frame *)emalloc(sizeof(function_frame));
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

static zend_always_inline void end_profiling_function()
{
    function_frame* frame = GLOB(current_frame);
    GLOB(current_frame) = frame->previous_frame;
    --GLOB(current_recursive_level);

    frame->w_end = time_milliseconds(GLOB(clock_source), GLOB(timebase_factor));

    print_debug(frame);
}

static zend_always_inline void print_debug(function_frame *frame)
{
    if (frame->class_name) {
        php_printf("%s:\n", ZSTR_VAL(frame->class_name));
    }
    if (frame->func_name) {
        php_printf("%s\n", ZSTR_VAL(frame->func_name));
    }
    php_printf("recursive_level: %d\n", frame->recursive_level);
    php_printf("time started: %llu\n", frame->w_start);
    php_printf("time ended: %llu\n", frame->w_end);
    php_printf("time interval: %llu\n", frame->w_start - frame->w_end);
}
