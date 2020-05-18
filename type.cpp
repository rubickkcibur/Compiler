#include "type.h"

string typeCode2String(typeCode t)
{
	switch (t) {
	case IDENFR: return "IDENFR";
	case INTCON: return "INTCON";
	case CHARCON: return "CHARCON";
	case STRCON: return "STRCON";
	case CONSTTK: return "CONSTTK";
	case INTTK: return "INTTK";
	case CHARTK: return "CHARTK";
	case VOIDTK: return "VOIDTK";
	case MAINTK: return "MAINTK";
	case IFTK: return "IFTK";
	case ELSETK: return "ELSETK";
	case DOTK: return "DOTK";
	case WHILETK: return "WHILETK";
	case FORTK: return "FORTK";
	case SCANFTK: return "SCANFTK";
	case PRINTFTK: return "PRINTFTK";
	case RETURNTK: return "RETURNTK";
	case PLUS: return "PLUS";
	case MINU: return "MINU";
	case MULT: return "MULT";
	case DIV: return "DIV";
	case LSS: return "LSS";
	case LEQ: return "LEQ";
	case GRE: return "GRE";
	case GEQ: return "GEQ";
	case EQL: return "EQL";
	case NEQ: return "NEQ";
	case ASSIGN: return "ASSIGN";
	case SEMICN: return "SEMICN";
	case COMMA: return "COMMA";
	case LPARENT: return "LPARENT";
	case RPARENT: return "RPARENT";
	case LBRACK: return "LBRACK";
	case RBRACK: return "RBRACK";
	case LBRACE: return "LBRACE";
	case RBRACE: return "RBRACE";
	}
}