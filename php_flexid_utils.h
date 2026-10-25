/* flexid extension for PHP */

#ifndef PHP_FLEXID_UTILS_H
# define PHP_FLEXID_UTILS_H

extern zend_module_entry flexid_utils_module_entry;
# define phpext_flexid_utils_ptr &flexid_utils_module_entry

#ifndef PHP_FLEXID_UTILS_VERSION
# define PHP_FLEXID_UTILS_VERSION "dev"
#endif

# if defined(ZTS) && defined(COMPILE_DL_FLEXID_UTILS)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

int flexid_minit(int module_number);
int siphash_minit(int module_number);
int blake3_minit(int module_number);
int sparx64_minit(int module_number);
int sparx128_minit(int module_number);
int encoder_minit(int module_number);
int hash_encoder_minit(int module_number);

#define MAX_SIGNED_INT (uint64_t)LLONG_MAX

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define _le64toh(x) ((uint64_t)(x))
#elif defined(_WIN32)
// Windows is always little endian, unless you're on xbox360
// http://msdn.microsoft.com/en-us/library/b0084kay(v=vs.80).aspx
#  define _le64toh(x) ((uint64_t)(x))
#elif defined(__APPLE__)
#  include <libkern/OSByteOrder.h>
#  define _le64toh(x) OSSwapLittleToHostInt64(x)
#else

// See: http://sourceforge.net/p/predef/wiki/Endianness/
#  if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#    include <sys/endian.h>
#  else
#    include <endian.h>
#  endif
#  if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
__BYTE_ORDER == __LITTLE_ENDIAN
#    define _le64toh(x) ((uint64_t)(x))
#  else
#    define _le64toh(x) le64toh(x)
#  endif

#endif

#ifdef __has_attribute
#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif
#endif
#ifndef ALWAYS_INLINE
#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif
#endif

#endif	/* PHP_FLEXID_H */
