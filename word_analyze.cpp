#include "heads.h"
#include "type.h"
#include "error.h"

void getToken(string& s)
{
	if (s == "")
		return;
	int i = 0;
	int status = 0;
	int start = 0;

	while (i < s.length()) {
		switch (status) {
		case 0:
			start = i;
			if (s[i] == '+')
				status = PLUS;
			else if (s[i] == '-')
				status = MINU;
			else if (s[i] == '*')
				status = MULT;
			else if (s[i] == '/')
				status = DIV;
			else if (s[i] == '<')
				status = LSS;
			else if (s[i] == '>')
				status = GRE;
			else if (s[i] == '=')
				status = ASSIGN;
			else if (s[i] == '!')
				status = NEQ;
			else if (s[i] == ';')
				status = SEMICN;
			else if (s[i] == ',')
				status = COMMA;
			else if (s[i] == '(')
				status = LPARENT;
			else if (s[i] == ')')
				status = RPARENT;
			else if (s[i] == '[')
				status = LBRACK;
			else if (s[i] == ']')
				status = RBRACK;
			else if (s[i] == '{')
				status = LBRACE;
			else if (s[i] == '}')
				status = RBRACE;
			else if (s[i] == '\'')
				status = CHARCON;
			else if (s[i] == '\"')
				status = STRCON;
			else if (isdigit(s[i]))
				status = INTCON;
			else if (isalpha(s[i]) || s[i] == '_')
				status = IDENFR;
			else if (isspace(s[i])) {
				if (s[i] == '\n')
					row++;
				i++;
			}
			else {
				errorPut(row, 'a');
				i++;
			}
			break;
		case IDENFR:
			++i;
			if (!(isalnum(s[i]) || s[i] == '_')) {
				type t = { IDENFR,s.substr(start, i-start),row };
				types.push_back(t);
				status = 0;
			}
			break;
		case INTCON:
			++i;
			if (!isdigit(s[i])) {
				type t = { INTCON, s.substr(start, i-start),row };
				types.push_back(t);
				status = 0;
			}
			break;
		case CHARCON:
			{
				++i;
				if (!(s[i] == '+' || s[i] == '-' || s[i] == '*'
					|| s[i] == '/' || isalnum(s[i]) || s[i] == '_')
					)
					errorPut(row, 'a');
				type t = { CHARCON,s.substr(i,1),row };
				types.push_back(t);
				++i;
				if (s[i] != '\'')
					errorPut(row, 'a');
				else
					++i;
				status = 0;
			}
			break;
		case STRCON:
		{
			bool flag = true;
			for (++i; s[i] != '\"'; i++)
				if (s[i] < 32) {
					errorPut(row, 'a');
					flag = false;
					break;
				}
			{
				type t = { STRCON,s.substr(start + 1,i - start - 1),row };
				types.push_back(t);
				if (flag)
					++i;
				status = 0;
			}
			break;
		}
		case PLUS:
		case MINU:
		case MULT:
		case DIV:
		case SEMICN:
		case COMMA:
		case LPARENT:
		case RPARENT:
		case LBRACK:
		case RBRACK:
		case LBRACE:
		case RBRACE:
			{
				++i;
				type t = { (typeCode)status,s.substr(start, i-start),row };
				types.push_back(t);
				status = 0;
			}
			break;
		case LSS:
		case GRE:
			{
				++i;
				if (s[i] == '=') {
					type t = { (typeCode)(status+1),s.substr(start, i+1-start),row };
					types.push_back(t);
					++i;
					status = 0;
				}
				else {
					type t = { (typeCode)status,s.substr(start, i-start),row };
					types.push_back(t);
					status = 0;
				}
			}
			break;
		case ASSIGN:
			{
				++i;
				if (s[i] == '=') {
					type t = { EQL,s.substr(start, i + 1-start),row };
					types.push_back(t);
					++i;
					status = 0;
				}
				else {
					type t = { ASSIGN,s.substr(start, i-start),row };
					types.push_back(t);
					status = 0;
				}
			}
			break;
		case NEQ:
			{
				++i;
				if (s[i] == '=') {
					type t = { NEQ,s.substr(start, i + 1-start),row };
					types.push_back(t);
					++i;
					status = 0;
				}
				else
					throw "invalid token \'!\'";
			}
			break;
		}
	
	}
}

void correct()
{
	for (int i = 0; i < types.size(); i++) {
		if (types[i].code == IDENFR) {
			string p = types[i].value;
			if (p == "const")
				types[i].code = CONSTTK;
			else if (p == "int")
				types[i].code = INTTK;
			else if (p == "char")
				types[i].code = CHARTK;
			else if (p == "void")
				types[i].code = VOIDTK;
			else if (p == "main")
				types[i].code = MAINTK;
			else if (p == "if")
				types[i].code = IFTK;
			else if (p == "else")
				types[i].code = ELSETK;
			else if (p == "do")
				types[i].code = DOTK;
			else if (p == "while")
				types[i].code = WHILETK;
			else if (p == "for")
				types[i].code = FORTK;
			else if (p == "scanf")
				types[i].code = SCANFTK;
			else if (p == "printf")
				types[i].code = PRINTFTK;
			else if (p == "return")
				types[i].code = RETURNTK;
		}
		if (types[i].code == INTCON && types[i].value.length() > 1 && types[i].value[0] == '0')
			errorPut(types[i].row, 'a');
	}
}

void wordAnalyze(ifstream& fin)
{
	string text;
	text.insert(text.end(), istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
	getToken(text);
	correct();
}