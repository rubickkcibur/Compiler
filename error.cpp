#include "heads.h"
#include "error.h"
#include "symbolTable.h"

int row;
vector<pair<dataType, int>> returnTypeCheck;

void errorInit()
{
	row = 1;
}

void errorPut(int n, char t)
{
	ferr << n << " " << t << "\n";
}