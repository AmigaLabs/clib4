#include <set>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <functional>
#include <iostream>
#include <unordered_set>

// generate a random integer sequence of lenth N
void generateRandomSequence(const size_t N, std::vector<int> &sequence) {

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distr(1, 10000);
    auto draw = std::bind(distr, generator);

    sequence.resize(N);
    for (auto &elem: sequence)
        elem = draw();
}

// find the unique elements in 'sequence' via appending them to a new vector
// and checking before each append inf the element is already in th vector
// Expected complexity: N (loop) * N (find)
void uniqueFind(const std::vector<int> &sequence) {

    std::vector<int> uniques;
    uniques.reserve(sequence.size());
    for (auto elem: sequence) {
        if (std::find(uniques.begin(), uniques.end(), elem) == sequence.end()) {
            uniques.push_back(elem);
        }
    }
    std::sort(uniques.begin(), uniques.end());

}

// find the unique elements in 'sequence' via inserting them into a set and
// then copying the set into a vector
void uniqueSet(const std::vector<int> &sequence) {

    std::set<int> uniquesTmp;
    for (auto elem: sequence) {
        uniquesTmp.insert(elem);
    }
    std::vector<int> uniques(uniquesTmp.begin(), uniquesTmp.end());

}

// find the unique elements in 'sequence' via insering them into a unordered set
// and then copying them into a vector
void uniqueUnordered(const std::vector<int> &sequence) {

    std::unordered_set<int> uniquesTmp;
    for (auto elem: sequence) {
        uniquesTmp.insert(elem);
    }
    std::vector<int> uniques(uniquesTmp.begin(), uniquesTmp.end());
    std::sort(uniques.begin(), uniques.end());

}

// find the unique elements in 'sequence' via std::unique
void unique(const std::vector<int> &sequence) {

    std::vector<int> uniques(sequence);
    std::sort(uniques.begin(), uniques.end());
    auto uniqueIt = std::unique(uniques.begin(), uniques.end());
    uniques.erase(uniqueIt, uniques.end());

}

// compare the different methods by determining them min time of 'n' runs for a random sequence
// of length 'N'
int main() {

    int n = 10;
    int N = 10000;

    std::vector <size_t> timesFind;
    std::vector <size_t> timesSet;
    std::vector <size_t> timesUnordered;
    std::vector <size_t> times;

    typedef std::chrono::microseconds TimeType;

    for (size_t i = 0; i < n; ++i) {
        std::vector<int> sequence;
        generateRandomSequence(N, sequence);

        auto startFind = std::chrono::system_clock::now();
        uniqueFind(sequence);
        auto stopFind = std::chrono::system_clock::now();
        auto elapsedFind = std::chrono::duration_cast<TimeType>(stopFind - startFind);
        timesFind.push_back(
                elapsedFind.count()
        );

        auto startSet = std::chrono::system_clock::now();
        uniqueSet(sequence);
        auto stopSet = std::chrono::system_clock::now();
        auto elapsedSet = std::chrono::duration_cast<TimeType>(stopSet - startSet);
        timesSet.push_back(
                elapsedSet.count()
        );

        auto startUnordered = std::chrono::system_clock::now();
        uniqueUnordered(sequence);
        auto stopUnordered = std::chrono::system_clock::now();
        auto elapsedUnordered = std::chrono::duration_cast<TimeType>(stopUnordered - startUnordered);
        timesUnordered.push_back(
                elapsedUnordered.count()
        );

        auto start = std::chrono::system_clock::now();
        unique(sequence);
        auto stop = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<TimeType>(stop - start);
        times.push_back(
                //(elapsed.count())
                elapsed.count()
        );
    }

    std::cout << "Min-time find:" << std::endl;
    std::cout << *std::min_element(timesFind.begin(), timesFind.end()) << std::endl;
    std::cout << "Min-time set:" << std::endl;
    std::cout << *std::min_element(timesSet.begin(), timesSet.end()) << std::endl;
    std::cout << "Min-time unordered:" << std::endl;
    std::cout << *std::min_element(timesUnordered.begin(), timesUnordered.end()) << std::endl;
    std::cout << "Min-time unique:" << std::endl;
    std::cout << *std::min_element(times.begin(), times.end()) << std::endl;
}