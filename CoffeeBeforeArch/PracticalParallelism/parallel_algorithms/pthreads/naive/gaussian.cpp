#include "utils.h"
#include <stdlib.h>
#include <cstring>

int main() {
    int num_threads = 8;
    int N = 2048;
    float *matrix;
    float *matrix_pthread;
    size_t bytes = N * N * sizeof(float);
    matrix = new float[N * N];
    matrix_pthread = new float[N * N];

    init_matrix(matrix, N);
    std::memcpy(matrix_pthread, matrix, bytes);

    launch_threads(num_threads, matrix_pthread, N);

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    start = std::chrono::high_resolution_clock::now();
    ge_serial(matrix, N);
    end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::cout << "Elapsed time serial = " << elapsed.count() << " seconds" << std::endl;

    verify_solution(matrix, matrix_pthread, N);

    delete[] matrix;
    delete[] matrix_pthread;
}