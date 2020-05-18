#include "symbolTable.h"

map<string, tableEntry> globalMap;

map<string, map<string, tableEntry>> nameMap;

map<string, tableEntry> * mapPointer;

int createMap(string name)
{
	map<string, map<string, tableEntry>>::iterator ite;
	ite = nameMap.find(name);
	if (ite != nameMap.end())
		return -1;
	map<string, tableEntry> m;
	nameMap[name] = m;
	return 0;
}

int movePointer(string name)
{
	map<string, map<string, tableEntry>>::iterator ite;
	ite = nameMap.find(name);
	if (ite == nameMap.end())
		return -1;
	mapPointer = &(nameMap[name]);
	return 0;
}

void resetPointer()
{
	mapPointer = &globalMap;
}

string dataType2string(dataType d)
{
	switch (d)
	{
	case INT:
		return "int";
		break;
	case CHAR:
		return "char";
		break;
	case VOID:
		return "void";
		break;
	default:
		break;
	}
}