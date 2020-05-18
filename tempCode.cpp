#include "tempCode.h"

vector<quaternion> quaternions;
static int nameNo = 0;
static int labelNo = 0;

void pushQuaternion(Operation op, string re, string a, string b) 
{
	quaternions.push_back({ op, re, a, b });
}

string getNewName()
{
	return "#t" + to_string(nameNo++);
}

string getNewLabel()
{
	return "labelgasc2389v" + to_string(labelNo++);
}

string op2string(Operation op) 
{
	switch (op)
	{
	case CONST2TABLE:
		return "CONST2TABLE";
		break;
	case VAR2TABLE:
		return "VAR2TABLE";
		break;
	case ARR2TABLE:
		return "ARR2TABLE";
		break;
	case FUNC2TABLE:
		return "FUNC2TABLE";
		break;
	case PUSH:
		return "PUSH";
		break;
	case CALL:
		return "CALL";
		break;
	case RET:
		return "RET";
		break;
	case GETRET:
		return "GETRET";
		break;
	case PRINT:
		return "PRINT";
		break;
	case EXIT:
		return "EXIT";
		break;
	case SCANF:
		return "SCANF";
		break;
	case LABEL:
		return "LABEL";
		break;
	case GOTO:
		return "GOTO";
		break;
	case BEQ:
		return "BEQ";
		break;
	case BGEZ:
		return "BGEZ";
		break;
	case BGTZ:
		return "BGTZ";
		break;
	case BNE:
		return "BNE";
		break;
	case BLTZ:
		return "BLTZ";
		break;
	case BLEZ:
		return "BLEZ";
		break;
	case SLT:
		return "SLT";
		break;
	case ASSIGNOP:
		return "ASSIGNOP";
		break;
	case ARRASSIGN:
		return "ARRASSIGN";
		break;
	case ADD:
		return "ADD";
		break;
	case SUB:
		return "SUB";
		break;
	case TIMES:
		return "TIMES";
		break;
	case DIVIDE:
		return "DIVIDE";
		break;
	case BRACK:
		return "BRACK";
		break;
	case STORE:
		return "STORE";
	case RECOVER:
		return "RECOVER";
	case FUNCEND:
		return "FUNCEND";
	case NOP:
		return "NOP";
	default:
		break;
	}
}

string quaternion2string(quaternion q) {
	return op2string(q.op) + " " + q.re + " "+ q.a + " " + q.b + "\n";
}

bool isdigitchar(string s) {
	if (isdigit(s[0]))
		return true;
	if (s[0] == '+' || s[0] == '-')
		return true;
	if (s[0] == '\'')
		return true;
	return false;
}

int str2digitchar(string s) {
	if (s[0] != '\'') {
		return stoi(s);
	}
	return s[1];
}