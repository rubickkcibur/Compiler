#pragma once
#include <string>
#include <vector>
using namespace std;

enum Operation {CONST2TABLE = 1,VAR2TABLE, ARR2TABLE, FUNC2TABLE, PUSH, CALL, RET, 
				GETRET, PRINT, EXIT,SCANF, LABEL, GOTO, BEQ,BGEZ,BGTZ,BNE,BLTZ,BLEZ, 
				SLT,ASSIGNOP, ARRASSIGN,ADD, SUB, TIMES, DIVIDE, BRACK,STORE,RECOVER,
				FUNCEND,NOP};

struct quaternion {
	Operation op;
	string re;
	string a;
	string b;
};

extern vector<quaternion> quaternions;
void pushQuaternion(Operation op, string re, string a, string b);
string getNewName();
string getNewLabel();
string op2string(Operation op);
string quaternion2string(quaternion q);
bool isdigitchar(string s);
int str2digitchar(string s);
string getNewName();