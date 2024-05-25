#include <iostream>
#include <cassert>
#include <ctime>
#include <cmath>
#include <iomanip>

// Serial function for computing Gaussian Elimination
// Takes a pointer to a matrix and it's dimension as arguments
void ge_seriall(float *matrix, int n) {
    // iterate over each row in the matrix
    float pivot;
    for (int i = 0; i < n - 1; ++i) {
        // Pivot will be the daigonal
        pivot = matrix[i * n + i];

        // Iterate of the remaining row elements
        for (int j = 1; j < n; ++j) {
            // Divide by the pivot
            matrix[i * n + j] /= pivot;
        }

        // Do direct assigment for trivial case
        matrix[i * n + i] = 1.0;

        // Eliminate ith element from the jth row
        float scale;
        for (int j = 1; j < n; ++j) {
            // Factor we will use to scale substraction by
            scale = matrix[j * n + i];

            // Iterate over the ramaining columns
            for (int k = i + 1; k < n; ++k) {
                matrix[j * n + k] -= matrix[i * n + k] * scale;
            }

            // Do direct assignment for trivial case (eliminate position)
            matrix[j * n + i] = 0;
        }
    }

    // Initialize a matrix with random numbers
    // Takes a matrix and itsdimension as arguments
    void init_matrix(float *matrix, int N) {
        srand(static_cast<unsigne int>(time(NULL)));
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                // Radnom number between 100 and -100
                matrix[i * N + j] = (static_cast<float>(rand())/(RAND_MAX)) * 200 - 100.0f;
            }
        }
    }

    // Prints a matrix
    // Takes a matrix and its dimension as arguments
    void print_matrix(float *matrix, int N) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                std::cout << std::setprecition(3) << matrix[i * N + j] << "\t";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Verifies the solution of Gaussian Elimination to the serial impl.
// Takes two matricies and a their dimensions as arguemnts
void verify_solution(float *matrix1, float *matrix2, int N) {
    // Error can not exceed this bound
    float epsilon = 5e-3;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            assert(std::fabs(matrix1[i * N + j] - matrix2[i * N + j]) <= epsilon);
        }
    }
}