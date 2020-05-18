#include "heads.h"
#include "type.h"
#include "gtype.h"
#include "symbolTable.h"
#include "error.h"
#include "tempCode.h"
#define CODE types[analyzePointer].code
#define ADDGTYPE(s) {gtype g = {analyzePointer,s};gtypes.push_back(g);}

int analyzePointer;
pair<vector<dataType>, vector<string>> parameters();
int unsignedInteger();
int integer();
char character();
void constantDefinition();
void constantDeclaration();
void variationDefinition();
void variationDeclaration();
pair<string,dataType> factor();
pair<string,dataType> term();
pair<string,dataType> expression();
vector<dataType> valueParameters();
void returnValueFunctionCall();
void nonReturnValueFunctionCall();
void relationalOperator();
void addOperator();
void mutiplyOperator();
void condition(string label,bool flag);
void conditionStatement();
string step();
void loopStatement();
void assignStatement(identifierType t);
void readStatement();
string sstring();
void writeStatement();
pair<dataType,int> returnStatement();
void statementList();
void statement();
void complexStatement();
void returnValueFunctionDefinition();
void nonReturnValueFunctionDefinition();
void mainFunction();
void program();

void grammarAnalyze() {
	resetPointer();
	analyzePointer = 0;
	program();
}

string identifier() 
{
	if (CODE == IDENFR) {
		analyzePointer++;
		//ADDGTYPE("<标识符>");
		return types[analyzePointer - 1].value;
	}
	else {
		throw "identifier wrong, pointer:" + analyzePointer; 
	}
		
}

pair<dataType,string> declarationHead() 
{
	string s;
	if (CODE == INTTK) {
		analyzePointer++;
		s = identifier();
		ADDGTYPE("<声明头部>");
		return pair<dataType, string>(INT, s);
	}
	else if (CODE == CHARTK) {
		analyzePointer++;
		s = identifier();
		ADDGTYPE("<声明头部>");
		return pair<dataType, string>(CHAR, s);
	}
	else
		throw "declarationHead wrong, pointer:" + analyzePointer;
}

dataType typeIdentifier() 
{
	if (CODE == INTTK) {
		analyzePointer++;
		//ADDGTYPE("<类型标识符>");
		return INT;
	}
	else if (CODE == CHARTK) {
		analyzePointer++;
		//ADDGTYPE("<类型标识符>");
		return CHAR;
	}
	else
		throw "typeIdentifier wrong, pointer: " + analyzePointer;
}

pair<vector<dataType>,vector<string>> parameters()
{
	vector<dataType> re;
	vector<string> res;
	int paracnt = 1;
	if (CODE != INTTK
		&& CODE != CHARTK) {
		ADDGTYPE("<参数表>");
		return {re,res};
	}
	dataType d = typeIdentifier();
	string s = identifier();
	if ((*mapPointer).find(s) != (*mapPointer).end()) {
		errorPut(types[analyzePointer-1].row,'b');
	}
	else {
		(*mapPointer)[s] = { VAR,d,"",0,{},paracnt++ };
		re.push_back(d);
		res.push_back(s);
	}
	while (CODE == COMMA) {
		analyzePointer++;
		dataType d = typeIdentifier();
		string s = identifier();
		if ((*mapPointer).find(s) != (*mapPointer).end()) {
			errorPut(types[analyzePointer - 1].row, 'b');
		}
		else {
			(*mapPointer)[s] = tableEntry{ VAR,d,"",0,{},paracnt++ };
			re.push_back(d);
			res.push_back(s);
		}
	}
	ADDGTYPE("<参数表>");
	return {re,res};
}

int unsignedInteger()
{
	if (CODE == INTCON) {
		analyzePointer++;
		ADDGTYPE("<无符号整数>")
		return stoi(types[analyzePointer-1].value);
	}
	else {
		throw "unsignedInteger wrong, pointer:" + analyzePointer;
	}
}

int integer()
{
	bool flag = true;
	if (CODE == PLUS)
		analyzePointer++;
	else if (CODE == MINU) {
		analyzePointer++;
		flag = false;
	}
	int u = unsignedInteger();
	ADDGTYPE("<整数>")
	return flag ? u : -u;
}

char character()
{
	if (CODE == CHARCON) {
		char c = types[analyzePointer].value[0];
		if (c == '+' || c == '-')
			analyzePointer++;
		else if (c == '*' || c == '/')
			analyzePointer++;
		else if (isalnum(c) || c == '_') {
			analyzePointer++;
		}
		else
			analyzePointer++;
		//ADDGTYPE("<字符>" )
		return c;
	}
	throw "character wrong, pointer: " + analyzePointer;
}

