#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

int find_key_avx512(const uint16_t *keys, uint16_t search_key) {
    __m512i search_key_vec = _mm512_set1_epi16(search_key);
    __m512i keys_vec = _mm512_loadu_si512((__m512i *)keys);
    __mmask32 mask = _mm512_cmpeq_epi16_mask(keys_vec, search_key_vec);

    if (mask != 0) {
        return __builtin_ctz(mask);
    }
    return -1;
}

int main() {
    uint16_t keys[32] = {
        100, 200, 300, 400, 500, 600, 700, 800,
        900, 1000, 1100, 1200, 1300, 1400, 1500, 1600,
        1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400,
        2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200
    };

    uint16_t search_key = 1500;
    int index = find_key_avx512(keys, search_key);

    if (index != -1) {
        printf("Key %d found at position %d\n", search_key, index);
    } else {
        printf("Key %d not found\n", search_key);
    }

    return 0;
}