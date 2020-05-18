#include "optimize.h"

void constReplace() {
	bool global = true;
	for (int i = 2; i < quaternions.size();i++) {
		quaternion &q = quaternions[i];
		if (global && q.op == CONST2TABLE) {
			global = true;
		}
		else {
			global = false;
		}
		if (global) {
			/*string name = q.a;
			string value = q.b;
			for (quaternion &quat : quaternions) {
				switch (quat.op)
				{
				case CONST2TABLE:
				case VAR2TABLE:
				case ARR2TABLE:
				case FUNC2TABLE:
				case CALL:
				case GETRET:
				case EXIT:
				case SCANF:
				case LABEL:
				case GOTO:
					break;
				case RET:
					if (quat.re == name)
						quat.re = value;
				case PUSH:
					if (quat.a == name)
						quat.a = value;
					break;
				case PRINT:
				case BRACK:
					if (quat.b == name)
						quat.b = value;
					break;
				case BEQ:
				case BGEZ:
				case BGTZ:
				case BNE:
				case BLTZ:
				case BLEZ:
				case SLT:
				case ASSIGNOP:
				case ARRASSIGN:
				case ADD:
				case SUB:
				case TIMES:
				case DIVIDE:
					if (quat.a == name)
						quat.a = value;
					if (quat.b == name)
						quat.b = value;
					break;
				default:
					break;
				}
			}
			resetPointer();
			mapPointer->erase(q.a);
			q.op = NOP;*/
		}
		else {
			if (q.op == FUNC2TABLE) {
				int j;
				for (j = i + 1; quaternions[j].op != FUNCEND; j++) {
					if (quaternions[j].op == CONST2TABLE) {
						string name = quaternions[j].a;
						string value = quaternions[j].b;
						for (int k = j + 1; quaternions[k].op != FUNCEND; k++) {
							quaternion &quat = quaternions[k];
							switch (quat.op)
							{
							case CONST2TABLE:
							case VAR2TABLE:
							case ARR2TABLE:
							case FUNC2TABLE:
							case CALL:
							case GETRET:
							case EXIT:
							case SCANF:
							case LABEL:
							case GOTO:
								break;
							case RET:
								if (quat.re == name)
									quat.re = value;
							case PUSH:
								if (quat.a == name)
									quat.a = value;
								break;
							case PRINT:
							case BRACK:
								if (quat.b == name)
									quat.b = value;
								break;
							case BEQ:
							case BGEZ:
							case BGTZ:
							case BNE:
							case BLTZ:
							case BLEZ:
							case SLT:
							case ASSIGNOP:
							case ARRASSIGN:
							case ADD:
							case SUB:
							case TIMES:
							case DIVIDE:
								if (quat.a == name)
									quat.a = value;
								if (quat.b == name)
									quat.b = value;
								break;
							default:
								break;
							}
						}
						movePointer(q.a);
						mapPointer->erase(quaternions[j].a);
						quaternions[j].op = NOP;
						resetPointer();
					}
				}
				i = j;
			}
		}
	}

	global = true;
	for (int i = 2; i < quaternions.size(); i++) {
		quaternion& q = quaternions[i];
		if (global && q.op == CONST2TABLE) {
			global = true;
		}
		else {
			global = false;
		}
		if (global) {
			string name = q.a;
			string value = q.b;
			bool duplicate = false;
			for (quaternion &quat : quaternions) {
				switch (quat.op)
				{
				case FUNC2TABLE: {
					auto temp = mapPointer;
					movePointer(quat.a);
					if (mapPointer->find(q.a) != mapPointer->end())
						duplicate = true;
					mapPointer = temp;
					break;
				}
				case CONST2TABLE:
				case VAR2TABLE:
				case ARR2TABLE:
				case CALL:
				case GETRET:
				case EXIT:
				case SCANF:
				case LABEL:
				case GOTO:
					break;
				case RET:
					if (!duplicate) {
						if (quat.re == name)
							quat.re = value;
					}
					break;
				case PUSH:
					if (!duplicate) {
						if (quat.a == name)
							quat.a = value;
					}
					break;
				case PRINT:
				case BRACK:
					if (!duplicate) {
						if (quat.b == name)
							quat.b = value;
					}
					break;
				case BEQ:
				case BGEZ:
				case BGTZ:
				case BNE:
				case BLTZ:
				case BLEZ:
				case SLT:
				case ASSIGNOP:
				case ARRASSIGN:
				case ADD:
				case SUB:
				case TIMES:
				case DIVIDE:
					if (!duplicate) {
						if (quat.a == name)
							quat.a = value;
						if (quat.b == name)
							quat.b = value;
					}
					break;
				case FUNCEND:
					duplicate = false;
					break;
				default:
					break;
				}
			}
			resetPointer();
			mapPointer->erase(q.a);
			q.op = NOP;
		}
		else {
			break;
		}
	}
}

