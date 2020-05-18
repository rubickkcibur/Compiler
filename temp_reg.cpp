#include "optimize.h"

void t_register_scale() {
	for (int i = 0; i < optimized_quaternions.size(); i++) {
		if (optimized_quaternions[i].op == FUNC2TABLE) {
			string funcname = optimized_quaternions[i].a;
			funcBlock& fb = name2fb[funcname];
			map<string, pair<int, int>> trs;
			int j;
			for (j = i; optimized_quaternions[j].op != FUNCEND; j++) {
				quaternion& q = optimized_quaternions[j];
				int s;
				int e;
				bool flag = false;
				string v;
				switch (q.op)
				{
				case GETRET:
				case SCANF:
				case SLT:
				case ASSIGNOP:
				case ADD:
				case SUB:
				case TIMES:
				case DIVIDE:
				case BRACK: {
					v = q.re;
					if (trs.find(v) == trs.end()
						&& fb.v2r.find(v) != fb.v2r.end()
						&& fb.v2r[v][1] == 't') {
						s = j;
						flag = true;
					}
				}
				default:
					break;
				}
				if (flag) {
					e = s;
					for (int k = j; optimized_quaternions[k].op != FUNCEND; k++) {
						quaternion& qk = optimized_quaternions[k];
						switch (qk.op)
						{
						case PUSH:
						case ASSIGNOP:
							if (qk.a == v)
								e = k;
							break;
						case RET:
							if (qk.re == v)
								e = k;
							break;
						case PRINT:
						case BRACK:
							if (qk.b == v)
								e = k;
							break;
						case BEQ:
						case BNE:
						case BGTZ:
						case BGEZ:
						case BLTZ:
						case BLEZ:
						case SLT:
						case ARRASSIGN:
						case ADD:
						case SUB:
						case TIMES:
						case DIVIDE:
							if (qk.a == v || qk.b == v)
								e = k;
							break;
						default:
							break;
						}
					}
					trs[v] = { s,e };
					if (s == e)//delete
						optimized_quaternions[s].op = NOP;
				}
			}
			fb.trs = trs;
			i = j;
		}
	}
}