void constantDefinition()
{
	if (CODE == INTTK) {
		string s;
		int inte;
		analyzePointer++;
		s = identifier();
		if (CODE == ASSIGN) {
			analyzePointer++;
			if (CODE == INTCON || CODE == MINU || CODE == PLUS)
				inte = integer();
			else
				errorPut(types[analyzePointer].row, 'o');
			if ((*mapPointer).find(s) != (*mapPointer).end())
				errorPut(types[analyzePointer-3].row,'b');
			else {
				(*mapPointer)[s] = { CONST,INT,to_string(inte) };
				pushQuaternion(CONST2TABLE, "int", s, to_string(inte));
			}
			while (CODE == COMMA) {
				analyzePointer++;
				s = identifier();
				if (CODE == ASSIGN) {
					analyzePointer++;
					inte = integer();
					if ((*mapPointer).find(s) != (*mapPointer).end())
						errorPut(types[analyzePointer - 3].row, 'b');
					else {
						(*mapPointer)[s] = { CONST,INT,to_string(inte) };
						pushQuaternion(CONST2TABLE, "int", s, to_string(inte));
					}
				}
				else
					throw "constantDefinition wrong, pointer: " + analyzePointer;
			}
			ADDGTYPE("<常量定义>" )
			return;
		}
		throw "constantDefinition wrong, pointer: " + analyzePointer;
	}
	if (CODE == CHARTK) {
		string s;
		char c;
		analyzePointer++;
		s = identifier();
		if (CODE == ASSIGN) {
			analyzePointer++;
			if (types[analyzePointer].code == CHARCON)
				c = character();
			else {
				errorPut(types[analyzePointer].row, 'o');
				analyzePointer++;
			}
			if ((*mapPointer).find(s) != (*mapPointer).end())
				errorPut(types[analyzePointer - 3].row, 'b');
			else {
				(*mapPointer)[s] = { CONST,CHAR,string(1,c) };
				pushQuaternion(CONST2TABLE, "char", s, "\'"+string(1, c)+"\'");
			}

			while (CODE == COMMA) {
				analyzePointer++;
				s = identifier();
				if (CODE == ASSIGN) {
					analyzePointer++;
					c = character();
					if ((*mapPointer).find(s) != (*mapPointer).end())
						errorPut(types[analyzePointer - 3].row, 'b');
					else {
						(*mapPointer)[s] = { CONST,CHAR,string(1,c) };
						pushQuaternion(CONST2TABLE, "char", s, "\'"+string(1, c)+"\'");
					}
				}
				else
					throw "constantDefinition wrong, pointer: " + analyzePointer;
			}
			ADDGTYPE("<常量定义>" )
			return;
		}
		throw "constantDefinition wrong, pointer: " + analyzePointer;
	}
	throw "constantDefinition wrong, pointer: " + analyzePointer;
}

void constantDeclaration()
{
	if (CODE == CONSTTK) {
		analyzePointer++;
		constantDefinition();
		if (CODE == SEMICN)
			analyzePointer++;
		else
			errorPut(types[analyzePointer-1].row, 'k');
	}
	else
		throw "constantDeclaration wrong, pointer: " + analyzePointer;
	while (CODE == CONSTTK) {
		analyzePointer++;
		constantDefinition();
		if (CODE == SEMICN)
			analyzePointer++;
		else
			errorPut(types[analyzePointer-1].row, 'k');
	}
	ADDGTYPE("<常量说明>" )
}

void variationDefinition()
{
	string s;
	dataType d = typeIdentifier();
	s = identifier();
	if (CODE == LBRACK) {
		analyzePointer++;
		int u = unsignedInteger();
		if (CODE == RBRACK) {
			analyzePointer++;
			if ((*mapPointer).find(s) != (*mapPointer).end())
				errorPut(types[analyzePointer - 4].row, 'b');
			else {
				(*mapPointer)[s] = { ARR,d,"",u };
				pushQuaternion(ARR2TABLE, dataType2string(d), s, to_string(u));
			}
		}
		else {
			errorPut(types[analyzePointer - 1].row, 'm');
			if ((*mapPointer).find(s) != (*mapPointer).end())
				errorPut(types[analyzePointer - 4].row, 'b');
			else
				(*mapPointer)[s] = { ARR,d,"",u };
		}
	}
	else {
		if ((*mapPointer).find(s) != (*mapPointer).end())
			errorPut(types[analyzePointer - 1].row, 'b');
		else {
			(*mapPointer)[s] = { VAR,d };
			pushQuaternion(VAR2TABLE, dataType2string(d), s, "");
		}
	}
	while (CODE == COMMA) {
		analyzePointer++;
		s = identifier();
		if (CODE == LBRACK) {
			analyzePointer++;
			int u = unsignedInteger();
			if (CODE == RBRACK) {
				analyzePointer++;
				if ((*mapPointer).find(s) != (*mapPointer).end())
					errorPut(types[analyzePointer - 4].row, 'b');
				else {
					(*mapPointer)[s] = { ARR,d,"",u };
					pushQuaternion(ARR2TABLE, dataType2string(d), s, to_string(u));
				}
			}
			else {
				errorPut(types[analyzePointer - 1].row, 'm');
				if ((*mapPointer).find(s) != (*mapPointer).end())
					errorPut(types[analyzePointer - 4].row, 'b');
				else
					(*mapPointer)[s] = { ARR,d,"",u };
			}
		}
		else {
			if ((*mapPointer).find(s) != (*mapPointer).end())
				errorPut(types[analyzePointer - 1].row, 'b');
			else {
				(*mapPointer)[s] = { VAR,d };
				pushQuaternion(VAR2TABLE, dataType2string(d), s, "");
			}
		}
	}
	ADDGTYPE("<变量定义>" )
}

