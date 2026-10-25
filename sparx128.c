#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_flexid_utils.h"
#include "flexid_utils_arginfo.h"

#define N_STEPS 10
#define ROUNDS_PER_STEPS 4
#define N_BRANCHES 4
#define K_SIZE 8

#define ROTL(x, n) (((x) << n) | ((x) >> (16 - (n))))
#define SWAP(x, y) tmp = x; x = y; y = tmp

typedef struct {
	uint16_t k[N_BRANCHES*N_STEPS+1][2*ROUNDS_PER_STEPS];
	zend_object std;
} sparx128_object;

static zend_class_entry     *sparx128_ce;
static zend_object_handlers  sparx128_handlers;

#define Z_SPARX128_P(zv) \
((sparx128_object *)((char *)(Z_OBJ_P(zv)) - offsetof(sparx128_object, std)))

static zend_object *sparx128_create_object(zend_class_entry *ce)
{
	sparx128_object *obj = zend_object_alloc(sizeof(sparx128_object), ce);
	zend_object_std_init(&obj->std, ce);
	object_properties_init(&obj->std, ce);
	obj->std.handlers = &sparx128_handlers;
	return &obj->std;
}

static void sparx128_free_object(zend_object *object)
{
	sparx128_object *obj = (sparx128_object *)((char *)object - offsetof(sparx128_object, std));
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

static void L_4(uint16_t * x)
{
    uint16_t tmp = x[0] ^ x[1] ^ x[2] ^ x[3];
    tmp = ROTL(tmp, 8);

    x[4] ^= x[2] ^ tmp;
    x[5] ^= x[1] ^ tmp;
    x[6] ^= x[0] ^ tmp;
    x[7] ^= x[3] ^ tmp;

    SWAP(x[0], x[4]);
    SWAP(x[1], x[5]);
    SWAP(x[2], x[6]);
    SWAP(x[3], x[7]);
}

static void L_4_inv(uint16_t * x)
{
    uint16_t tmp;
    SWAP(x[0], x[4]);
    SWAP(x[1], x[5]);
    SWAP(x[2], x[6]);
    SWAP(x[3], x[7]);

    tmp = x[0] ^ x[1] ^ x[2] ^ x[3];
    tmp = ROTL(tmp, 8);
    x[4] ^= x[2] ^ tmp;
    x[5] ^= x[1] ^ tmp;
    x[6] ^= x[0] ^ tmp;
    x[7] ^= x[3] ^ tmp;
}


void K_perm_128_256(uint16_t * k, uint16_t c)
{
    uint16_t tmp[6], i;
    /* Misty-like transformation */
    A(k+0, k+1);
    k[2] += k[0];
    k[3] += k[1];
    A(k+8, k+9);
    k[10] += k[8];
    k[11] += k[9] + c;
    /* Branch rotation */
    for (i=0 ; i<6 ; i++)
    {
        tmp[i] = k[10+i];
    }
    for (i=15 ; i>=6 ; i--)
    {
        k[i] = k[i-6];
    }
    for (i=0 ; i<6 ; i++)
    {
        k[i] = tmp[i];
    }
}

void key_schedule_256(uint16_t subkeys[][2*ROUNDS_PER_STEPS], uint16_t * master_key)
{
	uint8_t c, i;
	for (c=0 ; c<(N_BRANCHES*N_STEPS+1) ; c++)
	{
		for (i=0 ; i<2*ROUNDS_PER_STEPS ; i++)
		{
			subkeys[c][i] = master_key[i];
		}
		K_perm_128_256(master_key, c+1);
	}
}

PHP_METHOD(FlexId_Sparx128, __construct)
{
	char *key;
	size_t keyLen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(key, keyLen)
	ZEND_PARSE_PARAMETERS_END();

	sparx128_object *obj = Z_SPARX128_P(ZEND_THIS);

	if (keyLen != 32) {
		zend_argument_value_error(2, "Key must have 32 bytes");
		RETURN_THROWS();
	}

	uint16_t _key[2*K_SIZE];
	memcpy(_key, key, sizeof _key);

	key_schedule_256(obj->k, _key);
}

PHP_METHOD(FlexId_Sparx128, encrypt)
{
	zend_long dataHi, dataLow;
	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_LONG(dataHi)
	Z_PARAM_LONG(dataLow)
	ZEND_PARSE_PARAMETERS_END();

	uint64_t value[2];
	value[0] = (uint64_t)dataHi;
	value[1] = (uint64_t)dataLow;
	uint8_t s, r, b;
	uint16_t x[N_BRANCHES * 2];
	memcpy(x, &value, sizeof value);

	sparx128_object *obj = Z_SPARX128_P(ZEND_THIS);
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
		L_4(x);
	}

	for (b=0 ; b<N_BRANCHES ; b++)
	{
		x[2*b  ] ^= k[N_BRANCHES*N_STEPS][2*b  ];
		x[2*b+1] ^= k[N_BRANCHES*N_STEPS][2*b+1];
	}

	memcpy(value, x, sizeof value);

	array_init_size(return_value, 2);
	add_index_long(return_value, 0, (zend_long)(*(value)));
	add_index_long(return_value, 1, (zend_long)(*(value + 1)));
}

PHP_METHOD(FlexId_Sparx128, decrypt)
{
	zend_long dataHi, dataLow;
	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_LONG(dataHi)
	Z_PARAM_LONG(dataLow)
	ZEND_PARSE_PARAMETERS_END();

	uint64_t value[2];
	value[0] = (uint64_t)dataHi;
	value[1] = (uint64_t)dataLow;
	int8_t s, r, b;
	uint16_t x[N_BRANCHES * 2];
	memcpy(x, &value, sizeof value);

    sparx128_object *obj = Z_SPARX128_P(ZEND_THIS);
    const uint16_t (*k)[2*ROUNDS_PER_STEPS] = obj->k;

	for (b=0 ; b<N_BRANCHES ; b++)
	{
		x[2*b  ] ^= k[N_BRANCHES*N_STEPS][2*b  ];
		x[2*b+1] ^= k[N_BRANCHES*N_STEPS][2*b+1];
	}

	for (s=N_STEPS-1 ; s >= 0 ; s--)
	{
		L_4_inv(x);
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

    memcpy(value, x, sizeof value);

	array_init_size(return_value, 2);
	add_index_long(return_value, 0, (zend_long)(*(value)));
	add_index_long(return_value, 1, (zend_long)(*(value + 1)));
}

PHP_METHOD(FlexId_Sparx128, __serialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be serialized");
	RETURN_THROWS();
}

PHP_METHOD(FlexId_Sparx128, __unserialize)
{
	zend_throw_error(
		NULL,
		"This class cannot be unserialized");
	RETURN_THROWS();
}

int sparx128_minit(int module_number)
{
	(void)module_number;

	sparx128_ce = register_class_FlexId_Sparx128();
	sparx128_ce->create_object = sparx128_create_object;

	memcpy(&sparx128_handlers, zend_get_std_object_handlers(),
		   sizeof(zend_object_handlers));
	sparx128_handlers.offset    = offsetof(sparx128_object, std);
	sparx128_handlers.free_obj  = sparx128_free_object;
	sparx128_handlers.clone_obj = NULL;

	return SUCCESS;
}

