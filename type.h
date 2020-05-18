#pragma once
#include <string>
#include <vector>

using namespace std;

enum typeCode {
	IDENFR = 1, INTCON, CHARCON, STRCON, CONSTTK, INTTK,
	CHARTK, VOIDTK, MAINTK, IFTK, ELSETK, DOTK, WHILETK,
	FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS, MINU, MULT,
	DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN,
	COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
};

struct type {
	typeCode code;
	string value;
	int row;
};

extern vector<type> types;

string typeCode2String(typeCode t);
