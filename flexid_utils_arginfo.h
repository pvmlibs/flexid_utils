/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 60d95b8004eea90f3ebc6149314282e134e4f97e */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FlexId_Siphash___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Siphash_hash, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, num, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Siphash___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Siphash___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FlexId_Blake3___construct arginfo_class_FlexId_Siphash___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Blake3_hash, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, num, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, output_parts, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_class_FlexId_Blake3___serialize arginfo_class_FlexId_Siphash___serialize

#define arginfo_class_FlexId_Blake3___unserialize arginfo_class_FlexId_Siphash___unserialize

#define arginfo_class_FlexId_Sparx64___construct arginfo_class_FlexId_Siphash___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Sparx64_encrypt, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Sparx64_decrypt, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FlexId_Sparx64___serialize arginfo_class_FlexId_Siphash___serialize

#define arginfo_class_FlexId_Sparx64___unserialize arginfo_class_FlexId_Siphash___unserialize

#define arginfo_class_FlexId_Sparx128___construct arginfo_class_FlexId_Siphash___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Sparx128_encrypt, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, hipart, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, lowpart, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FlexId_Sparx128_decrypt arginfo_class_FlexId_Sparx128_encrypt

#define arginfo_class_FlexId_Sparx128___serialize arginfo_class_FlexId_Siphash___serialize

#define arginfo_class_FlexId_Sparx128___unserialize arginfo_class_FlexId_Siphash___unserialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FlexId_Encoder___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, alphabet)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, separator, "\' \'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Encoder_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Encoder_decode, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Encoder_doubleEncode, 0, 2, IS_STRING, 0)
	ZEND_ARG_INFO(0, hipart)
	ZEND_ARG_INFO(0, lowpart)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Encoder_doubleDecode, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_Encoder_getMaxEncodedLength, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FlexId_Encoder___serialize arginfo_class_FlexId_Siphash___serialize

#define arginfo_class_FlexId_Encoder___unserialize arginfo_class_FlexId_Siphash___unserialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FlexId_HashEncoder___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, alphabet)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, offset, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FlexId_HashEncoder_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

#define arginfo_class_FlexId_HashEncoder_decode arginfo_class_FlexId_Encoder_decode

#define arginfo_class_FlexId_HashEncoder_getMaxEncodedLength arginfo_class_FlexId_Encoder_getMaxEncodedLength

#define arginfo_class_FlexId_HashEncoder___serialize arginfo_class_FlexId_Siphash___serialize

#define arginfo_class_FlexId_HashEncoder___unserialize arginfo_class_FlexId_Siphash___unserialize


ZEND_METHOD(FlexId_Siphash, __construct);
ZEND_METHOD(FlexId_Siphash, hash);
ZEND_METHOD(FlexId_Siphash, __serialize);
ZEND_METHOD(FlexId_Siphash, __unserialize);
ZEND_METHOD(FlexId_Blake3, __construct);
ZEND_METHOD(FlexId_Blake3, hash);
ZEND_METHOD(FlexId_Blake3, __serialize);
ZEND_METHOD(FlexId_Blake3, __unserialize);
ZEND_METHOD(FlexId_Sparx64, __construct);
ZEND_METHOD(FlexId_Sparx64, encrypt);
ZEND_METHOD(FlexId_Sparx64, decrypt);
ZEND_METHOD(FlexId_Sparx64, __serialize);
ZEND_METHOD(FlexId_Sparx64, __unserialize);
ZEND_METHOD(FlexId_Sparx128, __construct);
ZEND_METHOD(FlexId_Sparx128, encrypt);
ZEND_METHOD(FlexId_Sparx128, decrypt);
ZEND_METHOD(FlexId_Sparx128, __serialize);
ZEND_METHOD(FlexId_Sparx128, __unserialize);
ZEND_METHOD(FlexId_Encoder, __construct);
ZEND_METHOD(FlexId_Encoder, encode);
ZEND_METHOD(FlexId_Encoder, decode);
ZEND_METHOD(FlexId_Encoder, doubleEncode);
ZEND_METHOD(FlexId_Encoder, doubleDecode);
ZEND_METHOD(FlexId_Encoder, getMaxEncodedLength);
ZEND_METHOD(FlexId_Encoder, __serialize);
ZEND_METHOD(FlexId_Encoder, __unserialize);
ZEND_METHOD(FlexId_HashEncoder, __construct);
ZEND_METHOD(FlexId_HashEncoder, encode);
ZEND_METHOD(FlexId_HashEncoder, decode);
ZEND_METHOD(FlexId_HashEncoder, getMaxEncodedLength);
ZEND_METHOD(FlexId_HashEncoder, __serialize);
ZEND_METHOD(FlexId_HashEncoder, __unserialize);


