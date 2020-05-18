#pragma once
#include <iostream>
#include "symbolTable.h"

using namespace std;

extern int row;
extern ofstream ferr;
extern vector<pair<dataType,int>> returnTypeCheck;
void errorInit();
void errorPut(int n,char t);