void variationDeclaration()
{
	variationDefinition();
	if (CODE != SEMICN)
		errorPut(types[analyzePointer - 1].row, 'k');
	analyzePointer++;
	while ((CODE == INTTK
		|| CODE == CHARTK) && types[analyzePointer+2].code != LPARENT) {
		variationDefinition();
		if (CODE != SEMICN)
			errorPut(types[analyzePointer - 1].row, 'k');
		analyzePointer++;
	}
	ADDGTYPE("<变量说明>" )
}

pair<string,dataType> factor()
{
	pair<string, dataType> p;
	p.second = VOID;
	if (CODE == LPARENT) {
		analyzePointer++;
		p = expression();
		p.second = INT;
		if (CODE != RPARENT)
			errorPut(types[analyzePointer - 1].row, 'l');
		else
			analyzePointer++;
	}
	else if (CODE == INTCON || CODE == PLUS || CODE == MINU) {
		int inte = integer();
		p.second = INT;
		p.first = to_string(inte);
	}
	else if (CODE == CHARCON) {
		char c = character();
		p.second = CHAR;
		p.first = "\'" + string(1,c) + "\'";
	}
	else if (CODE == IDENFR) {
		string s = identifier();
		map<string, tableEntry>* temp = mapPointer;
		tableEntry t;
		bool flag = true;
		if ((*mapPointer).find(s) == (*mapPointer).end()) {
			resetPointer();
			if ((*mapPointer).find(s) == (*mapPointer).end()) {
				errorPut(types[analyzePointer - 1].row, 'c');
				flag = false;
			}
			else
				t = (*mapPointer)[s];
		}
		else
			t = (*mapPointer)[s];
		mapPointer = temp;
		if (flag) {
			p.second = t.d;
			if (t.t == FUNC && t.d != VOID) {
				analyzePointer--;
				returnValueFunctionCall();
				p.first = getNewName();
				pushQuaternion(GETRET, p.first, dataType2string(t.d), "");
			}
			else if(t.t == FUNC && t.d == VOID){
				analyzePointer--;
				nonReturnValueFunctionCall();
			}
			else {
				if (CODE == LBRACK) {
					analyzePointer++;
					pair<string, dataType> pe;
					if ((pe = expression()).second != INT)
						errorPut(types[analyzePointer].row, 'i');
					p.first = getNewName();
					pushQuaternion(BRACK, p.first, s, pe.first);
					if (CODE == RBRACK)
						analyzePointer++;
					else
						errorPut(types[analyzePointer - 1].row, 'm');
				}
				else {
					p.first = s;
				}
			}
		}
		else {
			p.second = INT;
			if (CODE == LPARENT) {
				analyzePointer--;
				returnValueFunctionCall();
			}
			else {
				if (CODE == LBRACK) {
					analyzePointer++;
					if (expression().second != INT)
						errorPut(types[analyzePointer].row, 'i');
					if (CODE == RBRACK)
						analyzePointer++;
					else
						errorPut(types[analyzePointer - 1].row, 'm');
				}
			}
		}
	}
	else 
		throw "factor wrong, pointer: " + analyzePointer;
	ADDGTYPE("<因子>")
	return p;
}

pair<string,dataType> term()
{
	pair<string,dataType> p = factor();
	while (CODE == MULT || CODE == DIV) {
		Operation op = CODE == MULT ? TIMES : DIVIDE;
		mutiplyOperator();
		pair<string,dataType> pe = factor();
		string t = getNewName();
		pushQuaternion(op, t, p.first, pe.first);
		p.first = t;
		p.second = INT;
	}
	ADDGTYPE("<项>")
	return p;
}

pair<string,dataType> expression()
{
	Operation op;
	bool flag = false;
	if (CODE == PLUS || CODE == MINU) {
		op = CODE == PLUS ? ADD : SUB;
		analyzePointer++;
		flag = true;
	}
	pair<string,dataType> p = term();
	if (flag) {
		string t = getNewName();
		pushQuaternion(op, t, "0", p.first);
		p.first = t;
		p.second = INT;
	}
	while (CODE == PLUS || CODE == MINU) {
		flag = true;
		op = CODE == PLUS ? ADD : SUB;
		addOperator();
		pair<string,dataType> pe = term();
		string t = getNewName();
		pushQuaternion(op, t, p.first, pe.first);
		p.first = t;
		p.second = INT;
	}
	ADDGTYPE("<表达式>")
	return p;
}

