/*
 * Contains tests and demonstrates functionality
*/
#include "robinhood.h"
#include <random>
#include <vector>
#include <unordered_map>


int main() {
    int failures = 0;

    // Set up random number generator
    constexpr int N = 100000;
    rh_set<int> test;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    const auto seed = rd();
    std::mt19937 gen(seed); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distrib(1, N);
 

    // Containers to keep track of the values inserted
    std::unordered_map<int, int> count;
    std::vector<int> pushed;

    // Perform insertions
    for (int i = 0; i < 1000; ++i) {
        auto temp = distrib(gen);
        count[temp] += 1;
        pushed.push_back(temp);
        test.add(temp); // add element to set
    }

    // Test the 'contains' method
    for (auto x: pushed) {
        if (!test.contains(x)) {
            std::cout << "failed to find " << x << std::endl;
            ++failures;
        }
    }

    // Test the 'remove' method
    for (int i = 0; i < 100; i++) {

        const auto x = pushed[i];
        bool removed = test.pop(x);
        --count[x];
        if (!removed && count[x] != 0) {
            std::cout << "didn't find while removing: " << x << std::endl;
            ++failures;
        }

        // Check that contains returns false
        if (test.contains(x) ) {
            std::cout << "found element after removing: " << x << std::endl;
            ++failures;
        }
    }
    std::cout << "Seed: " << seed << std::endl;
    std::cout << "Failures: " << failures << std::endl;
}
