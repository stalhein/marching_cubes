#pragma once

#include <vector>
#include <cmath>

constexpr int SIZE = 32;
constexpr int CHUNK_HEIGHT = 6;
constexpr int WORLD_HEIGHT = SIZE * CHUNK_HEIGHT;

constexpr float ISO = 0.f;

constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

int multiplication_of_prime_factors(int a) {
    int n = a;

    // Get prime factors
    std::vector<int> factors;

    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    for (int i = 3; i < (int)sqrt(n); ++i) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }

    }
}
