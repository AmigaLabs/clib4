#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>

void test_vector_growth_alignment() {
    std::cout << "=== std::vector growth alignment test ===\n";

    std::vector<std::vector<int>> container;

    for (int i = 0; i < 100; ++i) {
        std::vector<int> v;
        for (int j = 0; j < 1000; ++j) {
            v.push_back(j);
        }
        container.push_back(std::move(v));
    }

    // Verify contents and structure
    for (int i = 0; i < 100; ++i) {
        assert(container[i].size() == 1000);
        assert(container[i][500] == 500);
    }

    std::cout << "Vector growth and nested containers test passed.\n\n";
}
