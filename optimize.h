#pragma once
#include "tempCode.h"
#include "heads.h"
#include "symbolTable.h"
#include <set>
#include <map>
struct basicBlock {
	int start;
	int end;
	vector<quaternion> quas;
};

struct data_flow_analysis {
	vector<set<string>> use;
	vector<set<string>> def;
	vector<set<string>> in;
	vector<set<string>> out;
};

struct funcBlock {
	string name;
	int start;
	int end;
	vector<basicBlock> bas;
	vector<pair<vector<int>, vector<int>>> bbrelation;
	data_flow_analysis dfa;
	set<string> gvs;
	set<string> pvs;
	set<string> tvs;
	set<string> svs;
	map<string, string> v2r;
	map<string, pair<int, int>> trs;
};

extern vector<funcBlock> funcBlocks;
extern vector<quaternion> optimized_quaternions;
extern map<string, funcBlock> name2fb;
const int K = 8;
const int T = 10;

void constReplace();
void optimize();
void blockDivide();
void constPropagate();
void setRelation();
void build_dfa();
set<string> union_set(const set<string>& a, const set<string>& b);
set<string> difference_set(const set<string>& a, const set<string>& b);
set<string> intersection(const set<string>& a, const set<string>& b);
void allocate_global_register();
void allocate_local_register();
void allocate_para_register();
void regenerate_quaternions();
void optimized_mips_generate();
void assign_optimize();
void t_register_scale();
void funcInsert();