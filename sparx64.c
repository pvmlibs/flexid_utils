#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_flexid_utils.h"
#include "flexid_utils_arginfo.h"

#define N_STEPS 8
#define ROUNDS_PER_STEPS 3
#define N_BRANCHES 2
#define K_SIZE 4
#define L L_2
#define L_inv L_2_inv
#define K_perm K_perm_64_128

#define ROTL(x, n) (((x) << n) | ((x) >> (16 - (n))))
#define SWAP(x, y) tmp = x; x = y; y = tmp

typedef struct {
	uint16_t k[N_BRANCHES*N_STEPS+1][2*ROUNDS_PER_STEPS];
	zend_object std;
} sparx64_object;

static zend_class_entry     *sparx64_ce;
static zend_object_handlers  sparx64_handlers;

#define Z_SPARX64_P(zv) \
((sparx64_object *)((char *)(Z_OBJ_P(zv)) - offsetof(sparx64_object, std)))

static zend_object *sparx64_create_object(zend_class_entry *ce)
{
	sparx64_object *obj = zend_object_alloc(sizeof(sparx64_object), ce);
	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);
	obj->std.handlers = &sparx64_handlers;
	return &obj->std;
}

static void sparx64_free_object(zend_object *object)
{
	sparx64_object *obj = (sparx64_object *)((char *)object - offsetof(sparx64_object, std));
	zend_object_std_dtor(&obj->std);
}

/* One keyless round of SPECK-32 */
static ALWAYS_INLINE void A(uint16_t * l, uint16_t * r)
{
	(*l) = ROTL((*l), 9);
	(*l) += (*r);
	(*r) = ROTL((*r), 2);
	(*r) ^= (*l);
}

/* One keyless inverse round of SPECK-32 */
static ALWAYS_INLINE void A_inv(uint16_t * l, uint16_t * r)
{
	(*r) ^= (*l);
	(*r) = ROTL((*r), 14);
	(*l) -= (*r);
	(*l) = ROTL((*l), 7);
}

/* The linear layers */

static void L_2(uint16_t * x)
{
	uint16_t tmp = ROTL((x[0] ^ x[1]), 8);
	x[2] ^= x[0] ^ tmp;
	x[3] ^= x[1] ^ tmp;
	SWAP(x[0], x[2]);
	SWAP(x[1], x[3]);
}

static void L_2_inv(uint16_t * x)
{
	uint16_t tmp;
	SWAP(x[0], x[2]);
	SWAP(x[1], x[3]);
	tmp = ROTL(x[0] ^ x[1], 8);
	x[2] ^= x[0] ^ tmp;
	x[3] ^= x[1] ^ tmp;
}


static void K_perm_64_128(uint16_t * k, uint16_t c)
{
	uint16_t tmp_0, tmp_1, i;
	/* Misty-like transformation */
	A(k+0, k+1);
	k[2] += k[0];
	k[3] += k[1];
	k[7] += c;
	/* Branch rotation */
	tmp_0 = k[6];
	tmp_1 = k[7];
	for (i=7 ; i>=2 ; i--)
	{
		k[i] = k[i-2];
	}
	k[0] = tmp_0;
	k[1] = tmp_1;
}

/* Takes a 128 bit master key and turns it into 2*(N_STEPS+1) subkeys of 96 bits */
static void key_schedule(uint16_t subkeys[][2*ROUNDS_PER_STEPS], uint16_t * master_key)
{
	uint8_t c, i;
	for (c=0 ; c<(N_BRANCHES*N_STEPS+1) ; c++)
	{
		for (i=0 ; i<2*ROUNDS_PER_STEPS ; i++)
		{
			subkeys[c][i] = master_key[i];
		}
		K_perm(master_key, c+1);
	}
}

PHP_METHOD(FlexId_Sparx64, __construct)
{
	char *key;
	size_t keyLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(key, keyLen)
	ZEND_PARSE_PARAMETERS_END();

	sparx64_object *obj = Z_SPARX64_P(ZEND_THIS);

	if (keyLen != 16) {
		zend_argument_value_error(2, "Key must have 16 bytes");
		RETURN_THROWS();
	}

	uint16_t _key[2*K_SIZE];
	memcpy(_key, key, sizeof _key);

	key_schedule(obj->k, _key);
}

