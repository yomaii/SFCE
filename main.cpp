#include "famicom.h"
#include <iostream>

int main() {
    Famicom famicom;
    int code = famicom.init("");
    cout << code << endl;
    famicom.showInfo();
    return 0;
}