vector<dataType> valueParameters()
{
	vector<dataType> re;
	int cnt = 1;
	if (CODE != RPARENT) {
		pair<string, dataType> pe = expression();
		pushQuaternion(PUSH, dataType2string(pe.second), pe.first, to_string(cnt++));
		re.push_back(pe.second);
		while (CODE == COMMA) {
			analyzePointer++;
			pe = expression();
			pushQuaternion(PUSH, dataType2string(pe.second), pe.first, to_string(cnt++));
			re.push_back(pe.second);
		}
	}
	ADDGTYPE("<值参数表>")
	return re;
}

void returnValueFunctionCall()
{
	string s = identifier();
	vector<dataType> stdmode;
	vector<dataType> remode;
	map<string, tableEntry>* temp = mapPointer;
	tableEntry entry;
	if ((*mapPointer).find(s) != (*mapPointer).end()) {
		entry = (*mapPointer)[s];
	}
	else {
		resetPointer();
		if ((*mapPointer).find(s) != (*mapPointer).end()) {
			entry = (*mapPointer)[s];
			stdmode = entry.v;
		}
		else {
			;
		}
	}
	mapPointer = temp;
	if (CODE == LPARENT) {
		analyzePointer++;
		pushQuaternion(STORE, "", "", "");
		remode = valueParameters();
		if (stdmode.size() != remode.size())
			errorPut(types[analyzePointer - 1].row, 'd');
		else {
			for (int i = 0;i < stdmode.size();i++)
				if (stdmode[i] != remode[i]) {
					errorPut(types[analyzePointer - 1].row, 'e');
					break;
				}
		}
		if (CODE == RPARENT) {
			analyzePointer++;
			pushQuaternion(CALL, dataType2string(entry.d), s, "");
			pushQuaternion(RECOVER, "", "", "");
			ADDGTYPE("<有返回值函数调用语句>" )
			return;
		}
		else {
			errorPut(types[analyzePointer - 1].row, 'l');
			return;
		}
	}
	throw "returnValueFunctionCall wrong, pointer: " + analyzePointer;
}

void nonReturnValueFunctionCall()
{
	string s = identifier();
	vector<dataType> stdmode;
	vector<dataType> remode;
	map<string, tableEntry>* temp = mapPointer;
	tableEntry entry;
	if ((*mapPointer).find(s) != (*mapPointer).end()) {
		entry = (*mapPointer)[s];
	}
	else {
		resetPointer();
		if ((*mapPointer).find(s) != (*mapPointer).end()) {
			entry = (*mapPointer)[s];
			stdmode = entry.v;
		}
		else {
			;
		}
	}
	mapPointer = temp;
	if (CODE == LPARENT) {
		analyzePointer++;
		pushQuaternion(STORE, "", "", "");
		remode = valueParameters();
		if (stdmode.size() != remode.size())
			errorPut(types[analyzePointer - 1].row, 'd');
		else {
			for (int i = 0; i < stdmode.size(); i++)
				if (stdmode[i] != remode[i]) {
					errorPut(types[analyzePointer - 1].row, 'e');
					break;
				}
		}
		if (CODE == RPARENT) {
			analyzePointer++;
			pushQuaternion(CALL, "void", s, "");
			pushQuaternion(RECOVER, "", "", "");
			ADDGTYPE("<无返回值函数调用语句>" )
			return;
		}
		else {
			errorPut(types[analyzePointer - 1].row, 'l');
			return;
		}
	}
	throw "nonReturnValueFunctionCall wrong, pointer: " + analyzePointer;
}

void relationalOperator()
{
	typeCode code = CODE;
	if (code == LSS || code == LEQ || code == GRE ||
		code == GEQ || code == NEQ || code == EQL) {
		analyzePointer++;
		//ADDGTYPE("<关系运算符>" )
	}
	else
		throw "relationalOperator wrong, pointer: " + analyzePointer;	
}

void addOperator()
{
	if (CODE == PLUS || CODE == MINU) {
		analyzePointer++;
		//ADDGTYPE("<加法运算符>" )
	}
	else
		throw "addOperator wrong, pointer: " + analyzePointer;
}

void mutiplyOperator()
{
	if (CODE == MULT || CODE == DIV) {
		analyzePointer++;
		//ADDGTYPE("<乘法运算符>" )
	}
	else
		throw "multiplyOperator wrong, pointer: " + analyzePointer;
}

