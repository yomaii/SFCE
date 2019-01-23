#pragma once

#include "rom.h"
#include "code.h"
#include <string>
using namespace std;

class Famicom
{
private:
    Rom rom;
    bool loaded = false;
public:
    int init(string);
    void showInfo();
};