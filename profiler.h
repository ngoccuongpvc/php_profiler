#include "timer.h"

#define GLOB(v) ZEND_MODULE_GLOBALS_ACCESSOR(php_profiler, v)
static zend_always_inline void add_leaf_node(function_frame *frame);

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
    frame->is_visited = IS_FALSE;
    return 1;
}

static zend_always_inline void end_profiling_function()
{
    function_frame* frame = GLOB(current_frame);
    GLOB(current_frame) = frame->previous_frame;
    frame->w_end = time_milliseconds(GLOB(clock_source), GLOB(timebase_factor));

    //this frame doesn't have any children frame => it is a leaf node
    if (frame->recursive_level == GLOB(current_recursive_level)) {
        add_leaf_node(frame);
    }
    --GLOB(current_recursive_level);
//    print_debug(frame);
}

static zend_always_inline void add_leaf_node(function_frame *frame)
{
    if (GLOB(n_leaf_node) == 0) {
        GLOB(leaf_nodes) = (function_frame**)emalloc(sizeof(function_frame*)*100);
        GLOB(n_leaf_node) = 100;
    }
    GLOB(leaf_nodes[GLOB(i_leaf_node)++]) = frame;
}