void condition(string label,bool flag)
{
	pair<string, dataType> p1;
	pair<string, dataType> p2;
	p2.second = VOID;
	p1 = expression();
	typeCode code = CODE;
	if (code == LSS || code == LEQ || code == GRE ||
		code == GEQ || code == NEQ || code == EQL) {
		relationalOperator();
		p2 = expression();
		string t;
		switch (code) {
		case LSS:
			t = getNewName();
			pushQuaternion(SLT, t, p1.first, p2.first);
			if (flag)
				pushQuaternion(BNE, label, t, "0");
			else
				pushQuaternion(BEQ, label, t, "0");
			break;
		case LEQ:
			t = getNewName();
			pushQuaternion(SLT, t, p2.first, p1.first);
			if (flag)
				pushQuaternion(BEQ, label, t, "0");
			else
				pushQuaternion(BNE, label, t, "0");
			break;
		case GRE:
			t = getNewName();
			pushQuaternion(SLT, t, p2.first, p1.first);
			if (flag)
				pushQuaternion(BNE, label, t, "0");
			else
				pushQuaternion(BEQ, label, t, "0");
			break;
		case GEQ:
			t = getNewName();
			pushQuaternion(SLT, t, p1.first, p2.first);
			if (flag)
				pushQuaternion(BEQ, label, t, "0");
			else
				pushQuaternion(BNE, label, t, "0");
			break;
		case NEQ:
			if (flag)
				pushQuaternion(BNE, label, p1.first, p2.first);
			else
				pushQuaternion(BEQ, label, p1.first, p2.first);
			break;
		case EQL:
			if (flag)
				pushQuaternion(BEQ, label, p1.first, p2.first);
			else
				pushQuaternion(BNE, label, p1.first, p2.first);
			break;
		}
	}
	else {
		if (flag)
			pushQuaternion(BNE, label, p1.first, "0");
		else
			pushQuaternion(BEQ, label, p1.first, "0");
	}
	if ((p2.second == VOID && p1.second != INT) || (p2.second != VOID && p1.second != p2.second))
		errorPut(types[analyzePointer - 1].row, 'f');
	ADDGTYPE("<条件>" )
}

void conditionStatement()
{
	string label1 = getNewLabel();
	string label2 = getNewLabel();
	if (CODE == IFTK)
		analyzePointer++;
	else
		throw "conditionStatement wrong, pointer: " + analyzePointer;
	if (CODE == LPARENT) {
		analyzePointer++;
		condition(label1,false);
	}
	else
		throw "conditionStatement wrong, pointer: " + analyzePointer;
	if (CODE == RPARENT) {
		analyzePointer++;
		statement();
	}
	else {
		errorPut(types[analyzePointer - 1].row, 'l');
		statement();
	}
	if (CODE == ELSETK) {
		pushQuaternion(GOTO, label2, "", "");
		pushQuaternion(LABEL, label1, "", "");
		analyzePointer++;
		statement();
		pushQuaternion(LABEL, label2, "", "");
	}
	else {
		pushQuaternion(LABEL, label1, "", "");
	}
	ADDGTYPE("<条件语句>" )
}

string step()
{
	int u = unsignedInteger();
	ADDGTYPE("<步长>")
	return to_string(u);
}

