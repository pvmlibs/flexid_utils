#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_flexid_utils.h"
#include "flexid_utils_arginfo.h"

#define ROTATE(x, b) (uint64_t)( ((x) << (b)) | ( (x) >> (64 - (b))) )

#define HALF_ROUND(a,b,c,d,s,t) \
a += b; c += d; \
b = ROTATE(b, s) ^ a; \
d = ROTATE(d, t) ^ c; \
a = ROTATE(a, 32);

#define DOUBLE_ROUND(v) \
HALF_ROUND(v[0],v[1],v[2],v[3],13,16); \
HALF_ROUND(v[2],v[1],v[0],v[3],17,21); \
HALF_ROUND(v[0],v[1],v[2],v[3],13,16); \
HALF_ROUND(v[2],v[1],v[0],v[3],17,21);

typedef struct {
	uint64_t k[4];
	zend_object std;
} siphash_object;

static zend_class_entry     *siphash_ce;
static zend_object_handlers  siphash_handlers;

#define Z_SIPHASH_P(zv) \
((siphash_object *)((char *)(Z_OBJ_P(zv)) - offsetof(siphash_object, std)))

PHP_METHOD(FlexId_Siphash, __construct)
{
	char *key;
	size_t keyLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(key, keyLen)
	ZEND_PARSE_PARAMETERS_END();

	siphash_object *obj = Z_SIPHASH_P(ZEND_THIS);

	if (keyLen != 16) {
		zend_argument_value_error(1, "Key must have 16 bytes");
		RETURN_THROWS();
	}

	const uint64_t *_key = (uint64_t *)key;
	const uint64_t k0 = _le64toh(_key[0]);
	const uint64_t k1 = _le64toh(_key[1]);

	obj->k[0] = k0 ^ (uint64_t)0x736f6d6570736575;
	obj->k[1]  = k1 ^ (uint64_t)0x646f72616e646f6d;
	obj->k[2]  = k0 ^ (uint64_t)0x6c7967656e657261;
	obj->k[3]  = k1 ^ (uint64_t)0x7465646279746573;
}

static zend_object *siphash_create_object(zend_class_entry *ce)
{
	siphash_object *obj = zend_object_alloc(sizeof(siphash_object), ce);
	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);
	obj->std.handlers = &siphash_handlers;
	return &obj->std;
}

static void siphash_free_object(zend_object *object)
{
	siphash_object *obj = (siphash_object *)((char *)object - offsetof(siphash_object, std));
	zend_object_std_dtor(&obj->std);
}

// siphash2-4, optimized for int + string input
PHP_METHOD(FlexId_Siphash, hash)
{
	char *data;
	size_t dataLen;
	zend_long id;
	bool isIdNull = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STRING(data, dataLen)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG_OR_NULL(id, isIdNull)
	ZEND_PARSE_PARAMETERS_END();

	siphash_object *obj = Z_SIPHASH_P(ZEND_THIS);

	const uint64_t *in = (uint64_t*)data;
	uint64_t mi, b;
	uint64_t v[4];
	memcpy(v, obj->k, sizeof(obj->k));

	if (isIdNull == false) {
		// add 8 bytes of data from id
		b = ((uint64_t)dataLen + 8) << 56;
		mi = _le64toh(id);
		v[3] ^= mi;
		DOUBLE_ROUND(v);
		v[0] ^= mi;
	} else {
		b = (uint64_t)dataLen << 56;
	}

	while (dataLen >= 8) {
		mi = _le64toh(*in);
		in++;
		dataLen -= 8;
		v[3] ^= mi;
		DOUBLE_ROUND(v);
		v[0] ^= mi;
	}

	uint64_t t = 0; uint8_t *pt = (uint8_t *)&t; uint8_t *m = (uint8_t *)in;
	switch (dataLen) {
		case 7: pt[6] = m[6];
		case 6: pt[5] = m[5];
		case 5: pt[4] = m[4];
		case 4: *((uint32_t*)&pt[0]) = *((uint32_t*)&m[0]); break;
		case 3: pt[2] = m[2];
		case 2: pt[1] = m[1];
		case 1: pt[0] = m[0];
	}
	b |= _le64toh(t);

	v[3] ^= b;
	DOUBLE_ROUND(v);
	v[0] ^= b;
	v[2] ^= 0xff;
	DOUBLE_ROUND(v);
	DOUBLE_ROUND(v);

	RETURN_LONG((zend_long)((v[0] ^ v[1]) ^ (v[2] ^ v[3])));
}

PHP_METHOD(FlexId_Siphash, __serialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be serialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_Siphash, __unserialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be unserialized");
	RETURN_THROWS();
}

int siphash_minit(int module_number)
{
	(void)module_number;

	siphash_ce = register_class_FlexId_Siphash();
	siphash_ce->create_object = siphash_create_object;

	memcpy(&siphash_handlers, zend_get_std_object_handlers(),
		   sizeof(zend_object_handlers));
	siphash_handlers.offset    = offsetof(siphash_object, std);
	siphash_handlers.free_obj  = siphash_free_object;
	siphash_handlers.clone_obj = NULL;

	return SUCCESS;
}

