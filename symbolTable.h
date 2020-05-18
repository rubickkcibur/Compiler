#pragma once
#include "heads.h"

enum identifierType { CONST = 1, VAR = 2, ARR = 3, FUNC = 4 };
enum dataType {VOID = 0,INT = 1, CHAR = 2};

struct tableEntry {
	identifierType t;
	dataType d;
	string constValue;
	int arrlength;
	vector<dataType> v;
	int paraNo = 0;
	vector<string> paraNames;
	tableEntry() {

	}
	tableEntry(identifierType t, dataType d) :t(t), d(d) {}
	tableEntry(identifierType t, dataType d, string c) :t(t), d(d), constValue(c) {}
	tableEntry(identifierType t, dataType d, string c, int a) 
		:t(t), d(d), constValue(c), arrlength(a) {}
	tableEntry(identifierType t, dataType d,string c,int a,vector<dataType> v,int p) 
		:t(t), d(d),constValue(c),arrlength(a),v(v),paraNo(p) {}

};

extern map<string, tableEntry> * mapPointer;

extern map<string, tableEntry> globalMap;

int createMap(string name);

int movePointer(string name);

void resetPointer();

string dataType2string(dataType d);