void loopStatement()
{
	string label1 = getNewLabel();
	string label2 = getNewLabel();
	if (CODE == WHILETK) {
		analyzePointer++;
		pushQuaternion(LABEL, label1, "", "");
		if (CODE == LPARENT) {
			analyzePointer++;
			condition(label2,false);
		}
		else
			throw "loopStatement wrong, pointer: " + analyzePointer;
		if (CODE == RPARENT) {
			analyzePointer++;
			statement();
			pushQuaternion(GOTO, label1, "", "");
			pushQuaternion(LABEL, label2, "", "");
		}
		else {
			errorPut(types[analyzePointer - 1].row, 'l');
			statement();
		}
		ADDGTYPE("<循环语句>" )
		
	}
	else if (CODE == DOTK) {
		analyzePointer++;
		pushQuaternion(LABEL, label1, "", "");
		statement();
		if (CODE == WHILETK) 
			analyzePointer++;
		else
			errorPut(types[analyzePointer - 1].row, 'n');
		if (CODE == LPARENT) {
			analyzePointer++;
			condition(label1,true);
		}
		else
			throw "loopStatement wrong, pointer: " + analyzePointer;
		if (CODE == RPARENT) {
			analyzePointer++;
			ADDGTYPE("<循环语句>" )
			
		}
		else
			errorPut(types[analyzePointer - 1].row, 'l');
	}
	else if (CODE == FORTK) {
		analyzePointer++;
		string label1 = getNewLabel();
		string label2 = getNewLabel();
		string s;
		string si;
		Operation op;
		string st;
		if (CODE == LPARENT) {
			analyzePointer++;
			s = identifier();
			map<string, tableEntry>* temp = mapPointer;
			if ((*mapPointer).find(s) == (*mapPointer).end()) {
				resetPointer();
				if ((*mapPointer).find(s) == (*mapPointer).end()) {
					errorPut(types[analyzePointer - 1].row, 'c');
				}
			}
			mapPointer = temp;
		}
		else
			throw "loopStatement wrong, pointer: " + analyzePointer;
		if (CODE == ASSIGN) {
			analyzePointer++;
			pair<string,dataType> pe = expression();
			pushQuaternion(ASSIGNOP, s, pe.first, "");
		}
		else
			throw "loopStatement wrong, pointer: " + analyzePointer;
		if (CODE == SEMICN) {
			analyzePointer++;
			pushQuaternion(LABEL, label1, "", "");
			condition(label2,false);
		}
		else {
			errorPut(types[analyzePointer - 1].row, 'k');
			condition(label2,false);
		}
		if (CODE == SEMICN) {
			analyzePointer++;
			s = identifier();
			map<string, tableEntry>* temp = mapPointer;
			if ((*mapPointer).find(s) == (*mapPointer).end()) {
				resetPointer();
				if ((*mapPointer).find(s) == (*mapPointer).end()) {
					errorPut(types[analyzePointer - 1].row, 'c');
				}
			}
			mapPointer = temp;
		}
		else
			errorPut(types[analyzePointer - 1].row, 'k');
		if (CODE == ASSIGN) {
			analyzePointer++;
			si = identifier();
			map<string, tableEntry>* temp = mapPointer;
			if ((*mapPointer).find(s) == (*mapPointer).end()) {
				resetPointer();
				if ((*mapPointer).find(s) == (*mapPointer).end()) {
					errorPut(types[analyzePointer - 1].row, 'c');
				}
			}
			mapPointer = temp;
		}
		else
			throw "loopStatement wrong, pointer: " + analyzePointer;
		if (CODE == PLUS) {
			analyzePointer++;
			op = ADD;
			st = step();
		}
		else if (CODE == MINU) {
			analyzePointer++;
			op = SUB;
			st = step();
		}
		else
			throw "loopStatement wrong, pointer: " + analyzePointer;
		if (CODE == RPARENT) {
			analyzePointer++;
			statement();
		}
		else {
			errorPut(types[analyzePointer - 1].row, 'l');
			statement();
		}
		string t = getNewName();
		pushQuaternion(op, t, si, st);
		pushQuaternion(ASSIGNOP, s, t, "");
		pushQuaternion(GOTO, label1, "", "");
		pushQuaternion(LABEL, label2, "", "");
		ADDGTYPE("<循环语句>" )
	}
}

void assignStatement(identifierType t)
{
	if (t == CONST)
		errorPut(types[analyzePointer].row, 'j');
	string s = identifier();
	if (CODE == ASSIGN) {
		analyzePointer++;
		pair<string,dataType> pe = expression();
		pushQuaternion(ASSIGNOP, s, pe.first, "");
	}
	else if (CODE == LBRACK) {
		analyzePointer++;
		pair<string,dataType> pe = expression();
		string index = pe.first;
		if (pe.second != INT)
			errorPut(types[analyzePointer - 1].row, 'i');
		if (CODE == RBRACK) {
			analyzePointer++;
		}
		else
			errorPut(types[analyzePointer - 1].row, 'm');
		if (CODE == ASSIGN) {
			analyzePointer++;
			pe = expression();
			pushQuaternion(ARRASSIGN, s, index, pe.first);
		}
		else
			throw "assignStatement wrong, pointer: " + analyzePointer;
	}
	else
		throw "assignStatement wrong, pointer: " + analyzePointer;
	ADDGTYPE("<赋值语句>" )
	
}

void readStatement()
{
	if (CODE == SCANFTK) {
		analyzePointer++;
		if (CODE == LPARENT) {
			analyzePointer++;
			string s = identifier();
			dataType d;
			map<string, tableEntry>* temp = mapPointer;
			if ((*mapPointer).find(s) == (*mapPointer).end()) {
				resetPointer();
				if ((*mapPointer).find(s) == (*mapPointer).end()) {
					errorPut(types[analyzePointer - 1].row, 'c');
				}
			}
			d = (*mapPointer)[s].d;
			mapPointer = temp;
			pushQuaternion(SCANF, s, dataType2string(d), "");
			while (CODE == COMMA) {
				analyzePointer++;
				string s = identifier();
				dataType d;
				map<string, tableEntry>* temp = mapPointer;
				if ((*mapPointer).find(s) == (*mapPointer).end()) {
					resetPointer();
					if ((*mapPointer).find(s) == (*mapPointer).end()) {
						errorPut(types[analyzePointer - 1].row, 'c');
					}
				}
				d = (*mapPointer)[s].d;
				mapPointer = temp;
				pushQuaternion(SCANF, s, dataType2string(d), "");
			}
			if (CODE == RPARENT) {
				analyzePointer++;
				ADDGTYPE("<读语句>" )
				return;
			}
			else {
				errorPut(types[analyzePointer - 1].row, 'l');
				return;
			}
		}
	}
	throw "readStatement wrong, pointer: " + analyzePointer;
}

