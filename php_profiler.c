/* php_profiler extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_php_profiler.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

static void (*original_zend_execute_ex) (zend_execute_data *execute_data);
static void (*original_zend_execute_internal) (zend_execute_data *execute_data, zval *return_value);

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

void print_class_detail(zend_execute_data *execute_data)
{
    if (!execute_data) {
        return;
    }
    if (execute_data->func->common.scope != NULL) {
        php_printf("%s", ZSTR_VAL(execute_data->func->common.scope->name));
    }
    return;
}

void my_execute_internal(zend_execute_data *execute_data, zval *return_value)
{
    print_class_detail(execute_data);

    zend_string *function_name = get_function_name(execute_data);
    if (function_name != NULL) {
        php_printf("function name: %s\n", ZSTR_VAL(function_name));
    }
    execute_internal(execute_data, return_value);
}
void my_execute_ex (zend_execute_data *execute_data)
{
    print_class_detail(execute_data);

    zend_string *function_name = get_function_name(execute_data);

    if (function_name != NULL) {
        php_printf("function name: %s\n", ZSTR_VAL(function_name));
    }
    original_zend_execute_ex(execute_data);
}

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(php_profiler)
{
#if defined(ZTS) && defined(COMPILE_DL_PHP_PROFILER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
    original_zend_execute_ex = zend_execute_ex;
    zend_execute_ex = my_execute_ex;

    original_zend_execute_internal = zend_execute_internal;
    zend_execute_internal = my_execute_internal;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(php_profiler)
{
    zend_execute_ex = original_zend_execute_ex;
    zend_execute_internal = original_zend_execute_internal;
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
