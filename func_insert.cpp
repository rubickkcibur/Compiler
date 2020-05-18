#include "optimize.h"
#include "tempCode.h"

pair<vector<string>, vector<quaternion>> replaceName(funcBlock& fb) {
	vector<string> refirst;
	map<string, string> newNames;
	vector<quaternion> resecond;
	resetPointer();
	for (string s : (*mapPointer)[fb.name].paraNames) {
		string t = getNewName();
		newNames[s] = t;
		refirst.push_back(t);
	}
	movePointer(fb.name);
	for (auto& p : (*mapPointer)) {
		if (p.second.t == VAR && p.second.paraNo < 1)
			newNames[p.first] = getNewName();
	}
	for (quaternion q : fb.bas[0].quas) {
		if (q.re[0] == '#' && newNames.find(q.re) == newNames.end()) {
			newNames[q.re] = getNewName();
		}
		Operation op = q.op;
		switch (op)
		{
		case PUSH:
			if (newNames.find(q.a) != newNames.end()) {
				q.a = newNames[q.a];
			}
			resecond.push_back(q);
			break;
		case CALL:
			resecond.push_back(q);
			break;
		case RET:
			if (newNames.find(q.re) != newNames.end()) {
				q.re = newNames[q.re];
			}
			refirst.push_back(q.re);
			resecond.push_back(q);
			break;
		case GETRET:
			if (newNames.find(q.re) != newNames.end()) {
				q.re = newNames[q.re];
			}
			resecond.push_back(q);
			break;
		case PRINT:
			if (newNames.find(q.b) != newNames.end()) {
				q.b = newNames[q.b];
			}
			resecond.push_back(q);
			break;
		case EXIT:
			break;
		case SCANF:
			if (newNames.find(q.re) != newNames.end()) {
				q.re = newNames[q.re];
			}
			resecond.push_back(q);
			break;
		
		case ASSIGNOP:
			if (newNames.find(q.a) != newNames.end()) {
				q.a = newNames[q.a];
			}
			if (newNames.find(q.re) != newNames.end()) {
				q.re = newNames[q.re];
			}
			resecond.push_back(q);
			break;
		case ARRASSIGN:
			if (newNames.find(q.a) != newNames.end()) {
				q.a = newNames[q.a];
			}
			if (newNames.find(q.b) != newNames.end()) {
				q.b = newNames[q.b];
			}
			resecond.push_back(q);
			break;
		case SLT:
		case ADD:
		case SUB:
		case TIMES:
		case DIVIDE:
			if (newNames.find(q.a) != newNames.end()) {
				q.a = newNames[q.a];
			}
			if (newNames.find(q.b) != newNames.end()) {
				q.b = newNames[q.b];
			}
			if (newNames.find(q.re) != newNames.end()) {
				q.re = newNames[q.re];
			}
			resecond.push_back(q);
			break;
		case BRACK:
			if (newNames.find(q.b) != newNames.end()) {
				q.b = newNames[q.b];
			}
			if (newNames.find(q.re) != newNames.end()) {
				q.re = newNames[q.re];
			}
			resecond.push_back(q);
			break;
		case STORE:
			resecond.push_back(q);
			break;
		case RECOVER:
			resecond.push_back(q);
			break;
		default:
			break;
		}
	}
	return { refirst,resecond };
}

void funcReplace(funcBlock& rpfb) {
	for (funcBlock& fb : funcBlocks) {
		if (fb.name == rpfb.name)
			continue;
		for (basicBlock& bb : fb.bas) {
			for (int i = 0; i < bb.quas.size(); i++) {
				quaternion& q = bb.quas[i];
				if (q.op == CALL && q.a == rpfb.name) {
					bool flag = true;
					int j;
					for (j = i - 1; j >= 0; j--) {
						if (bb.quas[j].op == STORE)
							break;
						if (bb.quas[j].op == CALL || bb.quas[j].op == RECOVER) {
							flag = false;
							break;
						}
					}
					if (flag) {
						int start = j;
						int end = i + 1;
						string revalue;
						pair<vector<string>, vector<quaternion>> re = replaceName(rpfb);
						if (re.second.back().op == RET) {
							re.second.pop_back();
							revalue = re.first.back();
							re.first.pop_back();
						}
						if (i + 2 < bb.quas.size() && bb.quas[i + 2].op == GETRET) {
							bb.quas[i + 2].op = ASSIGNOP;
							bb.quas[i + 2].a = revalue;
						}
						for (int k = j + 1; k < i; k++) {
							if (bb.quas[k].op == PUSH) {
								bb.quas[k].op = ASSIGNOP;
								bb.quas[k].re = re.first[stoi(bb.quas[k].b) - 1];
								bb.quas[k].b = "";
							}
						}
						bb.quas[start].op = NOP;
						bb.quas[end].op = NOP;
						bb.quas.insert(bb.quas.begin()+i+1,re.second.begin(), re.second.end());
						bb.quas[i].op = NOP;
					}
				}
			}
		}
	}
}

void funcInsert() {
	for (funcBlock& fb : funcBlocks) {
		if (fb.bas.size() == 1) {
			movePointer(fb.name);
			bool flag = true;
			for (auto& p : (*mapPointer)) {
				if (p.second.t == ARR) {
					flag = false;
				}
			}
			if (flag)
				funcReplace(fb);
			resetPointer();
		}
	}
}