string sstring()
{
	if (CODE == STRCON) {
		analyzePointer++;
		ADDGTYPE("<字符串>")
		return types[analyzePointer - 1].value;
	}
	else
		throw "sstring wrong, pointer: " + analyzePointer;
}

void writeStatement()
{
	string s;
	pair<string, dataType> pe;
	if (CODE == PRINTFTK) {
		analyzePointer++;
		if (CODE == LPARENT) {
			analyzePointer++;
			if (CODE == STRCON) {
				s = sstring();
				if (CODE == COMMA) {
					analyzePointer++;
					pe = expression();
				}
			}
			else
				pe = expression();
			if (CODE == RPARENT) {
				analyzePointer++;
				pushQuaternion(PRINT, dataType2string(pe.second), s, pe.first);
				ADDGTYPE("<写语句>" )
				
				return;
			}
			else {
				errorPut(types[analyzePointer - 1].row, 'l');
				return;
			}
		}
	}
	throw "writeStatement wrong, pointer: " + analyzePointer;
}

pair<dataType,int> returnStatement()
{
	pair<string, dataType> pe;
	pe.second = VOID;
	if (CODE == RETURNTK) {
		analyzePointer++;
		if (CODE == LPARENT) {
			analyzePointer++;
			pe = expression();
			if (CODE == RPARENT) {
				analyzePointer++;
				pushQuaternion(RET, pe.first,"","");
			}
			else
				errorPut(types[analyzePointer - 1].row, 'l');
		}
		else {
			pushQuaternion(RET, "", "", "");
		}
		ADDGTYPE("<返回语句>" )
		return pair<dataType,int> (pe.second,types[analyzePointer-1].row);
	}
	throw "returnStatemnet wrong, pointer: " + analyzePointer;
	
}

void statementList()
{
	while (CODE != RBRACE)
		statement();
	ADDGTYPE("<语句列>" )
}

void statement()
{
	if (CODE == IFTK) {
		conditionStatement();
	}
	else if (CODE == WHILETK || CODE == DOTK || CODE == FORTK){
		loopStatement();
	}
	else if (CODE == LBRACE) {
		analyzePointer++;
		statementList();
		if (CODE != RBRACE)
			throw "statement wrong, pointer: " + analyzePointer;
		else
			analyzePointer++;
	}
	else if (CODE == SCANFTK) {
		readStatement();
		if (CODE == SEMICN)
			analyzePointer++;
		else
			errorPut(types[analyzePointer - 1].row, 'k');
	}
	else if (CODE == PRINTFTK) {
		writeStatement();
		if (CODE == SEMICN)
			analyzePointer++;
		else 
			errorPut(types[analyzePointer - 1].row, 'k');
	}
	else if (CODE == RETURNTK) {
		returnTypeCheck.push_back(returnStatement());
		if (CODE == SEMICN)
			analyzePointer++;
		else
			errorPut(types[analyzePointer - 1].row, 'k');
	}
	else if (CODE == SEMICN) {
		analyzePointer++;
	}
	else if (CODE == IDENFR) {
		string s = types[analyzePointer].value;
		map<string, tableEntry> * temp = mapPointer;
		tableEntry entry;
		bool flag = true;
		if ((*mapPointer).find(s) != (*mapPointer).end()) {
			entry = (*mapPointer)[s];
		}
		else {
			resetPointer();
			if ((*mapPointer).find(s) != (*mapPointer).end()) {
				entry = (*mapPointer)[s];
			}
			else {
				errorPut(types[analyzePointer].row, 'c');
				flag = false;
			}
		}
		mapPointer = temp;
		if (flag) {
			if (entry.t == FUNC && entry.d == VOID)
				nonReturnValueFunctionCall();
			else if (entry.t == FUNC && entry.d != VOID)
				returnValueFunctionCall();
			else
				assignStatement(entry.t);
		}
		else {
			if (types[analyzePointer + 1].code == LPARENT)
				nonReturnValueFunctionCall();
			else
				assignStatement(VAR);
		}
		if (CODE == SEMICN)
			analyzePointer++;
		else 
			errorPut(types[analyzePointer - 1].row, 'k');
	}
	else
		throw "statement wrong, pointer: " + analyzePointer;
	ADDGTYPE("<语句>" )
}

void complexStatement()
{
	if (CODE == CONSTTK)
		constantDeclaration();
	if (CODE == INTTK || CODE == CHARTK)
		variationDeclaration();
	statementList();
	ADDGTYPE("<复合语句>" )
}

