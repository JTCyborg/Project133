#ifndef BSWAP_H
#define BSWAP_H

#include <stdint.h>

/* Portable byte swaps: avoid GNU statement expressions so this header
 * also compiles with MSVC's C compiler. */
static inline uint16_t bswap16(uint16_t x) {
        return (uint16_t)((x << 8) | (x >> 8));
}

static inline uint32_t bswap32(uint32_t x) {
        return ((x & UINT32_C(0x000000ff)) << 24) |
               ((x & UINT32_C(0x0000ff00)) << 8) |
               ((x & UINT32_C(0x00ff0000)) >> 8) |
               ((x & UINT32_C(0xff000000)) >> 24);
}

static inline uint64_t bswap64(uint64_t x) {
        return ((x & UINT64_C(0x00000000000000ff)) << 56) |
               ((x & UINT64_C(0x000000000000ff00)) << 40) |
               ((x & UINT64_C(0x0000000000ff0000)) << 24) |
               ((x & UINT64_C(0x00000000ff000000)) << 8) |
               ((x & UINT64_C(0x000000ff00000000)) >> 8) |
               ((x & UINT64_C(0x0000ff0000000000)) >> 24) |
               ((x & UINT64_C(0x00ff000000000000)) >> 40) |
               ((x & UINT64_C(0xff00000000000000)) >> 56);
}

#define bswap_16(x) bswap16((uint16_t)(x))
#define bswap_32(x) bswap32((uint32_t)(x))
#define bswap_64(x) bswap64((uint64_t)(x))

static inline void bswap16s(uint16_t *s) { *s = bswap16(*s); }
static inline void bswap32s(uint32_t *s) { *s = bswap32(*s); }
static inline void bswap64s(uint64_t *s) { *s = bswap64(*s); }

#if defined(WORDS_BIGENDIAN)
#define be_bswap(v, size) (v)
#define le_bswap(v, size) bswap##size(v)
#define be_bswaps(v, size)
#define le_bswaps(p, size) *p = bswap##size(*p);
#else
#define le_bswap(v, size) (v)
#define be_bswap(v, size) bswap##size(v)
#define le_bswaps(v, size)
#define be_bswaps(p, size) *p = bswap##size(*p);
#endif

#define CPU_CONVERT(endian, size, type)                                                                                          \
        static inline type endian##size##_to_cpu(type v) { return endian##_bswap(v, size); }                                     \
        static inline type cpu_to_##endian##size(type v) { return endian##_bswap(v, size); }                                     \
        static inline void endian##size##_to_cpus(type *p) { endian##_bswaps(p, size) }                                          \
        static inline void cpu_to_##endian##size##s(type *p) { endian##_bswaps(p, size) }                                        \
        static inline type endian##size##_to_cpup(const type *p) { return endian##size##_to_cpu(*p); }                           \
        static inline void cpu_to_##endian##size##p(type *p) { *p = cpu_to_##endian##size(*p); }

#endif /* BSWAP_H */
