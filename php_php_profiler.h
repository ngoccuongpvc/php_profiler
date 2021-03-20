/* php_profiler extension for PHP */

#ifndef PHP_PHP_PROFILER_H
# define PHP_PHP_PROFILER_H

extern zend_module_entry php_profiler_module_entry;
# define phpext_php_profiler_ptr &php_profiler_module_entry

# define PHP_PHP_PROFILER_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_PHP_PROFILER)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_PHP_PROFILER_H */
