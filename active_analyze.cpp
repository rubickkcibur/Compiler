#include "optimize.h"
#include "symbolTable.h"

set<string> difference_set(const set<string>& a, const set<string>& b) {
	set<string> re;
	for (string s : a) {
		if (b.find(s) == b.end()) {
			re.insert(s);
		}
	}
	return re;
}

set<string> union_set(const set<string>& a, const set<string>& b) {
	set<string> re;
	for (string s : a) {
		re.insert(s);
	}
	for (string s : b) {
		re.insert(s);
	}
	return re;
}

set<string> intersection(const set<string>& a, const set<string>& b) {
	set<string> re;
	for (string s : a) {
		if (b.find(s) != b.end()) {
			re.insert(s);
		}
	}
	return re;
}

void get_var_set(funcBlock& fb) {
	movePointer(fb.name);
	set<string> t;
	set<string> all;
	for (auto& p : *mapPointer) {
		all.insert(p.first);
		if (p.second.paraNo > 0 && p.second.paraNo < 5)
			fb.pvs.insert(p.first);
		else if (p.second.paraNo >= 5) {
			fb.svs.insert(p.first);
			t.insert(p.first);
		}
		else {
			t.insert(p.first);
		}
	}
	for (auto& s : fb.dfa.in) {
		fb.gvs.insert(s.begin(), s.end());
	}
	fb.gvs = intersection(fb.gvs, all);
	for (string s : fb.pvs)
		fb.gvs.erase(s);
	for (string s : fb.gvs)
		t.erase(s);
	fb.tvs = t;
	for (auto& p : *mapPointer) {
		if (p.second.t == ARR) {
			fb.gvs.erase(p.first);
			fb.tvs.erase(p.first);
		}
	}
	resetPointer();
}

void active_analyze(funcBlock& fb) {
	resetPointer();
	movePointer(fb.name);
	vector<set<string>> use;
	vector<set<string>> def;
	vector<set<string>> in;
	vector<set<string>> out;
	use.resize(fb.bas.size());
	def.resize(fb.bas.size());
	in.resize(fb.bas.size());
	out.resize(fb.bas.size());
	for (int i = 0; i < fb.bas.size(); i++) {
		auto& quas = fb.bas[i].quas;
		for (quaternion& q : quas) {
			switch (q.op)
			{
			case SCANF:
			case GETRET: {
				if (mapPointer->find(q.re) != mapPointer->end()) {
					if (use[i].find(q.re) == use[i].end()) {
						def[i].insert(q.re);
					}
				}
				break;
			}
			case PUSH: {
				if (mapPointer->find(q.a) != mapPointer->end()) {
					if (def[i].find(q.a) == def[i].end()) {
						use[i].insert(q.a);
					}
				}
				break;
			}
			case RET: {
				if (mapPointer->find(q.re) != mapPointer->end()) {
					if (def[i].find(q.re) == def[i].end()) {
						use[i].insert(q.re);
					}
				}
				break;
			}
			case PRINT: {
				if (mapPointer->find(q.b) != mapPointer->end()) {
					if (def[i].find(q.b) == def[i].end()) {
						use[i].insert(q.b);
					}
				}
				break;
			}
			case BEQ:
			case BNE:
			case BGTZ:
			case BGEZ:
			case BLTZ:
			case BLEZ: {
				if (mapPointer->find(q.b) != mapPointer->end()) {
					if (def[i].find(q.b) == def[i].end()) {
						use[i].insert(q.b);
					}
				}
				if (mapPointer->find(q.a) != mapPointer->end()) {
					if (def[i].find(q.a) == def[i].end()) {
						use[i].insert(q.a);
					}
				}
				break;
			}
			case SLT:
			case ADD:
			case SUB:
			case TIMES:
			case DIVIDE:			
			{
				if (mapPointer->find(q.b) != mapPointer->end()) {
					if (def[i].find(q.b) == def[i].end()) {
						use[i].insert(q.b);
					}
				}
				if (mapPointer->find(q.a) != mapPointer->end()) {
					if (def[i].find(q.a) == def[i].end()) {
						use[i].insert(q.a);
					}
				}
				if (mapPointer->find(q.re) != mapPointer->end()) {
					if (use[i].find(q.re) == use[i].end()) {
						def[i].insert(q.re);
					}
				}
				break;
			}
			case ASSIGNOP: {
				if (mapPointer->find(q.a) != mapPointer->end()) {
					if (def[i].find(q.a) == def[i].end()) {
						use[i].insert(q.a);
					}
				}
				if (mapPointer->find(q.re) != mapPointer->end()) {
					if (use[i].find(q.re) == use[i].end()) {
						def[i].insert(q.re);
					}
				}
				break;
			}
			case BRACK: {
				if (mapPointer->find(q.b) != mapPointer->end()) {
					if (def[i].find(q.b) == def[i].end()) {
						use[i].insert(q.b);
					}
				}
				if (mapPointer->find(q.re) != mapPointer->end()) {
					if (use[i].find(q.re) == use[i].end()) {
						def[i].insert(q.re);
					}
				}
				break;
			}
			case ARRASSIGN: {
				if (mapPointer->find(q.b) != mapPointer->end()) {
					if (def[i].find(q.b) == def[i].end()) {
						use[i].insert(q.b);
					}
				}
				if (mapPointer->find(q.a) != mapPointer->end()) {
					if (def[i].find(q.a) == def[i].end()) {
						use[i].insert(q.a);
					}
				}
				break;
			}
			default:
				break;
			}
		}
		
	}
	bool flag = true;
	while (flag) {
		flag = false;
		for (int i = fb.bas.size() - 1; i >= 0; i--) {
			set<string> temp;
			int os = out[i].size();
			int is = in[i].size();
			for (int no : fb.bbrelation[i].second) {
				if (no != -1) {
					temp = union_set(temp, in[no]);
				}
			}
			out[i] = temp;
			in[i] = union_set(use[i], difference_set(out[i], def[i]));
			if (out[i].size() != os || in[i].size() != is) {
				flag = true;
			}
		}
	}
	fb.dfa = { use,def,in,out };
	get_var_set(fb);
	resetPointer();
}

void build_dfa() {
	for (funcBlock& fb : funcBlocks)
		active_analyze(fb);
}