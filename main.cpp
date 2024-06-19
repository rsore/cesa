#include <iostream>
#include <cesa/vector.hpp>

int main() {
    cesa::vector<int, 10> vec;

    vec.push_back(5);
    vec.insert(vec.begin() + 1, {2, 3, 5, 4, 4, 2, 5});

    for (const auto &i : vec)
    {
        std::cout << i << std::endl;
    }

    return 0;
}
