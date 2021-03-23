/* php_profiler extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_php_profiler.h"
#include "timer.h"
#include "profiler.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "socket.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif
#define GLOB(v) ZEND_MODULE_GLOBALS_ACCESSOR(php_profiler, v)

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("php_profiler.clock_use_rdtsc", "0", PHP_INI_SYSTEM, OnUpdateBool, clock_use_rdtsc, zend_php_profiler_globals, php_profiler_globals)
PHP_INI_END()

static void (*original_zend_execute_ex) (zend_execute_data *execute_data);
static void (*original_zend_execute_internal) (zend_execute_data *execute_data, zval *return_value);

void my_execute_internal(zend_execute_data *execute_data, zval *return_value)
{
    int is_profiling = start_profiling_function(execute_data);
    execute_internal(execute_data, return_value);

    if (is_profiling) {
        end_profiling_function();
    }
}
void my_execute_ex (zend_execute_data *execute_data)
{
    int is_profiling = start_profiling_function(execute_data);
    original_zend_execute_ex(execute_data);

    if (is_profiling) {
        end_profiling_function();
    }
}

void init_request()
{
    GLOB(clock_source) = determine_clock_source(GLOB(clock_use_rdtsc));
    GLOB(timebase_factor) = get_timebase_factor(GLOB(clock_source));
    GLOB(current_recursive_level) = 0;
    GLOB(i_leaf_node) = 0;
    GLOB(n_leaf_node) = 0;
    GLOB(chunk_length) = 0;

    GLOB(threshold) = atoi(getenv("php_profiler_threshold") == NULL ? "0" : getenv("php_profiler_threshold"));
    GLOB(max_recursion) = atoi(getenv("php_profiler_max_recursion") == NULL ? "100" : getenv("php_profiler_max_recursion"));
    GLOB(enabled) = atoi(getenv("php_profiler_enabled") == NULL ? "0" : getenv("php_profiler_enabled"));

    init_sock("/tmp/echo.sock");
}

PHP_RINIT_FUNCTION(php_profiler)
{
#if defined(ZTS) && defined(COMPILE_DL_PHP_PROFILER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
    init_request();
    original_zend_execute_ex = zend_execute_ex;
    original_zend_execute_internal = zend_execute_internal;

    if (GLOB(enabled)) {
        zend_execute_ex = my_execute_ex;
        zend_execute_internal = my_execute_internal;
    }

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(php_profiler)
{
    zend_execute_ex = original_zend_execute_ex;
    zend_execute_internal = original_zend_execute_internal;

    php_printf("%s\n", php_profiler_globals.function_chunk);
    send_data(php_profiler_globals.function_chunk);
//    send_data("sdfsf");
	return SUCCESS;
}

PHP_GSHUTDOWN_FUNCTION(php_profiler)
{
}

PHP_GINIT_FUNCTION(php_profiler)
{
    php_profiler_globals->free_frames_list = NULL;
    php_profiler_globals->current_frame = NULL;
    php_profiler_globals->stack_frame = NULL;
}


/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(php_profiler)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "php_profiler support", "enabled");
	php_info_print_table_end();
}


/* {{{ php_profiler_functions[]
 */
static const zend_function_entry php_profiler_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ php_profiler_module_entry
 */
zend_module_entry php_profiler_module_entry = {
	STANDARD_MODULE_HEADER,
	"php_profiler",					/* Extension name */
	php_profiler_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(php_profiler),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(php_profiler),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(php_profiler),			/* PHP_MINFO - Module info */
	PHP_PHP_PROFILER_VERSION,		/* Version */
	PHP_MODULE_GLOBALS(php_profiler),
	PHP_GINIT(php_profiler),
	PHP_GSHUTDOWN(php_profiler),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_PHP_PROFILER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(php_profiler)
#endif
