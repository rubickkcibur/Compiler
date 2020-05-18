#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctype.h>
#include <iterator>

using namespace std;

void wordAnalyze(ifstream& fin);
void grammarAnalyze();
extern ofstream fdebug;
extern ofstream ferr;