PHP_METHOD(FlexId_Sparx64, encrypt)
{
	zend_long num;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END();

	uint64_t value = (uint64_t)num;
	uint8_t s, r, b;
	uint16_t x[N_BRANCHES * 2];
	memcpy(x, &value, sizeof value);

	sparx64_object *obj = Z_SPARX64_P(ZEND_THIS);
	const uint16_t (*k)[2*ROUNDS_PER_STEPS] = obj->k;

	s=0; b=0; r=0;
	for (s=0 ; s<N_STEPS ; s++)
	{
		for (b=0 ; b<N_BRANCHES ; b++)
		{
			uint16_t x2b = x[2*b  ];
			uint16_t x2b1 = x[2*b + 1];

			for (r=0 ; r<ROUNDS_PER_STEPS ; r++)
			{
				// x[2*b  ] ^= k[N_BRANCHES*s + b][2*r    ];
				// x[2*b+1] ^= k[N_BRANCHES*s + b][2*r + 1];
				// A(x + 2*b, x + 2*b+1);

				x2b ^= k[N_BRANCHES*s + b][2*r    ];
				x2b1 ^= k[N_BRANCHES*s + b][2*r + 1];

				/* One keyless round of SPECK-32 */
				x2b = ROTL(x2b, 9);
				x2b += x2b1;
				x2b1 = ROTL(x2b1, 2);
				x2b1 ^= x2b;
			}
			x[2*b  ] = x2b;
			x[2*b + 1] = x2b1;
		}
		L_2(x);
	}

	for (b=0 ; b<N_BRANCHES ; b++)
	{
		x[2*b  ] ^= k[N_BRANCHES*N_STEPS][2*b  ];
		x[2*b+1] ^= k[N_BRANCHES*N_STEPS][2*b+1];
	}

	memcpy(&value, x, sizeof value);
	RETURN_LONG((zend_long)value);
}

PHP_METHOD(FlexId_Sparx64, decrypt)
{
	zend_long data;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(data)
	ZEND_PARSE_PARAMETERS_END();

	uint64_t value = (uint64_t)data;
	int8_t s, r, b;
	uint16_t x[N_BRANCHES * 2];
	memcpy(x, &value, sizeof value);

	sparx64_object *obj = Z_SPARX64_P(ZEND_THIS);
	const uint16_t (*k)[2*ROUNDS_PER_STEPS] = obj->k;

	for (b=0 ; b<N_BRANCHES ; b++)
	{
		x[2*b  ] ^= k[N_BRANCHES*N_STEPS][2*b  ];
		x[2*b+1] ^= k[N_BRANCHES*N_STEPS][2*b+1];
	}

	for (s=N_STEPS-1 ; s >= 0 ; s--)
	{
		L_inv(x);
		for (b=0 ; b<N_BRANCHES ; b++)
		{
			uint16_t x2b = x[2*b  ];
			uint16_t x2b1 = x[2*b + 1];

			for (r=ROUNDS_PER_STEPS-1 ; r >= 0 ; r--)
			{
				/* One keyless inverse round of SPECK-32 */
				x2b1 ^= x2b;
				x2b1 = ROTL(x2b1, 14);
				x2b -= x2b1;
				x2b = ROTL(x2b, 7);

				x2b ^= k[N_BRANCHES*s + b][2*r    ];
				x2b1 ^= k[N_BRANCHES*s + b][2*r + 1];

			}
			x[2*b  ] = x2b;
			x[2*b + 1] = x2b1;
		}
	}

	memcpy(&value, x, sizeof value);
	RETURN_LONG((zend_long)value);
}

PHP_METHOD(FlexId_Sparx64, __serialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be serialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_Sparx64, __unserialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be unserialized");
	RETURN_THROWS();
}

int sparx64_minit(int module_number)
{
	(void)module_number;

	sparx64_ce = register_class_FlexId_Sparx64();
	sparx64_ce->create_object = sparx64_create_object;

	memcpy(&sparx64_handlers, zend_get_std_object_handlers(),
		   sizeof(zend_object_handlers));
	sparx64_handlers.offset    = offsetof(sparx64_object, std);
	sparx64_handlers.free_obj  = sparx64_free_object;
	sparx64_handlers.clone_obj = NULL;

	return SUCCESS;
}

