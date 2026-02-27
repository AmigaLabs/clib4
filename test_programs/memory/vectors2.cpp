#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

static size_t MM = 9000000;

void add(const vector<double> &a, const vector<double> &b, vector<double> &c) {
    size_t n = a.size();

    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    vector<double> a(MM), b(MM), c(MM);

    for (size_t i = 0; i < MM; ++i) {
        a[i] = 1.0 / (double) (i + 1);
        b[i] = a[i];
    }

    clock_t start = clock();
    add(a, b, c);
    clock_t end = clock();
    double diffs = (end - start) / (double) CLOCKS_PER_SEC;

    cout << "time = " << diffs << endl;

    return 0;
}