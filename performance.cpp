#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <immintrin.h>

#define VECTOR_SIZE 32      // 每个向量包含 32 个键
#define ARRAY_SIZE 10000   // 数组中有 100,000 个向量
#define NUM_RUNS 50          // 每种场景运行的次数

// 随机填充数组
void populate_array(uint16_t array[ARRAY_SIZE][VECTOR_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int j = 0; j < VECTOR_SIZE; j++) {
            array[i][j] = rand() % 65536; // 随机生成 16 位整数
        }
    }
}

// 生成随机索引
void generate_random_indices(int indices[ARRAY_SIZE]) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        indices[i] = i;
    }
    // 打乱索引
    for (int i = ARRAY_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
}

// 场景 1：AVX-512 单线程测试
void avx512_test(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key, int indices[ARRAY_SIZE]) {
    __m512i target_vec = _mm512_set1_epi16(target_key);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        int index = indices[i];
        __m512i vec = _mm512_loadu_si512((__m512i *)array[index]);
        __mmask32 mask = _mm512_cmpeq_epi16_mask(vec, target_vec);

        if (mask) {
            //printf("Match found in vector %d\n", index);
        }
    }
}

// 场景 2：逐键比较（所有键）
void key_to_key_test(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key, int indices[ARRAY_SIZE]) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int index = indices[i];
        for (int j = 0; j < VECTOR_SIZE; j++) {
            if (array[index][j] == target_key) {
                //printf("Match found in vector %d, key %d\n", index, j);
            }
        }
    }
}

// 场景 3：逐键比较（单键）
void single_key_test(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t target_key, int indices[ARRAY_SIZE]) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int index = indices[i];
        if (array[index][0] == target_key) {
            //printf("Match found in vector %d, first key\n", index);
        }
    }
}

// 测试运行时间
double measure_time(void (*test_func)(uint16_t array[ARRAY_SIZE][VECTOR_SIZE], uint16_t, int[]),
                    uint16_t array[ARRAY_SIZE][VECTOR_SIZE],
                    uint16_t target_key, int indices[ARRAY_SIZE],
                    const char* test_name) {
    double total_time = 0.0;

    for (int run = 0; run < NUM_RUNS; run++) {
        clock_t start_time = clock();
        test_func(array, target_key, indices);
        clock_t end_time = clock();

        double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        total_time += time_taken;

        printf("%s - Run %d: Time taken: %f seconds\n", test_name, run + 1, time_taken);
    }

    double average_time = total_time / NUM_RUNS;
    printf("%s: Average time: %f seconds over %d runs\n\n", test_name, average_time, NUM_RUNS);
    return average_time;
}

int main() {
    uint16_t array[ARRAY_SIZE][VECTOR_SIZE];
    int indices[ARRAY_SIZE];

    // 数据初始化
    populate_array(array);
    generate_random_indices(indices);

    uint16_t target_key = rand() % 65536;

    // 顺序访问测试
    printf("Sequential Access Tests:\n");
    measure_time(avx512_test, array, target_key, indices, "AVX-512 Test (Sequential)");
    measure_time(key_to_key_test, array, target_key, indices, "Key-to-Key Test (Sequential)");
    measure_time(single_key_test, array, target_key, indices, "Single Key Test (Sequential)");

    // 生成新的随机索引用于随机访问
    generate_random_indices(indices);

    // 随机访问测试
    printf("Random Access Tests:\n");
    measure_time(avx512_test, array, target_key, indices, "AVX-512 Test (Random)");
    measure_time(key_to_key_test, array, target_key, indices, "Key-to-Key Test (Random)");
    measure_time(single_key_test, array, target_key, indices, "Single Key Test (Random)");

    return 0;
}