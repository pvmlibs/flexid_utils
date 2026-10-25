#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_flexid_utils.h"
#include "flexid_utils_arginfo.h"

typedef struct {
	uint8_t alphabetLength;
	uint8_t resultMaxLength;
	uint8_t charPositions[256];
	char *alphabet;
	char *result;
	uint8_t separator;
	uint64_t M;
	uint64_t (*divmod)(uint64_t, uint32_t, uint64_t, uint64_t*);
	zend_object std;
} encoder_object;

static zend_class_entry     *encoder_ce;
static zend_object_handlers  encoder_handlers;
//static zend_function *encoder_decode_fn;

#define Z_ENCODER_P(zv) \
((encoder_object *)((char *)(Z_OBJ_P(zv)) - offsetof(encoder_object, std)))

static zend_object *encoder_create_object(zend_class_entry *ce)
{
	encoder_object *obj = zend_object_alloc(sizeof(encoder_object), ce);
	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);
	obj->std.handlers = &encoder_handlers;
	obj->alphabet = NULL;
	obj->alphabetLength = 0;
	obj->resultMaxLength = 0;
	obj->result = NULL;
	return &obj->std;
}

static void encoder_free_object(zend_object *object)
{
	encoder_object *obj = (encoder_object *)((char *)object - offsetof(encoder_object, std));
	if (obj->result) {
		efree(obj->result);
	}
	if (obj->alphabet) {
		efree(obj->alphabet);
	}
	zend_object_std_dtor(&obj->std);
}

static inline uint64_t udivmod(uint64_t dividend, uint32_t divisor, uint64_t M, uint64_t* reminder)
{
	#if defined(__SIZEOF_INT128__)
	uint64_t q = (uint64_t)(((__uint128_t)dividend * M) >> 64);
	uint64_t r = (uint64_t)((__uint128_t)dividend - q * divisor);

	q -= r >> 63;
	*reminder = dividend - q * divisor;
	return q;
	#else
	uint64_t q = dividend / divisor;
	*reminder = dividend - q * divisor;
	return q;
	#endif
}

static inline uint64_t udivmodshift(uint64_t dividend, uint32_t divisor, uint64_t M, uint64_t* reminder)
{
	uint64_t quotient = dividend >> M;
	*reminder = dividend - quotient * divisor;
	return quotient;
}

PHP_METHOD(FlexId_Encoder, __construct)
{
	char *alphabet, *separator = " ";
	size_t alphabetLen, separatorLen = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STRING(alphabet, alphabetLen)
	Z_PARAM_OPTIONAL
	Z_PARAM_STRING_OR_NULL(separator, separatorLen)
	ZEND_PARSE_PARAMETERS_END();


	if (alphabetLen < 2 || alphabetLen > 94) {
		zend_argument_value_error(1, "Alphabet must be between 2 and 94 characters long");
		RETURN_THROWS();
	}

	encoder_object *obj = Z_ENCODER_P(ZEND_THIS);

	if (separatorLen != 1) {
		zend_argument_value_error(2, "Separtor must be 1 character");
		RETURN_THROWS();
	}

	obj->separator = (uint8_t)separator[0];
	for (int i = 0; i < alphabetLen; i++) {
		if (obj->separator == (uint8_t)(alphabet[i])) {
			zend_argument_value_error(2, "Separtor must be unique - not included in alphabet");
			RETURN_THROWS();
		}
	}

	memset(obj->charPositions, 255, 256);

	// maps character value with it's position in alphabet - optimize decoding
	for (int i = 0; i < alphabetLen; i++) {
		uint8_t x = (uint8_t)(alphabet[i]);
		if (x > 127 || x < 32) {
			zend_argument_value_error(1, "Alphabet must contain printable characters");
			RETURN_THROWS();
		}
		if (obj->charPositions[x] != 255) {
			// char position already set, so we have duplicate
			zend_argument_value_error(1, "Alphabet must contain unique characters");
			RETURN_THROWS();
		}
		obj->charPositions[x] = i;
	}

	if (obj->alphabet) {
		efree(obj->alphabet);
	}
	if (obj->result) {
		efree(obj->result);
	}
	obj->alphabet = emalloc(alphabetLen);
	memcpy(obj->alphabet, alphabet, alphabetLen);
	obj->alphabetLength = (uint8_t)alphabetLen;

	uint64_t r, x = UINT64_MAX;

	if ((obj->alphabetLength & (obj->alphabetLength - 1)) == 0) {
		// only for power of 2
		obj->M = log2(obj->alphabetLength);
		obj->divmod = &udivmodshift;
	} else {
		obj->M = (UINT64_MAX / obj->alphabetLength) + 1;
		obj->divmod = &udivmod;
	}

	uint32_t digits = 0;
	do {
		digits++;
		x = obj->divmod(x, obj->resultMaxLength, obj->M, &r);
	} while (x > 0);

	if (digits > 64) {
		zend_argument_value_error(1, "Max result is too long");
		RETURN_THROWS();
	}

	obj->resultMaxLength = digits;
	obj->result = emalloc(obj->resultMaxLength * 2);
}

