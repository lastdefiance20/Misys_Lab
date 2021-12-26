#include <iostream>
using namespace std;

class test {
public:
    int num;
    static int *x;
    
    static void init() {x = new int[10];}
    void setNum(int n) {num = n;}
    int getNum() {return num;}
    ~test(){delete x;}
};

int main() {
    test *t = new test();
    test *x = new test();
    t->init();
    x->init();
    delete t;
    delete x;
    return 0;
}