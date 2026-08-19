#pragma once

#include <vector>
#include <cmath>

#include "logger.hpp"

constexpr int SIZE = 32;
constexpr int CHUNK_HEIGHT = 4;
constexpr int WORLD_HEIGHT = SIZE * CHUNK_HEIGHT;

constexpr float ISO = 0.f;

constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

inline int multiplication_of_prime_factors(int a) {
    if (a <= 0) return a;

    int n = a;

    // Get prime factors
    std::vector<int> factors;

    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    for (int i = 3; i*i < n; ++i) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
    }

    if (n > 2) {
        factors.push_back(n);
    }

    // Multiply out prime factors and return
    int b = 1;
    for (int i : factors) {
        b *= i;
    }

    return b;
}