static const zend_function_entry class_FlexId_Siphash_methods[] = {
	ZEND_ME(FlexId_Siphash, __construct, arginfo_class_FlexId_Siphash___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Siphash, hash, arginfo_class_FlexId_Siphash_hash, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Siphash, __serialize, arginfo_class_FlexId_Siphash___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Siphash, __unserialize, arginfo_class_FlexId_Siphash___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FlexId_Blake3_methods[] = {
	ZEND_ME(FlexId_Blake3, __construct, arginfo_class_FlexId_Blake3___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Blake3, hash, arginfo_class_FlexId_Blake3_hash, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Blake3, __serialize, arginfo_class_FlexId_Blake3___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Blake3, __unserialize, arginfo_class_FlexId_Blake3___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FlexId_Sparx64_methods[] = {
	ZEND_ME(FlexId_Sparx64, __construct, arginfo_class_FlexId_Sparx64___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx64, encrypt, arginfo_class_FlexId_Sparx64_encrypt, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx64, decrypt, arginfo_class_FlexId_Sparx64_decrypt, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx64, __serialize, arginfo_class_FlexId_Sparx64___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx64, __unserialize, arginfo_class_FlexId_Sparx64___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FlexId_Sparx128_methods[] = {
	ZEND_ME(FlexId_Sparx128, __construct, arginfo_class_FlexId_Sparx128___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx128, encrypt, arginfo_class_FlexId_Sparx128_encrypt, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx128, decrypt, arginfo_class_FlexId_Sparx128_decrypt, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx128, __serialize, arginfo_class_FlexId_Sparx128___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Sparx128, __unserialize, arginfo_class_FlexId_Sparx128___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FlexId_Encoder_methods[] = {
	ZEND_ME(FlexId_Encoder, __construct, arginfo_class_FlexId_Encoder___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, encode, arginfo_class_FlexId_Encoder_encode, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, decode, arginfo_class_FlexId_Encoder_decode, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, doubleEncode, arginfo_class_FlexId_Encoder_doubleEncode, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, doubleDecode, arginfo_class_FlexId_Encoder_doubleDecode, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, getMaxEncodedLength, arginfo_class_FlexId_Encoder_getMaxEncodedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, __serialize, arginfo_class_FlexId_Encoder___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_Encoder, __unserialize, arginfo_class_FlexId_Encoder___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FlexId_HashEncoder_methods[] = {
	ZEND_ME(FlexId_HashEncoder, __construct, arginfo_class_FlexId_HashEncoder___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_HashEncoder, encode, arginfo_class_FlexId_HashEncoder_encode, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_HashEncoder, decode, arginfo_class_FlexId_HashEncoder_decode, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_HashEncoder, getMaxEncodedLength, arginfo_class_FlexId_HashEncoder_getMaxEncodedLength, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_HashEncoder, __serialize, arginfo_class_FlexId_HashEncoder___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(FlexId_HashEncoder, __unserialize, arginfo_class_FlexId_HashEncoder___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_FlexId_Siphash(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FlexId", "Siphash", class_FlexId_Siphash_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_FlexId_Blake3(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FlexId", "Blake3", class_FlexId_Blake3_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_FlexId_Sparx64(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FlexId", "Sparx64", class_FlexId_Sparx64_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_FlexId_Sparx128(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FlexId", "Sparx128", class_FlexId_Sparx128_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_FlexId_Encoder(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FlexId", "Encoder", class_FlexId_Encoder_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_FlexId_HashEncoder(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FlexId", "HashEncoder", class_FlexId_HashEncoder_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
