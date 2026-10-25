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
	uint16_t keyHashes[4];
	uint16_t keyHashesInv[4];
	uint64_t offset;
	char *alphabet;
	char *result;
	uint64_t M;
	uint64_t (*divmod)(uint64_t, uint32_t, uint64_t, uint64_t*);
	zend_object std;
} hash_encoder_object;

static zend_class_entry     *hash_encoder_ce;
static zend_object_handlers  hash_encoder_handlers;

#define Z_HASH_ENCODER_P(zv) \
((hash_encoder_object *)((char *)(Z_OBJ_P(zv)) - offsetof(hash_encoder_object, std)))

static zend_object *hash_encoder_create_object(zend_class_entry *ce)
{
	hash_encoder_object *obj = zend_object_alloc(sizeof(hash_encoder_object), ce);
	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);
	obj->std.handlers = &hash_encoder_handlers;
	obj->alphabet = NULL;
	obj->alphabetLength = 0;
	obj->resultMaxLength = 0;
	obj->result = NULL;
	return &obj->std;
}

static void hash_encoder_free_object(zend_object *object)
{
	hash_encoder_object *obj = (hash_encoder_object *)((char *)object - offsetof(hash_encoder_object, std));
	if (obj->alphabet) {
		efree(obj->alphabet);
	}
	if (obj->result) {
		efree(obj->result);
	}
	zend_object_std_dtor(&obj->std);
}

uint16_t modInverse16(uint16_t x)
{
	uint16_t y = (3 * x) ^ 2;
	y = y * (2 - y * x);
	y = y * (2 - y * x);

	return y;
}

