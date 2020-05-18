#include "optimize.h"
#include "symbolTable.h"


bool is_top_four_para(string s) {
	return ((*mapPointer)[s].paraNo > 0) 
			&& ((*mapPointer)[s].paraNo < 5);
}

void allocate_s_register(string v, set<string>& se, map<string, string>& m) {
	int mark[K] = { 0 };
	for (string s : se) {
		if (m.find(s) != m.end()) {
			mark[m[s][2] - '0'] = 1;
		}
	}
	for (int i = 0; i < K; i++) {
		if (mark[i] == 0) {
			m[v] = "$s" + to_string(i);
			break;
		}
	}
}

void graph_coloring(funcBlock& fb) {
	movePointer(fb.name);
	map<string, set<string>> var_conflict;
	vector<string> nodes_with_register;
	vector<string> nodes_without_register;
	for (int i = 0; i < fb.dfa.in.size(); i++) {
		auto temp_set = intersection(fb.gvs,union_set(fb.dfa.in[i],
						union_set(fb.dfa.def[i],union_set(fb.dfa.use[i], fb.dfa.out[i]))));
		for (string s : temp_set) {
			if (var_conflict.find(s) == var_conflict.end()) {
				var_conflict[s] = temp_set;
			}
			else {
				var_conflict[s].insert(temp_set.begin(), temp_set.end()); 
			}
		}
	}
	for (auto& p : var_conflict) {
		p.second.erase(p.first);
	}

	/*if (fb.name == "complete_flower_num") {
		for (auto& p : var_conflict) {
			fdebug << p.first << " ";
			for (string s : p.second) {
				fdebug << s << " ";
			}
			fdebug << "\n";
		}
	}*/

	int n = var_conflict.size();
	auto temp_var_conflict = var_conflict;
	while (n--) {
		string s = "";
		for (auto& p : temp_var_conflict) {
			if (p.second.size() < K) {
				s = p.first;
				break;
			}
		}
		if (s != "") {
			nodes_with_register.push_back(s);
			for (auto& pe : temp_var_conflict) {
				if (pe.first != s) {
					pe.second.erase(s);
				}
			}
			temp_var_conflict.erase(s);
			/*fdebug << s << "with" << "\n";*/
		}
		else {
			s = temp_var_conflict.begin()->first;
			nodes_without_register.push_back(s);
			//
			for (auto& pe : temp_var_conflict) {
				if (pe.first != s) {
					pe.second.erase(s);
				}
			}
			//
			temp_var_conflict.erase(s);
			/*fdebug << s << "without" << "\n";*/
		}
	}
	map<string, string> var2register;
	for (string s : nodes_with_register) {
		/*var2register[s] = "mem";*/
		allocate_s_register(s, var_conflict[s], var2register);
	}
	for (string s : nodes_without_register) {
		var2register[s] = "mem";
	}
	fb.v2r = var2register;
	resetPointer();
}

void allocate_global_register() {
	for (funcBlock& fb : funcBlocks) {
		graph_coloring(fb);
	}
}

void allocate_t_register(funcBlock& fb) {
	for (basicBlock& bb : fb.bas) {
		vector<string> mark(T);
		set<string> doubleuse;
		set<string> defed;
		for (int i = 0; i < bb.quas.size(); i++) {
			string t;
			if (bb.quas[i].re[0] == '#' && bb.quas[i].op != RET) {
				t = bb.quas[i].re;
				/*这个ifelse块配合函数内联使用*/
				if (defed.find(t) == defed.end())
					defed.insert(t);
				else
					continue;
			}
			else if (bb.quas[i].op != RET && fb.tvs.find(bb.quas[i].re)!=fb.tvs.end() 
						&& fb.v2r.find(bb.quas[i].re) == fb.v2r.end())
				t = bb.quas[i].re;
			else
				continue;
			int j;
			for (j = 0; j < mark.size(); j++) {
				if (mark[j] == "")
					break;
			}
			if (j < mark.size()) {
				fb.v2r[t] = "$t" + to_string(j);
				mark[j] = t;
			}
			else {
				for (j = 0; j < mark.size(); j++) {
					if (mark[j][0] != '#')
						continue;
					bool flag = true;
					for (int k = i; k < bb.quas.size(); k++) {
						if (bb.quas[k].a == mark[j] || bb.quas[k].b == mark[j] || 
							(bb.quas[k].op == RET && bb.quas[k].re == mark[j])) {
							flag = false;
							break;
						}
					}
					if (flag) {
						mark[j] = t;
						fb.v2r[t] = "$t" + to_string(j);
						break;
					}
				}
				if (j >= mark.size()) {
					fb.v2r[t] = "mem";
				}
			}
		}
	}
}

void allocate_local_register() {
	for (funcBlock& fb : funcBlocks) {
		allocate_t_register(fb);
	}
}

void allocate_para_register() {
	for (funcBlock& fb : funcBlocks) {
		movePointer(fb.name);
		for (auto p : *mapPointer) {
			if (p.second.paraNo > 0 && p.second.paraNo <= 4) {
				fb.v2r[p.first] = "$a" + to_string(p.second.paraNo-1);
			}
		}
		resetPointer();
	}
}
