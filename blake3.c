#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_flexid_utils.h"
#include "flexid_utils_arginfo.h"

#define CHUNK_START  (1u << 0)
#define CHUNK_END    (1u << 1)
#define ROOT         (1u << 3)
#define KEYED_HASH   (1u << 4)

typedef struct {
	uint32_t cv[8];
	zend_object std;
} blake3_object;

static zend_class_entry     *blake3_ce;
static zend_object_handlers  blake3_handlers;

#define Z_BLAKE3_P(zv) \
((blake3_object *)((char *)(Z_OBJ_P(zv)) - offsetof(blake3_object, std)))

static  uint32_t IV[4] = {
	(uint32_t)0x6A09E667, (uint32_t)0xBB67AE85, (uint32_t)0x3C6EF372, (uint32_t)0xA54FF53A,
};

static const uint8_t MSG_SCHEDULE[7][16] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 },
	{ 2, 6, 3,10, 7, 0, 4,13, 1,11,12, 5, 9,14,15, 8 },
	{ 3, 4,10,12,13, 2, 7,14, 6, 5, 9, 0,11,15, 8, 1 },
	{10, 7,12, 9,14, 3,13,15, 4, 0,11, 2, 5, 8, 1, 6 },
	{12,13, 9,11,15,10,14, 8, 7, 2, 5, 3, 0, 1, 6, 4 },
	{ 9,14,11, 5, 8,12,15, 1,13, 3, 0,10, 2, 6, 4, 7 },
	{11,15, 5, 0, 1, 9, 8, 6,14,10, 2,12, 3, 4, 7,13 }
};

static inline uint32_t rotr32(uint32_t w, uint32_t c) {
	return (w >> c) | (w << (32 - c));
}

static inline void g(uint32_t *state, size_t a, size_t b, size_t c, size_t d,
					 uint32_t x, uint32_t y) {
	state[a] = state[a] + state[b] + x;
	state[d] = rotr32(state[d] ^ state[a], 16);
	state[c] = state[c] + state[d];
	state[b] = rotr32(state[b] ^ state[c], 12);
	state[a] = state[a] + state[b] + y;
	state[d] = rotr32(state[d] ^ state[a], 8);
	state[c] = state[c] + state[d];
	state[b] = rotr32(state[b] ^ state[c], 7);
}

static inline void round_fn(uint32_t state[16], const uint32_t *msg, size_t round) {
	const uint8_t *schedule = MSG_SCHEDULE[round];
	g(state, 0, 4, 8,12, msg[schedule[0]], msg[schedule[1]]);
	g(state, 1, 5, 9,13, msg[schedule[2]], msg[schedule[3]]);
	g(state, 2, 6,10,14, msg[schedule[4]], msg[schedule[5]]);
	g(state, 3, 7,11,15, msg[schedule[6]], msg[schedule[7]]);

	g(state, 0, 5,10,15, msg[schedule[8]], msg[schedule[9]]);
	g(state, 1, 6,11,12, msg[schedule[10]], msg[schedule[11]]);
	g(state, 2, 7, 8,13, msg[schedule[12]], msg[schedule[13]]);
	g(state, 3, 4, 9,14, msg[schedule[14]], msg[schedule[15]]);
}

/* One compression, updates cv in‑place */
static void compress_in_place(
			uint32_t cv[8],
			const uint32_t block_words[16],
			uint32_t block_len,
			uint32_t flags)
{
	uint32_t state[16];

	state[0] = cv[0]; state[1] = cv[1]; state[2] = cv[2]; state[3] = cv[3];
	state[4] = cv[4]; state[5] = cv[5]; state[6] = cv[6]; state[7] = cv[7];
	state[8] = IV[0]; state[9] = IV[1]; state[10] = IV[2]; state[11] = IV[3];
	state[12] = 0;
	state[13] = 0;
	state[14] = block_len;
	state[15] = flags;

	round_fn(state, block_words, 0);
	round_fn(state, block_words, 1);
	round_fn(state, block_words, 2);
	round_fn(state, block_words, 3);
	round_fn(state, block_words, 4);
	round_fn(state, block_words, 5);
	round_fn(state, block_words, 6);

	cv[0] = state[0] ^ state[8];
	cv[1] = state[1] ^ state[9];
	cv[2] = state[2] ^ state[10];
	cv[3] = state[3] ^ state[11];
	cv[4] = state[4] ^ state[12];
	cv[5] = state[5] ^ state[13];
	cv[6] = state[6] ^ state[14];
	cv[7] = state[7] ^ state[15];
}