static uint8_t encode(uint64_t num, uint8_t startFrom, encoder_object **obj)
{
	uint64_t reminder;
	uint8_t c = startFrom;
	uint64_t quotient = (*obj)->divmod(num, (*obj)->alphabetLength, (*obj)->M, &reminder);

	(*obj)->result[c++] = (*obj)->alphabet[reminder];

	if (quotient == 0) {
		return c;
	}

	while (quotient >= (*obj)->alphabetLength) {
		quotient = (*obj)->divmod(quotient, (*obj)->alphabetLength, (*obj)->M, &reminder);
		(*obj)->result[c++] = (*obj)->alphabet[reminder];
	}

	// here always >= 1 chars, this will be the last so don't need to div
	(*obj)->result[c++] = (*obj)->alphabet[quotient];

	return c;
}

PHP_METHOD(FlexId_Encoder, encode)
{
	zend_long num;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END();

	encoder_object *obj = Z_ENCODER_P(ZEND_THIS);

	uint8_t length = encode((uint64_t)num, 0, &obj);

	RETURN_STRINGL(obj->result, length);
}

PHP_METHOD(FlexId_Encoder, doubleEncode)
{
	zend_long hipart, lowpart;
	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_LONG(hipart)
	Z_PARAM_LONG(lowpart)
	ZEND_PARSE_PARAMETERS_END();

	encoder_object *obj = Z_ENCODER_P(ZEND_THIS);

	uint8_t length = encode((uint64_t)hipart, 0, &obj);
	if (length < obj->resultMaxLength) {
		obj->result[length++] = obj->separator;
	}
	length = encode((uint64_t)lowpart, length, &obj);

	RETURN_STRINGL(obj->result, length);
}

static uint64_t decode(size_t startPos, size_t strLen, char* str, encoder_object **obj, char **err)
{
	uint8_t x;
	uint8_t *data = (uint8_t*)str + startPos;
	uint64_t value = 0, reminder;

	if (strLen > (*obj)->resultMaxLength || strLen == 0) {
		*err = "Invalid input length";
		return 0;
	}

	for (int i = strLen-1; i > 0; i--) {
		x = (*obj)->charPositions[data[i]];
		if (x == 255) { // character not found
			*err = "Input contains invalid character";
			return 0;
		}
		value = value * (*obj)->alphabetLength + x;
	}

	// the last one can overflow
	x = (*obj)->charPositions[data[0]];
	if (x == 255) { // character not found
		*err = "Input contains invalid character";
		return 0;
	}

	if (value > (*obj)->divmod(UINT64_MAX - x,  (*obj)->alphabetLength, (*obj)->M, &reminder)) { //(UINT64_MAX - x) / obj->alphabetLength)
		*err = "Input is out of range";
		return 0;
	}

	return value * (*obj)->alphabetLength + x;
}

PHP_METHOD(FlexId_Encoder, decode)
{
	char *str, *err = NULL;
	size_t strLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(str, strLen)
	ZEND_PARSE_PARAMETERS_END();

	encoder_object *obj = Z_ENCODER_P(ZEND_THIS);
	uint64_t decoded = decode(0, strLen, str, &obj, &err);

	if (err != NULL) {
		zend_argument_value_error(1, err);
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long)decoded);
}

PHP_METHOD(FlexId_Encoder, doubleDecode)
{
	char *str, *err = NULL;
	size_t strLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(str, strLen)
	ZEND_PARSE_PARAMETERS_END();

	encoder_object *obj = Z_ENCODER_P(ZEND_THIS);

	if (strLen > obj->resultMaxLength * 2) {
		zend_argument_value_error(1, "Input string is too long");
		RETURN_THROWS();
	}

	array_init_size(return_value, 2);

	uint16_t part1Len = obj->resultMaxLength;
	uint16_t part2Start = obj->resultMaxLength;

	for (int16_t i = 0; i < strLen; i++) {
		if ((uint8_t)str[i] == obj->separator) {
			part1Len = i;
			part2Start = i + 1;
		}
	}
	add_index_long(return_value, 0, (zend_long)decode(0, part1Len, str, &obj, &err));
    if (err != NULL) {
        zend_argument_value_error(1, err);
        RETURN_THROWS();
    }
	add_index_long(return_value, 1, (zend_long)decode(part2Start, strLen - part2Start, str, &obj, &err));
	if (err != NULL) {
		zend_argument_value_error(1, err);
		RETURN_THROWS();
	}
}

PHP_METHOD(FlexId_Encoder, __serialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be serialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_Encoder, __unserialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be unserialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_Encoder, getMaxEncodedLength)
{
	encoder_object *obj = Z_ENCODER_P(ZEND_THIS);

	RETURN_LONG((zend_long)obj->resultMaxLength);
}

int encoder_minit(int module_number)
{
	(void)module_number;

	encoder_ce = register_class_FlexId_Encoder();
	encoder_ce->create_object = encoder_create_object;

	memcpy(&encoder_handlers, zend_get_std_object_handlers(),
		   sizeof(zend_object_handlers));
	encoder_handlers.offset    = offsetof(encoder_object, std);
	encoder_handlers.free_obj  = encoder_free_object;
	encoder_handlers.clone_obj = NULL;

	return SUCCESS;
}

