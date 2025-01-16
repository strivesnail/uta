#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <immintrin.h>

#define VECTOR_SIZE 32 
#define ARRAY_SIZE 100000 

void populate_array(uint16_t array[ARRAY_SIZE][VECTOR_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int j = 0; j < VECTOR_SIZE; j++) {
            array[i][j] = rand() % 65536;
        }
    }
}

void avx512_test(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key) {
    __m512i target_vec = _mm512_set1_epi16(target_key);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        __m512i vec = _mm512_loadu_si512((__m512i *)array[i]);
        __mmask32 mask = _mm512_cmpeq_epi16_mask(vec, target_vec);

        if (mask) {
            //printf("Match found in vector %d\n", i);
        }
    }
}

void key_to_key_test(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int j = 0; j < VECTOR_SIZE; j++) {
            if (array[i][j] == target_key) {
                //printf("Match found in vector %d, key %d\n", i, j);
            }
        }
    }
}

void single_key_test(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (array[i][0] == target_key) {
            //printf("Match found in vector %d, first key\n", i);
        }
    }
}

void measure_time(void (*test_func)(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t), uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key, const char* test_name) {
    clock_t start_time = clock();
    test_func(array, target_key);
    clock_t end_time = clock();

    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("%s: Time taken: %f seconds\n", test_name, time_taken);
}

int main() {
    uint16_t array[ARRAY_SIZE][VECTOR_SIZE];
    populate_array(array);

    uint16_t target_key = rand() % 65536;

    printf("AVX-512 Test:\n");
    measure_time(avx512_test, array, target_key, "AVX-512 Test");

    printf("\nKey-to-Key Test (All Keys):\n");
    measure_time(key_to_key_test, array, target_key, "Key-to-Key Test (All Keys)");

    printf("\nKey-to-Key Test (First Key Only):\n");
    measure_time(single_key_test, array, target_key, "Key-to-Key Test (First Key Only)");

    return 0;
}