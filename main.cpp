//
// Created by Alexandra Ivanova on 21/06/2019.
//

#include <iostream>
#include <vector.h>

static int cnt = 0;

struct my_class {
    int a;
    my_class(int a) : a(a) {
        cnt++;
    }
    my_class() = delete;
    my_class(my_class &other) : a(other.a) {
        cnt++;
    }
    ~my_class() {
        cnt--;
    }
};

int main() {
    vector<int> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    vector<int> b;
    b.push_back(4);
    b.push_back(5);
    b = a;
    b[1] = 0;
    b.push_back(6);
    for (auto i : a) {
        std:: cout << i << " ";
    }
    std::cout << '\n';
    for (auto i : b) {
        std:: cout << i << " ";
    }
    std::cout << '\n' << cnt << '\n';
    return 0;
}