uint64_t partialXor(uint64_t input, uint8_t shift, uint16_t firstBlock)
{
	uint64_t mask, block = (input >> shift) & 0xFFFF;
	int i;

	if (block < 256) {
		i = 7;
	} else {
		i = 15;
	}

	for (; i > 0; i--) {
		if ((block & (1 << i)) > 0) {
			mask = ((1 << i) - 1);
			block = (block ^ firstBlock) & mask;

			return (input & ~(mask << shift)) | (block << shift);
		}
	}

	return input;
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

PHP_METHOD(FlexId_HashEncoder, __construct)
{
	char *alphabet;
	size_t alphabetLen;
	zend_long offset = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STRING(alphabet, alphabetLen)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	hash_encoder_object *obj = Z_HASH_ENCODER_P(ZEND_THIS);
	if (obj->alphabet) {
		efree(obj->alphabet);
	}
	if (obj->result) {
		efree(obj->result);
	}

	bool alfabetLenPowOf2 = (alphabetLen & (alphabetLen - 1)) == 0;

	if (alphabetLen < 9 || alphabetLen > 94 || alfabetLenPowOf2) {
		zend_argument_value_error(1, "Alphabet must be between 9 and 94 and not power of 2");
		RETURN_THROWS();
	}

	if (offset < 0) {
		zend_argument_value_error(2, "Offset must be >= 0");
		RETURN_THROWS();
	}

	// *2 for rotation without div
	obj->alphabet = emalloc(alphabetLen * 2);
	memcpy(obj->alphabet, alphabet, alphabetLen);
	memcpy(obj->alphabet + alphabetLen, alphabet, alphabetLen);
	obj->alphabetLength = (uint8_t)alphabetLen;
	obj->offset = (uint64_t)offset;

	memset(obj->charPositions, 255, 256);

	// maps character value with it's position in alphabet - optimize decoding
	for (int i = 0; i < obj->alphabetLength; i++) {
		uint8_t x = (uint8_t)alphabet[i];
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

	obj->M = (UINT64_MAX / obj->alphabetLength) + 1;
	obj->divmod = &udivmod;

	uint64_t r, x = MAX_SIGNED_INT;
	int digits = 0;
	do {
		digits++;
		x = obj->divmod(x, obj->alphabetLength, obj->M, &r);
	} while (x > 0);

	if (digits > 64) {
		zend_argument_value_error(1, "Max result is too long");
		RETURN_THROWS();
	}

	obj->resultMaxLength = digits;
	obj->result = emalloc(obj->resultMaxLength);

	uint64_t key = 0;
	for (int i = obj->alphabetLength - 1; i >= obj->alphabetLength - 9; i--) { // 9 max length with base 94
		key = 94 * key + (((int)(obj->alphabet[i])) - 32);
	}

	for (int i = 0; i < 4; i++) {
		obj->keyHashes[i] = (key >> (16 * i) & 0xFFFF) | 1;
		obj->keyHashesInv[i] = modInverse16(obj->keyHashes[i]);
	}
}

PHP_METHOD(FlexId_HashEncoder, encode)
{
	zend_long num;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END();

	hash_encoder_object *obj = Z_HASH_ENCODER_P(ZEND_THIS);

	uint64_t value = (uint64_t)num, reminder;

	if (value > MAX_SIGNED_INT || value + obj->offset > MAX_SIGNED_INT) {
		// we reduce support to positive signed int range, to have feature parity with php
		zend_argument_value_error(1, "Input is out of range");
		RETURN_THROWS();
	}

	value += obj->offset;

	// hash number
	uint16_t firstBlock = value & 0xFFFF;
	const uint64_t mask = 0xFFFF;

	// hash first 16-bit block
	for (int i = 0; i < 4; i++) {
		firstBlock ^= firstBlock >> 8;
		firstBlock *= obj->keyHashes[i];
	}

	uint64_t number = (value & ~mask) | firstBlock;

	// mix firsts block with rest of 16-bit blocks
	if (number > (uint64_t)0xFFFFFFFFFFFF) { // number >= 2^48
		number = (number & ~(mask << 32)) | (((number >> 32) ^ firstBlock) << 32);
		number = (number & ~(mask << 16)) | (((number >> 16) ^ firstBlock) << 16);
		number = partialXor(number, 48, firstBlock);
	} else if (number > (uint64_t)0xFFFFFFFF) { // number < 2^48 && number >= 2^32
		number = (number & ~(mask << 16)) | (((number >> 16) ^ firstBlock) << 16);
			number = partialXor(number, 32, firstBlock);
	} else if (number > (uint64_t)0xFFFF) {
		number = partialXor(number, 16, firstBlock);
	}

	// encode to string
	uint8_t c = 1;
	uint64_t quotient = obj->divmod(number, obj->alphabetLength, obj->M, &reminder);
	uint64_t rotate = reminder; // value % obj->alphabetLength;

	obj->result[0] = obj->alphabet[rotate];
	if (quotient == 0) {
		RETURN_STRINGL(obj->result, c);
	}

	while (quotient >= obj->alphabetLength) {
		quotient = obj->divmod(quotient, obj->alphabetLength, obj->M, &reminder);
		obj->result[c++] = obj->alphabet[reminder + rotate];
	}

	// here always >= 1 chars, this will be the last so don't need to div
	obj->result[c++] = obj->alphabet[quotient + rotate];

	RETURN_STRINGL(obj->result, c);
}

PHP_METHOD(FlexId_HashEncoder, decode)
{
	char *str;
	uint64_t value = 0, number, x;
	size_t strLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(str, strLen)
	ZEND_PARSE_PARAMETERS_END();

	hash_encoder_object *obj = Z_HASH_ENCODER_P(ZEND_THIS);
	uint8_t *data = (uint8_t*)str;

	// decode string to int
	if (strLen > obj->resultMaxLength || strLen == 0) {
		zend_argument_value_error(1, "Invalid input length");
		RETURN_THROWS();
	}

	uint8_t rotate = obj->charPositions[data[0]];
	if (rotate == 255) { // character not found
		zend_argument_value_error(1, "Input contains invalid character");
		RETURN_THROWS();
	}

	for (int i = strLen-1; i > 0; i--) {
		x = obj->charPositions[data[i]];
		if (x == 255) { // character not found
			zend_argument_value_error(1, "Input contains invalid character");
			RETURN_THROWS();
		}

		obj->divmod((obj->alphabetLength + x - rotate), obj->alphabetLength, obj->M, &x); // x = (...) % obj->alphabetLength;
		value = value * obj->alphabetLength + x;
	}

	if (value > (obj->divmod(MAX_SIGNED_INT - rotate, obj->alphabetLength, obj->M, &number))) { //(MAX_SIGNED_INT - rotate) / obj->alphabetLength) {
		zend_argument_value_error(1, "Input is out of range");
		RETURN_THROWS();
	}

	number = value * obj->alphabetLength + rotate;

	// unhash to id
	uint16_t firstBlock = number & 0xFFFF;
	const uint64_t mask = 0xFFFF;

	if (number > (uint64_t)0xFFFFFFFFFFFF) { // number >= 2^48
		number = (number & ~(mask << 32)) | (((number >> 32) ^ firstBlock) << 32);
		number = (number & ~(mask << 16)) | (((number >> 16) ^ firstBlock) << 16);
		number = partialXor(number, 48, firstBlock);
	} else if (number > (uint64_t)0xFFFFFFFF) { // number < 2^48 && number >= 2^32
		number = (number & ~(mask << 16)) | (((number >> 16) ^ firstBlock) << 16);
		number = partialXor(number, 32, firstBlock);
	} else if (number > (uint64_t)0xFFFF) {
		number = partialXor(number, 16, firstBlock);
	}

	// inverse first block hash
	for (int i = 3; i >= 0; i--) {
		firstBlock *= obj->keyHashesInv[i];
		firstBlock ^= firstBlock >> 8;
	}

	uint64_t id = (number & ~mask) | firstBlock;

	if (id < obj->offset) {
		zend_argument_value_error(1, "Invalid input");
		RETURN_THROWS();
	}

	id = id - obj->offset;

	if (id > MAX_SIGNED_INT) {
		zend_argument_value_error(1, "Input is out of range");
		RETURN_THROWS();
	}

	RETURN_LONG((zend_long)id);
}

PHP_METHOD(FlexId_HashEncoder, getMaxEncodedLength)
{
	hash_encoder_object *obj = Z_HASH_ENCODER_P(ZEND_THIS);

	RETURN_LONG((zend_long)obj->resultMaxLength);
}

PHP_METHOD(FlexId_HashEncoder, __serialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be serialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_HashEncoder, __unserialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be unserialized");
	RETURN_THROWS();
}

int hash_encoder_minit(int module_number)
{
	(void)module_number;

	hash_encoder_ce = register_class_FlexId_HashEncoder();
	hash_encoder_ce->create_object = hash_encoder_create_object;

	memcpy(&hash_encoder_handlers, zend_get_std_object_handlers(),
		   sizeof(zend_object_handlers));
	hash_encoder_handlers.offset    = offsetof(hash_encoder_object, std);
	hash_encoder_handlers.free_obj  = hash_encoder_free_object;
	hash_encoder_handlers.clone_obj = NULL;

	return SUCCESS;
}

