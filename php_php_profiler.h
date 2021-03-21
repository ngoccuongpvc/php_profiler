/* php_profiler extension for PHP */

#ifndef PHP_PHP_PROFILER_H
# define PHP_PHP_PROFILER_H

extern zend_module_entry php_profiler_module_entry;
# define phpext_php_profiler_ptr &php_profiler_module_entry

# define PHP_PHP_PROFILER_VERSION "0.1.0"
#define PHP_PROFILER_CLOCK_CGT 0
#define PHP_PROFILER_CLOCK_GTOD 1
#define PHP_PROFILER_CLOCK_TSC 2
#define PHP_PROFILER_CLOCK_MACH 3
#define PHP_PROFILER_CLOCK_QPC 4
#define PHP_PROFILER_CLOCK_NONE 255

#if !defined(uint32)
typedef unsigned int uint32;
#endif

#if !defined(uint64)
typedef unsigned long long uint64;
#endif

typedef struct function_frame function_frame;

struct function_frame {
    zend_string *class_name;
    zend_string *func_name;
    uint64 w_start;
    uint64 w_end;
    int recursive_level;
    struct function_frame *previous_frame;
};

ZEND_BEGIN_MODULE_GLOBALS(php_profiler)
    function_frame *free_frames_list, *current_frame, *stack_frame;
    int i_leaf_node, n_leaf_node, clock_source, chunk_length, current_recursive_level, max_recursion, enabled;
    zend_bool clock_use_rdtsc;
    double timebase_factor;
    char function_chunk[32768];
    uint64 threshold;
ZEND_END_MODULE_GLOBALS(php_profiler)

ZEND_DECLARE_MODULE_GLOBALS(php_profiler)

# if defined(ZTS) && defined(COMPILE_DL_PHP_PROFILER)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_PHP_PROFILER_H */