void constPropagate() {
	for (funcBlock& fb : funcBlocks) {
		for (basicBlock& bb : fb.bas) {
			for (int i = 0; i < bb.quas.size();i++) {
				quaternion &q = bb.quas[i];
				int value;
				if (isdigitchar(q.a) && isdigitchar(q.b)) {
					switch (q.op)
					{
					case SLT:
						value = str2digitchar(q.a) < str2digitchar(q.b) ? 1 : 0;
						if (q.re[0] == '#') {
							for (int j = i; j < bb.quas.size(); j++) {
								if (bb.quas[j].a == q.re)
									bb.quas[j].a = to_string(value);
								if (bb.quas[j].b == q.re)
									bb.quas[j].b = to_string(value);
								if (bb.quas[j].op == RET && bb.quas[j].re == q.re)
									bb.quas[j].re = to_string(value);
							}
							q.op = NOP;
						}
						else {
							q.op = ASSIGNOP;
							q.a = to_string(value);
							q.b = "";
						}
						break;
					case ADD:
						value = str2digitchar(q.a) + str2digitchar(q.b);
						if (q.re[0] == '#') {
							for (int j = i; j < bb.quas.size(); j++) {
								if (bb.quas[j].a == q.re)
									bb.quas[j].a = to_string(value);
								if (bb.quas[j].b == q.re)
									bb.quas[j].b = to_string(value);
								if (bb.quas[j].op == RET && bb.quas[j].re == q.re)
									bb.quas[j].re = to_string(value);
							}
							q.op = NOP;
						}
						else {
							q.op = ASSIGNOP;
							q.a = to_string(value);
							q.b = "";
						}
						break;
					case SUB:
						value = str2digitchar(q.a) - str2digitchar(q.b);
						if (q.re[0] == '#') {
							for (int j = i; j < bb.quas.size(); j++) {
								if (bb.quas[j].a == q.re)
									bb.quas[j].a = to_string(value);
								if (bb.quas[j].b == q.re)
									bb.quas[j].b = to_string(value);
								if (bb.quas[j].op == RET && bb.quas[j].re == q.re)
									bb.quas[j].re = to_string(value);
							}
							q.op = NOP;
						}
						else {
							q.op = ASSIGNOP;
							q.a = to_string(value);
							q.b = "";
						}
						break;
					case TIMES:
						value = str2digitchar(q.a) * str2digitchar(q.b);
						if (q.re[0] == '#') {
							for (int j = i; j < bb.quas.size(); j++) {
								if (bb.quas[j].a == q.re)
									bb.quas[j].a = to_string(value);
								if (bb.quas[j].b == q.re)
									bb.quas[j].b = to_string(value);
								if (bb.quas[j].op == RET && bb.quas[j].re == q.re)
									bb.quas[j].re = to_string(value);
							}
							q.op = NOP;
						}
						else {
							q.op = ASSIGNOP;
							q.a = to_string(value);
							q.b = "";
						}
						break;
					case DIVIDE:
						value = str2digitchar(q.a) / str2digitchar(q.b);
						if (q.re[0] == '#') {
							for (int j = i; j < bb.quas.size(); j++) {
								if (bb.quas[j].a == q.re)
									bb.quas[j].a = to_string(value);
								if (bb.quas[j].b == q.re)
									bb.quas[j].b = to_string(value);
								if (bb.quas[j].op == RET && bb.quas[j].re == q.re)
									bb.quas[j].re = to_string(value);
							}
							q.op = NOP;
						}
						else {
							q.op = ASSIGNOP;
							q.a = to_string(value);
							q.b = "";
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}
}