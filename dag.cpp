#include "optimize.h"

basicBlock DAG(basicBlock bb) {
	return {};
}

void assign_optimize() {
	for (funcBlock& fb : funcBlocks) {
		for (basicBlock& bb : fb.bas) {
			for (int i = 0; i < bb.quas.size(); i++) {
				quaternion& q = bb.quas[i];
				switch (q.op)
				{
				case ADD:
				case SUB:
				case TIMES:
				case DIVIDE:
				case SLT:
				case GETRET:
				case BRACK: {
					if (q.re[0] == '#') {
						if (i + 1 < bb.quas.size() 
							&& bb.quas[i + 1].op == ASSIGNOP 
							&& bb.quas[i + 1].a == q.re) {
							q.re = bb.quas[i + 1].re;
							bb.quas[i + 1].op = NOP;
						}
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}