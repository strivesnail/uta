#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <immintrin.h>

#define VECTOR_SIZE 32
#define ARRAY_SIZE 1000000
#define NUM_RUNS 50

void populate_array(uint16_t (*array)[VECTOR_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int j = 0; j < VECTOR_SIZE; j++) {
            array[i][j] = rand() % 65536;
        }
    }
}

void generate_random_indices(int *indices) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        indices[i] = i;
    }
    for (int i = ARRAY_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
}

void avx512_test(uint16_t (*array)[VECTOR_SIZE], uint16_t target_key, int *indices) {
    __m512i target_vec = _mm512_set1_epi16(target_key);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int index = indices[i];
        __m512i vec = _mm512_loadu_si512((__m512i *)array[index]);
        __mmask32 mask = _mm512_cmpeq_epi16_mask(vec, target_vec);
        if (mask) {
            // Match logic here
        }
    }
}

int main() {
    uint16_t (*array)[VECTOR_SIZE] = malloc(ARRAY_SIZE * sizeof(*array));
    int *indices = malloc(ARRAY_SIZE * sizeof(int));

    if (!array || !indices) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    populate_array(array);
    generate_random_indices(indices);

    uint16_t target_key = rand() % 65536;
    avx512_test(array, target_key, indices);

    free(array);
    free(indices);
    return 0;
}