PHP_METHOD(FlexId_Blake3, __construct)
{
	char *key;
	size_t keyLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(key, keyLen)
	ZEND_PARSE_PARAMETERS_END();

	blake3_object *obj = Z_BLAKE3_P(ZEND_THIS);

	if (keyLen != 32) {
		zend_argument_value_error(1, "Key must have 32 bytes");
		RETURN_THROWS();
	}

	const uint32_t *k = (uint32_t *)key;

	memcpy(obj->cv, k, sizeof obj->cv);
}

static zend_object *blake3_create_object(zend_class_entry *ce)
{
	blake3_object *obj = zend_object_alloc(sizeof(blake3_object), ce);
	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);
	obj->std.handlers = &blake3_handlers;
	return &obj->std;
}

static void blake3_free_object(zend_object *object)
{
	blake3_object *obj = (blake3_object *)((char *)object - offsetof(blake3_object, std));
	zend_object_std_dtor(&obj->std);
}

// keyed blake3, optimized for int + string input
PHP_METHOD(FlexId_Blake3, hash)
{
	char *data;
	size_t data_length;
	zend_long id, output_parts = 1;
	bool isIdNull = true;

	ZEND_PARSE_PARAMETERS_START(1, 3)
	Z_PARAM_STRING(data, data_length)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG_OR_NULL(id, isIdNull)
	Z_PARAM_LONG(output_parts)
	ZEND_PARSE_PARAMETERS_END();

	if (data_length > 1024 || (data_length > 1016 && isIdNull == false)) {
		zend_argument_value_error(1, "Total input data must be <= 1024 bytes");
		RETURN_THROWS();
	}

	if (output_parts < 1 || output_parts > 4) {
		zend_argument_value_error(1, "Output parts must be between 1 and 4");
		RETURN_THROWS();
	}

	blake3_object *obj = Z_BLAKE3_P(ZEND_THIS);

	uint32_t cv[8];
	memcpy(cv, obj->cv, sizeof(cv));

	uint32_t block[16] = {0};
	uint32_t pos = 0;
	uint8_t offset = 0;
	uint64_t num = (uint64_t)id;

	if (isIdNull == false) {
		memcpy(block, &num, sizeof num);
		pos = sizeof num;
		offset = pos;
	}

	uint64_t data_len_first_block = MIN(64 - pos, data_length);

	if (data_length > 0) {
		memcpy(block + (pos >> 2), data, data_len_first_block);
		pos += data_len_first_block;
	}

	bool more_data = data_length + offset > 64;
	uint8_t flags = KEYED_HASH;

	if (more_data == false) {
		compress_in_place(cv, block, pos, flags | CHUNK_START | CHUNK_END | ROOT);
	}
	else {
		compress_in_place(cv, block, pos, flags | CHUNK_START);
		uint64_t n_blocks = (data_length + offset - 1) >> 6;

		for (uint64_t i = 1; i <= n_blocks; i++) {
			uint32_t block[16] = {0};
			uint32_t off = i * 64 - offset;
			uint32_t remaining = data_length - off;
			uint32_t block_len = (remaining >= 64) ? 64 : remaining;

			memcpy(block, data + off, block_len);

			if (i == n_blocks) {
				flags |= CHUNK_END;
				flags |= ROOT;
			}

			compress_in_place(cv, block, block_len, flags);
		}
	}

	array_init_size(return_value, output_parts);
	uint64_t u64vals[4];
	memcpy(u64vals, cv, sizeof(cv));

	for (zend_long i = 0; i < output_parts; i++) {
		add_index_long(return_value, i, (zend_long)u64vals[i]);
	}
}

PHP_METHOD(FlexId_Blake3, __serialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be serialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_Blake3, __unserialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be unserialized");
	RETURN_THROWS();
}


int blake3_minit(int module_number)
{
	(void)module_number;

	blake3_ce = register_class_FlexId_Blake3();
	blake3_ce->create_object = blake3_create_object;

	memcpy(&blake3_handlers, zend_get_std_object_handlers(),
		   sizeof(zend_object_handlers));
	blake3_handlers.offset    = offsetof(blake3_object, std);
	blake3_handlers.free_obj  = blake3_free_object;
	blake3_handlers.clone_obj = NULL;

	return SUCCESS;
}