void returnValueFunctionDefinition()
{
	returnTypeCheck.clear();
	pair<dataType,string> p = declarationHead();
	resetPointer();
	string s = p.second;
	dataType d = p.first;
	tableEntry * t;
	if (createMap(s) < 0)
		errorPut(types[analyzePointer - 1].row, 'b');
	else {
		(*mapPointer)[s] = { FUNC,d };
	}
	t = &((*mapPointer)[s]);
	movePointer(s);
	if (CODE == LPARENT) {
		analyzePointer++;
		pair<vector<dataType>, vector<string>> pds
			= parameters();
		t->v = pds.first;
		t->paraNames = pds.second;
		pushQuaternion(LABEL, s, "", "");
		pushQuaternion(FUNC2TABLE, dataType2string(d), s, to_string((t->v).size()));
		if (CODE == RPARENT) 
			analyzePointer++;
		else
			errorPut(types[analyzePointer - 1].row, 'l');
		if (CODE == LBRACE) {
			analyzePointer++;
			complexStatement();
			if (CODE == RBRACE) {
				analyzePointer++;
				resetPointer();
				ADDGTYPE("<有返回值函数定义>")
				pushQuaternion(FUNCEND, "", "", "");
				if (returnTypeCheck.size() == 0)
					errorPut(types[analyzePointer - 1].row, 'h');
				else
					for (int i = 0; i < returnTypeCheck.size(); i++)
						if (returnTypeCheck[i].first != d) {
							errorPut(returnTypeCheck[i].second, 'h');
							break;
						}
				return;
			}
		}
	}
	throw "returnValueFunctionDefinition wrong, pointer: " + analyzePointer;
}

void nonReturnValueFunctionDefinition()
{
	string s;
	tableEntry* t;
	returnTypeCheck.clear();
	if (CODE == VOIDTK) {
		analyzePointer++;
		s = identifier();
		resetPointer();
		if (createMap(s) < 0)
			errorPut(types[analyzePointer - 1].row, 'b');
		else {
			(*mapPointer)[s] = { FUNC,VOID };
		}
		t = &((*mapPointer)[s]);
		movePointer(s);
		if (CODE == LPARENT) {
			analyzePointer++;
			pair<vector<dataType>, vector<string>> pds
				= parameters();
			t->v = pds.first;
			t->paraNames = pds.second;
			pushQuaternion(LABEL, s, "", "");
			pushQuaternion(FUNC2TABLE, dataType2string(VOID), s, to_string((t->v).size()));
			if (CODE == RPARENT)
				analyzePointer++;
			else
				errorPut(types[analyzePointer - 1].row, 'l');
			if (CODE == LBRACE) {
				analyzePointer++;
				complexStatement();
				if (CODE == RBRACE) {
					analyzePointer++;
					pushQuaternion(RET, "", "", "");
					resetPointer();
					ADDGTYPE("<无返回值函数定义>")
					pushQuaternion(FUNCEND, "", "", "");
					if (returnTypeCheck.size() != 0) {
						for (int i = 0; i < returnTypeCheck.size(); i++)
							if (returnTypeCheck[i].first != VOID) {
								errorPut(returnTypeCheck[i].second, 'g');
								break;
							}
					}
					return;
				}
			}
	}
	}
	throw "nonReturnValueFunctionDefinition wrong, pointer: " + analyzePointer;
}

void mainFunction()
{
	if (CODE == VOIDTK) {
		analyzePointer++;
		if (CODE == MAINTK) {
			analyzePointer++;
			pushQuaternion(LABEL, "main", "", "");
			if (CODE == LPARENT) {
				analyzePointer++;
				if (CODE == RPARENT)
					analyzePointer++;
				else
					errorPut(types[analyzePointer - 1].row, 'l');
				resetPointer();
				if (createMap("main") < 0)
					throw "duplicate identifier: main";
				else {
					(*mapPointer)["main"] = { FUNC,VOID };
					movePointer("main");
				}
				pushQuaternion(FUNC2TABLE, "void", "main", "0");
				if (CODE == LBRACE) {
					analyzePointer++;
					complexStatement();
					if (CODE == RBRACE) {
						analyzePointer++;
						pushQuaternion(RET, "", "", "");
						resetPointer();
						ADDGTYPE("<主函数>")
						pushQuaternion(FUNCEND, "", "", "");
						return;
					}
				}
			}
		}
	}
	throw "mainFunction wrong, pointer: " + analyzePointer;
}

void program()
{
	pushQuaternion(CALL, "void", "main", "");
	pushQuaternion(EXIT, "", "", "");
	resetPointer();
	if (CODE == CONSTTK)
		constantDeclaration();
	if (CODE == INTTK || CODE == CHARTK) {
		if (types[analyzePointer + 2].code != LPARENT)
			variationDeclaration();
	}
	while (CODE == INTTK || CODE == CHARTK || CODE == VOIDTK) {
		if (types[analyzePointer + 1].code == MAINTK)
			break;
		if (CODE == VOIDTK)
			nonReturnValueFunctionDefinition();
		else
			returnValueFunctionDefinition();
	}
	mainFunction();
	ADDGTYPE("<程序>" )
}
