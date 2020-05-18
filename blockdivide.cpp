#include "optimize.h"

vector<funcBlock> funcBlocks;

void blockDivide() {
	for (int i = 0; i < quaternions.size(); i++) {
		quaternion& q = quaternions[i];
		if (q.op == FUNC2TABLE) {
			funcBlock fb;
			fb.name = q.a;
			fb.start = i;
			int pre = i + 1;
			int j;
			for (j = i + 1; quaternions[j].op != FUNCEND; j++) {
				switch (quaternions[j].op)
				{
				case BNE:
				case BEQ:
				case BGTZ:
				case BGEZ:
				case BLTZ:
				case BLEZ:
				case GOTO:
				case RET: {
					basicBlock bb;
					bb.start = pre;
					bb.end = j;
					bb.quas.insert(bb.quas.end(), quaternions.begin() + pre, quaternions.begin() + j + 1);
					if (!bb.quas.empty())
						fb.bas.push_back(bb);
					pre = j+1;
					break;
				}
				case LABEL: {
					basicBlock bb;
					bb.start = pre;
					bb.end = j-1;
					bb.quas.insert(bb.quas.end(), quaternions.begin() + pre, quaternions.begin() + j);
					if (!bb.quas.empty())
						fb.bas.push_back(bb);
					pre = j;
					break;
				}
				default:
					break;
				}
			}
			basicBlock bb;
			bb.start = pre;
			bb.end = j - 1;
			bb.quas.insert(bb.quas.end(), quaternions.begin() + pre, quaternions.begin() + j);
			if (!bb.quas.empty())
				fb.bas.push_back(bb);
			fb.end = j;
			i = j;
			funcBlocks.push_back(fb);
		}
	}
}

void setRelation() {
	for (funcBlock& fb : funcBlocks) {
		auto& relation = fb.bbrelation;
		relation.resize(fb.bas.size());
		for (int i = 0; i < fb.bas.size(); i++) {
			Operation op = fb.bas[i].quas.back().op;
			switch (op)
			{
			case GOTO: {
				string label = fb.bas[i].quas.back().re;
				for (int j = 0; j < fb.bas.size(); j++) {
					if (fb.bas[j].quas[0].op == LABEL
						&& fb.bas[j].quas[0].re == label) {
						relation[i].second.push_back(j);
						relation[j].first.push_back(i);
						break;
					}
				}
			}
				break;
			case RET: {
				relation[i].second.push_back(-1);
			}
				break;
			case BEQ:
			case BGEZ:
			case BGTZ:
			case BNE:
			case BLTZ:
			case BLEZ: {
				string label = fb.bas[i].quas.back().re;
				for (int j = 0; j < fb.bas.size(); j++) {
					if (fb.bas[j].quas[0].op == LABEL
						&& fb.bas[j].quas[0].re == label) {
						relation[i].second.push_back(j);
						relation[j].first.push_back(i);
						break;
					}
				}
				if (i != fb.bas.size() - 1) {
					relation[i].second.push_back(i + 1);
					relation[i + 1].first.push_back(i);
				}
				else {
					relation[i].second.push_back(-1);
				}
			}
				break;
			default: {
				if (i != fb.bas.size() - 1) {
					relation[i].second.push_back(i + 1);
					relation[i + 1].first.push_back(i);
				}
				else {
					relation[i].second.push_back(-1);
				}
			}
			}
		}
	}
}