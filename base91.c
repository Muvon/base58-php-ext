/*
  +----------------------------------------------------------------------+
  | Base91 PHP extension                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2020 Arnold Daniels                                    |
  +----------------------------------------------------------------------+
  | Permission is hereby granted, free of charge, to any person          |
  | obtaining a copy of this software and associated documentation files |
  | (the "Software"), to deal in the Software without restriction,       |
  | including without limitation the rights to use, copy, modify, merge, |
  | publish, distribute, sublicense, and/or sell copies of the Software, |
  | and to permit persons to whom the Software is furnished to do so,    |
  | subject to the following conditions:                                 |
  |                                                                      |
  | The above copyright notice and this permission notice shall be       |
  | included in all copies or substantial portions of the Software.      |
  |                                                                      |
  | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      |
  | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   |
  | MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                |
  | NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  |
  | BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   |
  | ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    |
  | CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     |
  | SOFTWARE.                                                            |
  +----------------------------------------------------------------------+
  | Author: Arnold Daniels <arnold@jasny.net>                            |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_base91.h"
#include "zend_exceptions.h"

#include "lib/libbase91.h"

#if HAVE_BASE91

ZEND_BEGIN_ARG_INFO_EX(arginfo_base91_encode, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_base91_decode, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry base91_functions[] = {
    PHP_FE(base91_encode, arginfo_base91_encode)
    PHP_FE(base91_decode, arginfo_base91_decode)
    PHP_FE_END
};

zend_module_entry base91_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_BASE91_EXTNAME,
    base91_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_BASE91_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BASE91
ZEND_GET_MODULE(base91)
#endif

PHP_FUNCTION(base91_encode)
{
    zend_string *data;

    zend_string *b91;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(data)
    ZEND_PARSE_PARAMETERS_END();

    b91 = zend_string_alloc((size_t)ceil(ZSTR_LEN(data) * 1.5) + 1, 0);

    if (!b91enc(ZSTR_VAL(b91), &ZSTR_LEN(b91), ZSTR_VAL(data), ZSTR_LEN(data))) {
        zend_string_free(b91);

        php_error_docref(NULL, E_WARNING, "Failed to base91 encode string");
        RETURN_FALSE;
    }

    /* Exclude ending '\0` from string length */
    ZSTR_LEN(b91)--;

    RETURN_STR(b91);
}

PHP_FUNCTION(base91_decode)
{
    zend_string *b91;

    char *data;
    size_t data_len;

    zend_string *result;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(b91)
    ZEND_PARSE_PARAMETERS_END();

    data_len = ZSTR_LEN(b91);
    data = emalloc(data_len);

    if (!b91tobin(data, &data_len, ZSTR_VAL(b91), ZSTR_LEN(b91))) {
        efree(data);

        php_error_docref(NULL, E_WARNING, "Failed to base91 decode string");
        RETURN_FALSE;
    }

    /* libbase91 starts at the end of the buffer, so skip preceding '\0' chars. */
    result = zend_string_init(data + (ZSTR_LEN(b91) - data_len), data_len, 0);
    efree(data);

    RETURN_STR(result);
}

#endif

