/* flexid_utils extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "php_flexid_utils.h"
#include "flexid_utils_arginfo.h"

#if SIZEOF_ZEND_LONG < 8
# error "flexid_utils requires a 64-bit system"
#endif

/* ---- Module ----------------------------------------------------------- */

int flexid_minit(int module_number)
{
	(void)module_number;

	return SUCCESS;
}

PHP_MINIT_FUNCTION(flexid_utils)
{
	#if defined(ZTS) && defined(COMPILE_DL_FLEXID_UTILS)
	ZEND_TSRMLS_CACHE_UPDATE();
	#endif
	if (flexid_minit(module_number) == FAILURE) return FAILURE;
	if (siphash_minit(module_number) == FAILURE) return FAILURE;
	if (blake3_minit(module_number) == FAILURE) return FAILURE;
	if (sparx64_minit(module_number) == FAILURE) return FAILURE;
	if (sparx128_minit(module_number) == FAILURE) return FAILURE;
	if (encoder_minit(module_number) == FAILURE) return FAILURE;
	if (hash_encoder_minit(module_number) == FAILURE) return FAILURE;

	return SUCCESS;
}

PHP_MINFO_FUNCTION(flexid_utils)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "flexid_utils support", "enabled");
	php_info_print_table_row(2, "flexid_utils version", PHP_FLEXID_UTILS_VERSION);
	php_info_print_table_end();
}

zend_module_entry flexid_utils_module_entry = {
	STANDARD_MODULE_HEADER,
	"flexid_utils",			/* Extension name */
	NULL,					/* zend_function_entry */
	PHP_MINIT(flexid_utils),		/* PHP_MINIT - Module initialization */
	NULL,					/* PHP_MSHUTDOWN - Module shutdown */
	NULL,					/* PHP_RINIT - Request initialization */
	NULL,					/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(flexid_utils),		/* PHP_MINFO - Module info */
	PHP_FLEXID_UTILS_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FLEXID_UTILS
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(flexid_utils)
#endif
