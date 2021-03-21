/* php_profiler extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_php_profiler.h"
#include "timer.h"
#include "profiler.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

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


PHP_RINIT_FUNCTION(php_profiler)
{
#if defined(ZTS) && defined(COMPILE_DL_PHP_PROFILER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

    php_profiler_globals.clock_source = determine_clock_source(php_profiler_globals.clock_use_rdtsc);
    php_profiler_globals.timebase_factor = get_timebase_factor(php_profiler_globals.clock_source);
    php_profiler_globals.current_recursive_level = 0;
    php_profiler_globals.i_leaf_node = 0;
    php_profiler_globals.n_leaf_node = 0;


    original_zend_execute_ex = zend_execute_ex;
    zend_execute_ex = my_execute_ex;

    original_zend_execute_internal = zend_execute_internal;
    zend_execute_internal = my_execute_internal;
	return SUCCESS;
}

static void print_debug(function_frame *frame)
{
    if (frame->is_visited == IS_TRUE) {
        return;
    }
    if (frame->previous_frame) {
        print_debug(frame->previous_frame);
    }

    frame->is_visited = IS_TRUE;

    if (frame->class_name) {
        php_printf("%s:\n", ZSTR_VAL(frame->class_name));
    }
    if (frame->func_name) {
        php_printf("%s\n", ZSTR_VAL(frame->func_name));
    }
    php_printf("recursive_level: %d\n", frame->recursive_level);
    php_printf("time started: %llu\n", frame->w_start);
    php_printf("time ended: %llu\n", frame->w_end);
    php_printf("time interval: %llu\n", frame->w_end - frame->w_start);
}

PHP_RSHUTDOWN_FUNCTION(php_profiler)
{
    zend_execute_ex = original_zend_execute_ex;
    zend_execute_internal = original_zend_execute_internal;

    for (int i=0; i<php_profiler_globals.i_leaf_node; ++i) {
        print_debug(php_profiler_globals.leaf_nodes[i]);
    }
	return SUCCESS;
}

PHP_GSHUTDOWN_FUNCTION(php_profiler)
{
}

PHP_GINIT_FUNCTION(php_profiler)
{
    php_profiler_globals->root_frame = NULL;
    php_profiler_globals->current_frame = NULL;
    php_profiler_globals->leaf_nodes = NULL;
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
