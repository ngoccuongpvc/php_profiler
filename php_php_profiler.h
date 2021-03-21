/* php_profiler extension for PHP */

#ifndef PHP_PHP_PROFILER_H
# define PHP_PHP_PROFILER_H

extern zend_module_entry php_profiler_module_entry;
# define phpext_php_profiler_ptr &php_profiler_module_entry

# define PHP_PHP_PROFILER_VERSION "0.1.0"
typedef struct function_frame function_frame;

struct function_frame {
    zend_string *class_name;
    zend_string *func_name;
    struct function_frame *previous_frame;
};

ZEND_BEGIN_MODULE_GLOBALS(php_profiler)
    function_frame *root_frame;
    function_frame *current_frame;
    function_frame **leaf_nodes;
ZEND_END_MODULE_GLOBALS(php_profiler)

ZEND_DECLARE_MODULE_GLOBALS(php_profiler)

# if defined(ZTS) && defined(COMPILE_DL_PHP_PROFILER)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_PHP_PROFILER_H */
