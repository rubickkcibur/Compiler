#include "mips.h"
#include "tempCode.h"
#include "symbolTable.h"
#include "heads.h"
#include "optimize.h"
#define OUTPUT(s) outputBuffer.push_back(s);

vector<string> outputBuffer;
map<string, int> var2addr;
int sp = 0x7fffeffc;
int orisp;
int smark[K] = { 0 };
int tmark[T] = { 0 };
int amark[4] = { 0 };
int aaddr[4] = { 0 };
int s_num;
int t_num;
int a_num;
vector<int> store_sp;
vector<int> store_a_size;
vector<vector<int>> store_t_reg;
funcBlock current_fb;

string regMap(string s) {
	if (s == "$s0")
		return "r4";
	if (s == "$s1")
		return "r5";
	if (s == "$s2")
		return "r6";
	if (s == "$s3")
		return "r7";
	if (s == "$s4")
		return "r8";
	if (s == "$s5")
		return "r10";
	if (s == "$a0")
		return "r0";
	if (s == "$a1")
		return "r1";
	if (s == "$a2")
		return "r2";
	if (s == "$a3")
		return "r3";

}

void get_t_use(vector<int>& t, int no) {
	for (auto& pt : current_fb.trs) {
		if (pt.second.first < no && pt.second.second > no) {
			if (current_fb.v2r.find(pt.first) != current_fb.v2r.end())
				if (current_fb.v2r[pt.first][1] == 't')
					t[current_fb.v2r[pt.first][2] - '0'] = 1;
		}
	}
}

pair<string, int> get_var_addr(string v) {
	if (current_fb.v2r.find(v) != current_fb.v2r.end()) {
		if (current_fb.v2r[v] != "mem") {
			if (current_fb.v2r[v][1] == 'a')
				if (amark[current_fb.v2r[v][2] - '0'] == 1)
					return{ "",aaddr[current_fb.v2r[v][2] - '0'] };
			return { current_fb.v2r[v], -1 };
		}
	}
	if (var2addr.find(v) != var2addr.end()){
		return { "",var2addr[v] };
	}
	else {
		auto tempPointer = mapPointer;
		resetPointer();
		if (mapPointer->find(v) != mapPointer->end())
			return { v,-1 };
		else
			ferr << "get_var_addr error!\n";
	}
}

bool is_con(string s) {
	return isdigit(s[0]) || s[0] == '\'' || s[0] == '-' || s[0] == '+';
}

string chulifanxiegang(string s) {
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == '\\') {
			s.insert(i, "\\");
			i++;
		}
	}
	return s;
}

string getNewSname() {
	static int i = 1;
	return "string" + to_string(i++);
}

void const2table(quaternion q)
{
	if (mapPointer == &globalMap) {
		OUTPUT(".data\n" + q.a + ": .word " + q.b)
			OUTPUT(".text\n");
	}
	else {
		
	}
}

void var2table(quaternion q)
{
	if (mapPointer == &globalMap) {
		//OUTPUT(".data\n" + q.a + ":.space 4");
		//OUTPUT(".text\n");
		OUTPUT(".data\n" + q.a + ":.zero 4");
		OUTPUT(".text\n");
	}
	else {

	}
}

void arr2table(quaternion q)
{
	int length = stoi(q.b);
	if (mapPointer == &globalMap) {
		//OUTPUT(".data\n" + q.a + ":.space " + to_string(length * 4));
		//OUTPUT(".text\n");
		OUTPUT(".data\n" + q.a + ":.zero " + to_string(length * 4));
		OUTPUT(".text\n");
	}
	else {
	}
}

void func2table(quaternion q) {
	sp = 0;
	resetPointer();
	tableEntry te = (*mapPointer)[q.a];
	current_fb = name2fb[q.a];
	vector<string> names = te.paraNames;
	a_num = names.size();
	movePointer(q.a);
	var2addr.clear();
	int paracnt = 0;
	for (auto& p : *mapPointer) {
		if (p.second.paraNo >= 5) {
			var2addr[p.first] = (4 - p.second.paraNo) * 4;
			paracnt++;
		}
	}
	sp -= paracnt * 4;
	for (auto& p : *mapPointer) {
		if (p.second.t == ARR) {
			sp -= p.second.arrlength * 4;
			var2addr[p.first] = sp;
		}
		else {
			if (current_fb.v2r[p.first] == "mem") {
				sp -= 4;
				var2addr[p.first] = sp;
			}
		}
	}
	//
	for (auto& p : current_fb.v2r) {
		if (p.second == "mem" && var2addr.find(p.first) == var2addr.end()) {
			sp -= 4;
			var2addr[p.first] = sp;
		}
	}
	//
	for (int i = 0; i < K; i++) {
		smark[i] = 0;
	}
	for (int i = 0; i < T; i++) {
		tmark[i] = 0;
	}
	for (auto& p : current_fb.v2r) {
		if (p.second == "")
			continue;
		if (p.second[1] == 's') {
			smark[p.second[2] - '0'] = 1;
		}
		if (p.second[1] == 't') {
			tmark[p.second[2] - '0'] = 1;
		}
	}
	s_num = 0;
	for (int i : smark)
		s_num += i;
	sp -= s_num * 4;
	t_num = 0;
	for (int i : tmark)
		t_num += i;

	OUTPUT(".text");
	if (sp+paracnt*4 != 0)
		//OUTPUT("addiu $sp,$sp," + to_string(sp+paracnt*4));
		OUTPUT("ADD SP,SP,#" + to_string(sp + paracnt * 4));
	int cnt = 0;
	for (int i = 0; i < K; i++) {
		if (smark[i] == 1) {
			//OUTPUT("sw $s" + to_string(i) + "," + to_string(cnt*4) + "($sp)");
			OUTPUT("STR " + regMap("$s" + to_string(i)) + ",[SP,#" + to_string(cnt * 4) + "]");
			cnt++;
		}
	}
	for (auto& p : *mapPointer) {
		if (p.second.paraNo >= 5 && current_fb.v2r[p.first] != "mem") {
			string reg = current_fb.v2r[p.first];
			OUTPUT("LDR " + regMap(reg) + ",[SP,#" + to_string(var2addr[p.first] - sp) + "]");
		}
	}
}

void store(quaternion q,int no) {
	/*cout << "store" << "\n";
	cout << sp << "\n";*/
	bool outerist = store_sp.empty();;
	int a_cnt = a_num > 4 ? 4 : a_num;
	int a_dirty = 0;
	for (int i = a_cnt; i < 4; i++)
		if (amark[i] == 1)
			a_dirty++;
	int a_size = outerist ? a_cnt : a_cnt + a_dirty;
	
	vector<int> t_reg_mark(T);
	get_t_use(t_reg_mark, no);
	int t_cnt = 0;
	for (int i : t_reg_mark)
		t_cnt += i;


	//OUTPUT("addiu $sp,$sp," + to_string(-t_cnt*4 -a_size*4 - 4));
	OUTPUT("ADD SP,SP,#" + to_string(-t_cnt * 4 - a_size * 4 - 4));
	store_a_size.push_back(a_size);
	store_t_reg.push_back(t_reg_mark);
	sp -= (t_cnt+a_size+1)*4;
	store_sp.push_back(sp);
	//OUTPUT("sw $ra,0($sp)");
	OUTPUT("STR r14,[SP]");
	int cnt = t_cnt;
	for (int i = 0; i < T; i++) {
		if (t_reg_mark[i] == 1) {
			OUTPUT("sw $t" + to_string(i) + "," + to_string(cnt * 4) + "($sp)");//理应走不到
			cnt--;
		}
	}
	for (int i = 0; i < a_cnt; i++) {
		//OUTPUT("sw $a" + to_string(i) + "," + to_string((t_cnt + i + 1) * 4) + "($sp)");
		OUTPUT("STR a" + to_string(i+1) + ",[SP,#" + to_string((t_cnt + i + 1) * 4) + "]");
		if (outerist) {
			aaddr[i] = (t_cnt + i + 1) * 4 + sp;
			/*cout << "aaddr" << i << ":" << aaddr[i] << "\n";*/
		}
	}
	if (!outerist) {
		for (int i = a_cnt; i < 4; i++) {
			if (amark[i] == 1) {
				//OUTPUT("sw $a" + to_string(i) + "," + to_string((t_cnt + i + 1) * 4) + "($sp)");
				OUTPUT("STR a" + to_string(i+1) + ",[SP,#" + to_string((t_cnt + i + 1) * 4) + "]");
			}
		}
	}
	/*cout << sp << "\n";*/
}

void recover(quaternion q) {
	/*cout << "recover" << "\n";
	cout << sp << "\n";*/
	bool outerist;
	outerist = (store_sp.size() == 1);
	//OUTPUT("lw $ra,0($sp)");
	OUTPUT("LDR r14,[SP]");

	vector<int> t_reg_mark = store_t_reg.back();
	store_t_reg.pop_back();
	int t_cnt = 0;
	for (int i : t_reg_mark)
		t_cnt += i;
	int cnt = t_cnt;
	
	int a_cnt = a_num > 4 ? 4 : a_num;
	int a_size_temp = store_a_size.back();
	store_a_size.pop_back();
	int a_size = outerist ? a_cnt : a_size_temp;
	for (int i = 0; i < T; i++) {
		if (t_reg_mark[i] == 1) {
			OUTPUT("lw $t" + to_string(i) + "," + to_string(cnt * 4) + "($sp)");//理应走不到
			cnt--;
		}
	}
	for (int i = 0; i < a_cnt; i++) {
		//OUTPUT("lw $a" + to_string(i) + "," + to_string((t_cnt + i + 1) * 4) + "($sp)");
		OUTPUT("LDR a" + to_string(i+1) + ",[SP,#" + to_string((t_cnt + i + 1) * 4) + "]");
	}
	if (!outerist) {
		for (int i = a_cnt; i < a_size; i++) {
			//OUTPUT("lw $a" + to_string(i) + "," + to_string((t_cnt + i + 1) * 4) + "($sp)");
			OUTPUT("LDR a" + to_string(i+1) + ",[SP,#" + to_string((t_cnt + i + 1) * 4) + "]");
		}
	}
	if (outerist) {
		for (int i = 0; i < 4; i++) {
			amark[i] = 0;
			aaddr[i] = 0;
		}
	}
	sp = store_sp.back();
	store_sp.pop_back();
	sp += (t_cnt +a_size+ 1) * 4;
	//OUTPUT("addiu $sp,$sp," + to_string((t_cnt +a_size+ 1) * 4));
	OUTPUT("ADD SP,SP,#" + to_string((t_cnt + a_size + 1) * 4));
	/*cout << sp << "\n";*/
}

void push(quaternion q) {
	/*cout << "push" << "\n";
	cout << sp << "\n";*/
	if (is_con(q.a)) {
		if (stoi(q.b) <= 4) {
			//OUTPUT("li $a" + to_string(stoi(q.b) - 1) + "," + q.a);
			OUTPUT("MOVW a" + to_string(stoi(q.b)) + ",#" + q.a);
			amark[stoi(q.b) - 1] = 1;
		}
		else {
			//OUTPUT("li $k0," + q.a);
			OUTPUT("MOVW r12,#" + q.a);
			//OUTPUT("addiu $sp,$sp,-4");
			//sp -= 4;
			//OUTPUT("sw $k0,0($sp)");
			OUTPUT("PUSH {r12}");
			sp -= 4;
		}
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			if (stoi(q.b) <= 4) {
				//OUTPUT("lw $a" + to_string(stoi(q.b) - 1) + "," + to_string(p.second - sp) + "($sp)");
				OUTPUT("LDR a" + to_string(stoi(q.b)) + ",[SP,#" + to_string(p.second - sp) + "]");
				amark[stoi(q.b) - 1] = 1;
			}
			else {
				//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
				OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
				//OUTPUT("addiu $sp,$sp,-4");
				//sp -= 4;
				//OUTPUT("sw $k0,0($sp)");
				OUTPUT("PUSH {r12}");
				sp -= 4;
			}
		}
		else if (p.first[0] == '$') {
			if (stoi(q.b) <= 4) {
				if (p.first[1] == 'a' && p.first[2] == stoi(q.b) - 1 + '0')//delete
					;
				else
					//OUTPUT("move $a" + to_string(stoi(q.b) - 1) + "," + p.first);
					OUTPUT("MOV a" + to_string(stoi(q.b)) + ",a" + to_string(p.first[2]-'0'+1));
				amark[stoi(q.b) - 1] = 1;
			}
			else {
				//OUTPUT("addiu $sp,$sp,-4");
				//sp -= 4;
				//OUTPUT("sw " + p.first + ",0($sp)");
				OUTPUT("PUSH {a" + to_string(p.first[2] - '0' + 1) + "}");
			}
		}
		else {
			if (stoi(q.b) <= 4) {
				//OUTPUT("lw $a" + to_string(stoi(q.b) - 1) + "," + p.first);
				OUTPUT("LDR a" + to_string(stoi(q.b)) + "," + p.first);
				amark[stoi(q.b) - 1] = 1;
			}
			else {
				//OUTPUT("lw $k0," + p.first)
				OUTPUT("LDR r12," + p.first)
				//OUTPUT("addiu $sp,$sp,-4");
				//sp -= 4;
				//OUTPUT("sw $k0,0($sp)");
				OUTPUT("PUSH {r12}");
				sp -= 4;
			}
		}
	}
	/*cout << sp << "\n";*/
}

void call(quaternion q) {
	//OUTPUT("jal " + q.a);
	OUTPUT("BL " + q.a);
}

void ret(quaternion q) {
	int off = orisp - sp;
	if (q.re != "") {
		if (is_con(q.re)) {
			//OUTPUT("li $v0," + q.re);
			OUTPUT("MOV a1,#" + q.re);
		}
		else {
			auto p = get_var_addr(q.re);
			if (p.second != -1) {
				//OUTPUT("lw $v0," + to_string(p.second - sp) + "($sp)")
				OUTPUT("LDR a1,[SP,#" + to_string(p.second - sp) + "]")
			}
			else if (p.first[0] == '$') {
				//OUTPUT("move $v0," + p.first);
				OUTPUT("MOV a1," + regMap(p.first))
			}
			else {
				//OUTPUT("lw $v0," + p.first)
				OUTPUT("LDR a1," + p.first)
			}
		}
	}
	int cnt = 0;
	for (int i = 0; i < K; i++) {
		if (smark[i] == 1) {
			//OUTPUT("lw $s" + to_string(i) + "," + to_string(cnt * 4) + "($sp)");
			OUTPUT("LDR " + regMap("$s" + to_string(i)) + ",[SP,#" + to_string(cnt * 4) + "]")
			cnt++;
		}
	}
	//OUTPUT("addiu $sp,$sp," + to_string(-sp));
	OUTPUT("ADD SP,SP,#" + to_string(-sp));
	//OUTPUT("jr $ra")
	OUTPUT("MOV PC,LR")
}

void getret(quaternion q) {
	auto p = get_var_addr(q.re);
	if (p.second != -1) {
		//OUTPUT("sw $v0," + to_string(p.second - sp) + "($sp)")
		OUTPUT("STR a1,[SP,#" + to_string(p.second - sp) + "]")
	}
	else if (p.first[0] == '$') {
		//OUTPUT("move " + p.first + ",$v0");
		OUTPUT("MOV " + regMap(p.first) + ",a1")
	}
	else {
		//OUTPUT("sw $v0," + p.first)
		OUTPUT("STR a1,"+p.first)
	}
}

void print(quaternion q) { //not done
	string sname = getNewSname();
	if (q.a != "") {
		OUTPUT(".data\n");
		if (q.re != "void")
			OUTPUT(sname + ":.asciiz \"" + chulifanxiegang(q.a) + "\"")
		else
			OUTPUT(sname + ":.asciiz \"" + chulifanxiegang(q.a) + "\\n\"")
			OUTPUT(".text\n")
		if (a_num != 0)
			;
			//OUTPUT("move $k0,$a0")
		//OUTPUT("la $a0," + sname)
		//OUTPUT("li $v0,4")
		//OUTPUT("syscall")
		OUTPUT("PUSH {a1}")
		OUTPUT("ADR a1," + sname)
		OUTPUT("PUSH {LR}")
		sp-=8;
		OUTPUT("BL putf")
		OUTPUT("POP {LR}")
		OUTPUT("POP {a1}")
		sp += 8;
		if (a_num != 0)
			;
			//OUTPUT("move $a0,$k0")
	}
	if (q.re != "void") {
		if (is_con(q.b)) {
			if (a_num != 0)
				;
				//OUTPUT("move $k0,$a0")
			OUTPUT("PUSH {a1}")
				//OUTPUT("li $a0," + q.b);
			OUTPUT("MOVW a1,#" + q.b)
			sp -= 4;
		}
		else {
			auto p = get_var_addr(q.b);
			if (p.second != -1) {
				if (a_num != 0)
					;
					//OUTPUT("move $k0,$a0")
				OUTPUT("PUSH {a1}")
				sp -= 4;
				//OUTPUT("lw $a0," + to_string(p.second - sp) + "($sp)")
				OUTPUT("LDR a1,[SP,#" + to_string(p.second - sp) + "]")
			}
			else if (p.first[0] == '$') {
				if (a_num != 0)
					;
					//OUTPUT("move $k0,$a0")
				OUTPUT("PUSH {a1}")
				sp -= 4;
				//OUTPUT("move $a0," + p.first);
				OUTPUT("MOV a1," + regMap(p.first));
			}
			else {
				if (a_num != 0)
					;
					//OUTPUT("move $k0,$a0")
				OUTPUT("PUSH {a1}")
				sp -= 4;
				//OUTPUT("lw $a0," + p.first)
				OUTPUT("LDR a1," + p.first)
			}
		}
		if (q.re == "int") {
			OUTPUT("PUSH {LR}")
			sp -= 4;
			OUTPUT("BL putint")
			OUTPUT("POP {LR}")
			sp += 4;
			//OUTPUT("li $v0,1");
		}
		else if (q.re == "char") {
			OUTPUT("PUSH {LR}")
			sp -= 4;
			OUTPUT("BL putchar")
			OUTPUT("POP {LR}")
			sp += 4;
			//OUTPUT("li $v0,11");
		}
		OUTPUT("POP {a1}")
		sp += 4;
		//OUTPUT("syscall");
		//OUTPUT("la $a0,int");
		//OUTPUT("li $v0,4");
		//OUTPUT("syscall");
		if (a_num != 0)
			;
			//OUTPUT("move $a0,$k0")
	}
}

void exit(quaternion q) { //not done
	//OUTPUT("li $v0,10");
	//OUTPUT("syscall");
	OUTPUT("MOV PC,LR");
}

void scanf(quaternion q) { //not done
	if (q.a == "int") {
		//OUTPUT("li $v0,5")
		sp -= 4;
		OUTPUT("PUSH {LR}")
		OUTPUT("BL getint")
	}
	else if (q.a == "char") {
		//OUTPUT("li $v0,12")
		sp -= 4;
		OUTPUT("PUSH {LR}")
		OUTPUT("BL getchar")
	}
	//OUTPUT("syscall");
	OUTPUT("POP {LR}")
	auto p = get_var_addr(q.re);
	if (p.second != -1) {
		//OUTPUT("sw $v0," + to_string(p.second - sp) + "($sp)")
		OUTPUT("STR a1,[SP,#" + to_string(p.second - sp) + "]")
	}
	else if (p.first[0] == '$') {
		//OUTPUT("move " + p.first + ",$v0");
		OUTPUT("MOV " + regMap(p.first) + ",a1");
	}
	else {
		//OUTPUT("sw $v0," + p.first)
		OUTPUT("STR a1," + p.first)
	}
}

void label(quaternion q) {
	OUTPUT(q.re + ":");
}

void j(quaternion q) {
	//OUTPUT("j " + q.re);
	OUTPUT("B " + q.re);
}

void beq(quaternion q) {
	string reg1 = "r11";
	if (is_con(q.a)) {
		/*
		if (q.a[0] != '\'' && stoi(q.a) == 0) {
			reg1 = "$0";
		}
		else
		{
			OUTPUT("li $k0," + q.a);
		}*/
		OUTPUT("MOVW r11,#"+ q.a)
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			if (p.first[1] == 's')
				reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	string reg2 = "r12";
	if (is_con(q.b)) {
		/*
		if (q.b[0] != '\'' && stoi(q.b) == 0) {
			reg2 = "$0";
		}
		else
		{
			OUTPUT("li $k1," + q.b);
		}*/
		OUTPUT("MOVW r12,#" + q.b)
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//OUTPUT("beq " + reg1 + "," + reg2 + "," + q.re);
	OUTPUT("CMP " + reg1 + "," + reg2);
	OUTPUT("BEQ " + q.re);
}

void bne(quaternion q) {
	string reg1 = "r11";
	if (is_con(q.a)) {
		/*
		if (q.a[0] != '\'' && stoi(q.a) == 0) {
			reg1 = "$0";
		}
		else
		{
			OUTPUT("li $k0," + q.a);
		}*/
		OUTPUT("MOVW r11,#" + q.a)
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	string reg2 = "r12";
	if (is_con(q.b)) {
		/*
		if (q.b[0] != '\'' && stoi(q.b) == 0) {
			reg2 = "$0";
		}
		else
		{
			OUTPUT("li $k1," + q.b);
		}*/
		OUTPUT("MOVW r12,#" + q.b)
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//OUTPUT("beq " + reg1 + "," + reg2 + "," + q.re);
	OUTPUT("CMP " + reg1 + "," + reg2);
	OUTPUT("BNE " + q.re);
}

void bgez(quaternion q) { //用不到
	string reg1 = "$k0";
	if (is_con(q.a)) {
		OUTPUT("li $k0," + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			reg1 = p.first;
		}
		else {
			OUTPUT("lw $k0," + p.first)
		}
	}
	OUTPUT("bgez " + reg1 + "," + q.re);
}

void bgtz(quaternion q) { //用不到
	string reg1 = "$k0";
	if (is_con(q.a)) {
		OUTPUT("li $k0," + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			reg1 = p.first;
		}
		else {
			OUTPUT("lw $k0," + p.first)
		}
	}
	OUTPUT("bgtz " + reg1 + "," + q.re);
}

void bltz(quaternion q) { //用不到
	string reg1 = "$k0";
	if (is_con(q.a)) {
		OUTPUT("li $k0," + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			reg1 = p.first;
		}
		else {
			OUTPUT("lw $k0," + p.first)
		}
	}
	OUTPUT("bltz " + reg1 + "," + q.re);
}

void blez(quaternion q) { //用不到
	string reg1 = "$k0";
	if (is_con(q.a)) {
		OUTPUT("li $k0," + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			reg1 = p.first;
		}
		else {
			OUTPUT("lw $k0," + p.first)
		}
	}
	OUTPUT("blez " + reg1 + "," + q.re);
}

void slt(quaternion q) {
	string reg1 = "r11";
	if (is_con(q.a)) {
		//OUTPUT("li $k0," + q.a);
		OUTPUT("MOVW r11,#" + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11,"+p.first)
		}
	}
	string reg2 = "r12";
	if (is_con(q.b)) {
		//reg2 = q.b;
		OUTPUT("MOVW r12,#" + q.b);
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r12," + p.first);
		}
	}
	auto p = get_var_addr(q.re);
	/*
	if (reg2[0] == '$') {
		if (p.second != -1) {
			OUTPUT("slt $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			OUTPUT("slt " + p.first + "," + reg1 + "," + reg2)
		}
		else {
			OUTPUT("slt $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + p.first)
		}
	}
	else {
		if (p.second != -1) {
			OUTPUT("slti $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			OUTPUT("slti " + p.first + "," + reg1 + "," + reg2)
		}
		else {
			OUTPUT("slti $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + p.first)
		}
	}*/
	OUTPUT("CMP " + reg1 + "," + reg2);
	OUTPUT("MOVLT r12,#1");
	OUTPUT("MOVGE r12,#0");
	if (p.second != -1) {
		//OUTPUT("slti $k0," + reg1 + "," + reg2)
		//OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		OUTPUT("STR r12,[SP,#" + to_string(p.second - sp) + "]");
	}
	else if (p.first[0] == '$') {
		//OUTPUT("slti " + p.first + "," + reg1 + "," + reg2)
		OUTPUT("MOV " + regMap(p.first) + ",r12")
	}
	else {
		//OUTPUT("slti $k0," + reg1 + "," + reg2)
		//OUTPUT("sw $k0," + p.first)
		OUTPUT("STR r12," + p.first)
	}

}

void assignop(quaternion q) {
	string reg1 = "r12";
	if (is_con(q.a)) {
		auto pe = get_var_addr(q.re);
		if (pe.second != -1) {
			//OUTPUT("li $k0," + q.a)
			//OUTPUT("sw $k0," + to_string(pe.second - sp) + "($sp)")
			OUTPUT("MOVW r12,#" + q.a)
			OUTPUT("STR r12,[SP,#" + to_string(pe.second - sp) + "]")
		}
		else if (pe.first[0] == '$') {
			//OUTPUT("li " + pe.first + "," + q.a)
			OUTPUT("MOVW " + regMap(pe.first) + "," + q.a)
		}
		else {
			//OUTPUT("li $k0," + q.a)
			//OUTPUT("sw $k0," + pe.first)
			OUTPUT("MOVW r12,#" + q.a)
			OUTPUT("STR r12," + pe.first)
		}
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			auto pe = get_var_addr(q.re);
			if (pe.second != -1) {
				//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
				//OUTPUT("sw $k0," + to_string(pe.second - sp) + "($sp)")
				OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
				OUTPUT("STR r12,[SP,#" + to_string(pe.second - sp) + "]")
			}
			else if (pe.first[0] == '$') {
				//OUTPUT("lw " + pe.first + "," + to_string(p.second - sp) + "($sp)")
				OUTPUT("LDR " + regMap(pe.first) + ",[SP,#" + to_string(p.second - sp) + "]")
			}
			else {
				//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
				//OUTPUT("sw $k0," + pe.first)
				OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
				OUTPUT("STR r12," + pe.first)
			}
		}
		else if (p.first[0] == '$') {
			auto pe = get_var_addr(q.re);
			if (pe.second != -1) {
				//OUTPUT("sw " + p.first + "," + to_string(pe.second - sp) + "($sp)")
				OUTPUT("STR " + regMap(p.first) + ",[SP,#" + to_string(pe.second - sp) + "]")
			}
			else if (pe.first[0] == '$') {
				if (pe.first != p.first)//delete
					//OUTPUT("move " + pe.first + "," + p.first)
					OUTPUT("MOV " + regMap(pe.first) + "," + regMap(p.first))
			}
			else {
				//OUTPUT("sw " + p.first + "," + pe.first)
				OUTPUT("STR " + regMap(p.first) + "," + pe.first)
			}
		}
		else {
			auto pe = get_var_addr(q.re);
			if (pe.second != -1) {
				//OUTPUT("lw $k0," + p.first)
				//OUTPUT("sw $k0," + to_string(pe.second - sp) + "($sp)")
				OUTPUT("LDR r12," + p.first)
				OUTPUT("STR r12,[SP,#" + to_string(pe.second - sp) + "]")
			}
			else if (pe.first[0] == '$') {
				//OUTPUT("lw " + pe.first + "," + p.first)
				OUTPUT("LDR " + regMap(pe.first) + "," + p.first)
			}
			else {
				//OUTPUT("lw $k0," + p.first)
				//OUTPUT("sw $k0," + pe.first)
				OUTPUT("LDR r12," + p.first)
				OUTPUT("STR r12," + pe.first)
			}
		}
	}
}

void arrassign(quaternion q) {
	//lwxc1 swxc1
	string reg1 = "r11";
	if (is_con(q.b)) {
		//OUTPUT("li $k0," + q.b);
		OUTPUT("MOVW r11,#" + q.b)
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//re label  a con		sw b label+con
	//re label	a off		avalue->$ sw b label($)  
	//re label  a $					  sw b label($)
	//re label  a label		avalue->$ sw b label($)
	//re off	a con		sw b off+con($sp)
	//re off	a off		avalue->$ + $sp sw off($)
	//re off	a off				$ + $sp sw off($)
	//re off	a label		avalue->$ + $sp sw off($)
	auto pre = get_var_addr(q.re);
	if (pre.second != -1) {
		string reg2 = "r12";
		if (is_con(q.a)) {
			string aoff = to_string(stoi(q.a) * 4);
			int off = pre.second - sp + stoi(q.a) * 4;
			//OUTPUT("sw " + reg1 + "," +to_string(off) + "($sp)")
			OUTPUT("STR " + reg1 + ",[SP,#" + to_string(off) + "]")
		}
		else {
			auto pa = get_var_addr(q.a);
			if (pa.second != -1) {
				//OUTPUT("lw $k1," + to_string(pa.second - sp) + "($sp)")
				//OUTPUT("sll $k1,$k1,2")
				OUTPUT("LDR r12,[SP,#" + to_string(pa.second - sp) + "]")
				OUTPUT("LSL r12,r12,#2")
			}
			else if (pa.first[0] == '$') {
				//reg2 = pa.first;
				//OUTPUT("sll $k1," + pa.first + ",2")
				reg2 = regMap(pa.first);
				OUTPUT("LSL r12," + regMap(pa.first) + ",#2")
			}
			else {
				//OUTPUT("lw $k1," + pa.first)
				//OUTPUT("sll $k1,$k1,2")
				OUTPUT("LDR r12," + pa.first)
				OUTPUT("LSL r12,r12,#2")
			}
			//OUTPUT("addu $k1,$k1,$sp" )
			//OUTPUT("sw " + reg1 + "," + to_string(pre.second - sp) + "($k1)")
			OUTPUT("ADD r12,r12,SP")
			OUTPUT("STR " + reg1 + ",[r12,#" + to_string(pre.second - sp) + "]")
		}
	}
	else {
		//string reg2 = "$k1";
		string reg2 = "r12";
		if (is_con(q.a)) {
			string aoff = to_string(stoi(q.a) * 4);
			//OUTPUT("sw " + reg1 + "," + pre.first + "+" + aoff)
			OUTPUT("STR " + reg1 + "," + pre.first + "+" + aoff) //not done
		}
		else {
			auto pa = get_var_addr(q.a);
			if (pa.second != -1) {
				//OUTPUT("lw $k1," + to_string(pa.second - sp) + "($sp)")
				//OUTPUT("sll $k1,$k1,2")
				OUTPUT("LDR r12,[SP,#" + to_string(pa.second - sp) + "]")
				OUTPUT("LSL r12,r12,#2")
			}
			else if (pa.first[0] == '$') {
				//reg2 = pa.first;
				//OUTPUT("sll $k1," + pa.first + ",2")
				reg2 = regMap(pa.first);
				OUTPUT("LSL r12," + regMap(pa.first) + ",#2")
			}
			else {
				//OUTPUT("lw $k1," + pa.first)
				//OUTPUT("sll $k1,$k1,2")
				OUTPUT("LDR r12," + pa.first)
				OUTPUT("LSL r12,r12,#2")
			}
			//OUTPUT("sw " + reg1 + "," + pre.first + "($k1)")
			OUTPUT("STR " + reg1 + ",[r12," + pre.first + "]") // not done
		}
	}
}

void add(quaternion q) {
	//string reg1 = "$k0";
	string reg1 = "r11";
	if (is_con(q.a)) {
		//reg1 = q.a;
		OUTPUT("MOVW r11,#"+q.a)
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//string reg2 = "$k1";
	string reg2 = "r12";
	if (is_con(q.b)) {
		//reg2 = q.b;
		OUTPUT("MOVW r12,#" + q.b);
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//UTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r12," + p.first)
		}
	}
	auto p = get_var_addr(q.re);
	if (p.second != -1) {
		/*
		if (reg1[0] != '$') {
			OUTPUT("addiu " + reg2 + "," + reg2 + "," + reg1)
			OUTPUT("sw " + reg2 + "," + to_string(p.second - sp) + "($sp)")
		}
		else if (reg2[0] != '$') {
			OUTPUT("addiu " + reg1 + "," + reg1 + "," + reg2)
			OUTPUT("sw " + reg1 + "," + to_string(p.second - sp) + "($sp)")
		}
		else {
			OUTPUT("addu " + reg1 + "," + reg1 + "," + reg2)
			OUTPUT("sw " + reg1 + "," + to_string(p.second - sp) + "($sp)")
		}*/
		OUTPUT("ADD " + reg1 + "," + reg1 + "," + reg2)
		OUTPUT("STR " + reg1 + ",[SP,#" + to_string(p.second - sp) + "]")
	}
	else if (p.first[0] == '$') {
		/*
		if (reg1[0] != '$') {
			OUTPUT("addiu " + p.first + "," + reg2 + "," + reg1)
		}
		else if (reg2[0] != '$') {
			OUTPUT("addiu " + p.first + "," + reg1 + "," + reg2)
		}
		else {
			OUTPUT("addu " + p.first + "," + reg1 + "," + reg2)
		}*/
		OUTPUT("ADD " + regMap(p.first) + "," + reg1 + "," + reg2)
	}
	else {
		/*
		if (reg1[0] != '$') {
			OUTPUT("addiu " + reg2 + "," + reg2 + "," + reg1)
			OUTPUT("sw " + reg2 + "," + p.first)
		}
		else if (reg2[0] != '$') {
			OUTPUT("addiu " + reg1 + "," + reg1 + "," + reg2)
			OUTPUT("sw " + reg1 + "," + p.first)
		}
		else {
			OUTPUT("addu " + reg1 + "," + reg1 + "," + reg2)
			OUTPUT("sw " + reg1 + "," + p.first)
		}*/
		OUTPUT("ADD " + reg1 + "," + reg1 + "," + reg2)
		OUTPUT("STR " + reg1 + "," + p.first)
	}
}

void sub(quaternion q) {
	//string reg1 = "$k0";
	string reg1 = "r11";
	if (is_con(q.a)) {
		//OUTPUT("li $k0," + q.a);
		OUTPUT("MOVW r11,#" + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//string reg2 = "$k1";
	string reg2 = "r12";
	if (is_con(q.b)) {
		//reg2 = q.b;
		OUTPUT("MOVW r12,#" + q.b);
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r12," + p.first)
		}
	}

	auto p = get_var_addr(q.re);
	/*
	if (reg2[0] == '$') {
		if (p.second != -1) {
			OUTPUT("subu $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			OUTPUT("subu " + p.first + "," + reg1 + "," + reg2)
		}
		else {
			OUTPUT("subu $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + p.first)
		}
	}
	else {
		if (p.second != -1) {
			OUTPUT("subiu $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		}
		else if (p.first[0] == '$') {
			OUTPUT("subiu " + p.first + "," + reg1 + "," + reg2)
		}
		else {
			OUTPUT("subiu $k0," + reg1 + "," + reg2)
			OUTPUT("sw $k0," + p.first)
		}
	}*/
	if (p.second != -1) {
		OUTPUT("SUB r11," + reg1 + "," + reg2)
		OUTPUT("STR r11,[SP,#," + to_string(p.second - sp) + "]")
	}
	else if (p.first[0] == '$') {
		OUTPUT("SUB " + regMap(p.first) + "," + reg1 + "," + reg2)
	}
	else {
		OUTPUT("SUB r11," + reg1 + "," + reg2)
		OUTPUT("STR r11," + p.first)
	}
}

void times(quaternion q) {
	//string reg1 = "$k0";
	string reg1 = "r11";
	if (is_con(q.a)) {
		//OUTPUT("li $k0," + q.a);
		OUTPUT("MOVW r11,#" + q.a)
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//string reg2 = "$k1";
	string reg2 = "r12";
	if (is_con(q.b)) {
		//OUTPUT("li $k1," + q.b);
		OUTPUT("MOVW r12,#" + q.b);
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r12," + p.first)
		}
	}

	auto p = get_var_addr(q.re);
	if (p.second != -1) {
		//OUTPUT("mul $k0," + reg1 + "," + reg2)
		//OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		OUTPUT("UMULL r11,r12," + reg1 + "," + reg2)
		OUTPUT("STR r11,[SP,#" + to_string(p.second - sp) + "]")
	}
	else if (p.first[0] == '$') {
		//OUTPUT("mul " + p.first + "," + reg1 + "," + reg2)
		OUTPUT("UMULL " + regMap(p.first) + ",r12," + reg1 + "," + reg2)
	}
	else {
		//OUTPUT("mul $k0," + reg1 + "," + reg2)
		//OUTPUT("sw $k0," + p.first)
		OUTPUT("UMULL r11,r12," + reg1 + "," + reg2)
		OUTPUT("STR r11," + p.first)
	}
}

void divide(quaternion q) {
	//string reg1 = "$k0";
	string reg1 = "r11";
	if (is_con(q.a)) {
		//OUTPUT("li $k0," + q.a);
		OUTPUT("MOVW r11,#" + q.a);
	}
	else {
		auto p = get_var_addr(q.a);
		if (p.second != -1) {
			//OUTPUT("lw $k0," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r11,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg1 = p.first;
			reg1 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k0," + p.first)
			OUTPUT("LDR r11," + p.first)
		}
	}
	//string reg2 = "$k1";
	string reg2 = "r12";
	if (is_con(q.b)) {
		//OUTPUT("li $k1," + q.b);
		OUTPUT("MOVW r12,#" + q.b)
	}
	else {
		auto p = get_var_addr(q.b);
		if (p.second != -1) {
			//OUTPUT("lw $k1," + to_string(p.second - sp) + "($sp)")
			OUTPUT("LDR r12,[SP,#" + to_string(p.second - sp) + "]")
		}
		else if (p.first[0] == '$') {
			//reg2 = p.first;
			reg2 = regMap(p.first);
		}
		else {
			//OUTPUT("lw $k1," + p.first)
			OUTPUT("LDR r12," + p.first)
		}
	}
	auto p = get_var_addr(q.re);
	if (p.second != -1) {
		//OUTPUT("div " + reg1 + "," + reg2)
		//OUTPUT("mflo $k0")
		//OUTPUT("sw $k0," + to_string(p.second - sp) + "($sp)")
		OUTPUT("SDIV r11," + reg1 + "," + reg2)
		OUTPUT("STR r11,[SP,#" + to_string(p.second - sp) + "]")
	}
	else if (p.first[0] == '$') {
		//OUTPUT("div " + reg1 + "," + reg2)
		//OUTPUT("mflo " + p.first)
		OUTPUT("SDIV " + regMap(p.first) + "," + reg1 + "," + reg2)
	}
	else {
		//OUTPUT("div " + reg1 + "," + reg2)
		//OUTPUT("mflo $k0")
		//OUTPUT("sw $k0," + p.first)
		OUTPUT("SDIV r11," + reg1 + "," + reg2)
		OUTPUT("STR r11," + p.first)
	}
}

void brack(quaternion q) {
	auto pre = get_var_addr(q.re);
	//re label  a con		sw b label+con
	//re label	a off		avalue->$ sw b label($)  
	//re label  a $					  sw b label($)
	//re label  a label		avalue->$ sw b label($)
	//re off	a con		sw b off+con($sp)
	//re off	a off		avalue->$ + $sp sw off($)
	//re off	a off				$ + $sp sw off($)
	//re off	a label		avalue->$ + $sp sw off($)
	if (pre.second != -1) {
		auto pa = get_var_addr(q.a);
		if (pa.second != -1) {
			if (is_con(q.b)) {
				string boff = to_string(stoi(q.b) * 4);
				int off = pa.second - sp + stoi(q.b) * 4;
				//OUTPUT("lw $k0," + to_string(off) + "($sp)")
				//OUTPUT("sw $k0," + to_string(pre.second - sp) + "($sp)")
				OUTPUT("LDR r11,[SP,#" + to_string(off) + "]")
				OUTPUT("STR r12,[SP,#" + to_string(pre.second - sp) + "]")
			}
			else {
				auto pb = get_var_addr(q.b);
				//string reg = "$k0";
				string reg = "r11";
				if (pb.second != -1) {
					//OUTPUT("lw $k0," + to_string(pb.second - sp) + "($sp)")
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11,[SP,#" + to_string(pb.second - sp) + "]")
					OUTPUT("LSL r11,r11,#2")
				}
				else if (pb.first[0] == '$') {
					//OUTPUT("sll $k0," + pb.first + ",2")
					OUTPUT("LSL r11," + regMap(pb.first) + ",#2")
				}
				else {
					//OUTPUT("lw $k0," + pb.first)
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11," + pb.first)
					OUTPUT("LSL r11,r11,#2")
				}
				//OUTPUT("addu $k0,$k0,$sp");
				//OUTPUT("lw $k0," + to_string(pa.second - sp) + "$k0")
				//OUTPUT("sw $k0," + to_string(pre.second - sp) + "($sp)")
				OUTPUT("ADD r11,r11,SP");
				OUTPUT("LDR r11,[r11,#" + to_string(pa.second - sp) + "]")
				OUTPUT("STR r11,[r11,#" + to_string(pre.second - sp) + "]")
			}
		}
		else {
			if (is_con(q.b)) {
				string boff = to_string(stoi(q.b) * 4);
				//OUTPUT("lw $k0," + pa.first + "+" + boff);
				//OUTPUT("sw $k0," + to_string(pre.second - sp) + "($sp)")
				OUTPUT("LDR r11," + pa.first + "+" + boff);
				OUTPUT("STR r11,[SP,#" + to_string(pre.second - sp) + "]")
			}
			else {
				auto pb = get_var_addr(q.b);
				//string reg = "$k0";
				string reg = "r11";
				if (pb.second != -1) {
					//OUTPUT("lw $k0," + to_string(pb.second - sp) + "($sp)")
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11,[SP,#" + to_string(pb.second - sp) + "]")
					OUTPUT("LSL r11,r11,#2")
				}
				else if (pb.first[0] == '$') {
					//OUTPUT("sll $k0," + pb.first + ",2")
					OUTPUT("LSL r11," + regMap(pb.first) + ",#2")
				}
				else {
					//OUTPUT("lw $k0," + pb.first)
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11," + pb.first)
					OUTPUT("LSL r11,r11,#2")
				}
				//OUTPUT("lw $k0," + pa.first + "($k0)")
				//OUTPUT("sw $k0," + to_string(pre.second - sp) + "($sp)")
				OUTPUT("LDR r11,[r11,#" + pa.first + "]")
				OUTPUT("STR r11,[SP,#" + to_string(pre.second - sp) + "]")
			}
		}
	}
	else if (pre.first[0] == '$') {
		auto pa = get_var_addr(q.a);
		if (pa.second != -1) {
			if (is_con(q.b)) {
				string boff = to_string(stoi(q.b) * 4);
				int off = pa.second - sp + stoi(q.b) * 4;
				//OUTPUT("lw " + pre.first + "," + to_string(off) + "($sp)")
				OUTPUT("LDR " + regMap(pre.first) + ",[SP,#" + to_string(off) + "]")
			}
			else {
				auto pb = get_var_addr(q.b);
				//string reg = "$k0";
				string reg = "r11";
				if (pb.second != -1) {
					//OUTPUT("lw $k0," + to_string(pb.second - sp) + "($sp)")
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11,[SP,#" + to_string(pb.second - sp) + "]")
					OUTPUT("LSL r11,r11,#2")
				}
				else if (pb.first[0] == '$') {
					//OUTPUT("sll $k0," + pb.first + ",2")
					OUTPUT("LSL r11," + regMap(pb.first) + ",#2")
				}
				else {
					//OUTPUT("lw $k0," + pb.first)
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11," + pb.first)
					OUTPUT("LSL r11,r11,#2")
				}
				//OUTPUT("addu $k0,$k0,$sp");
				//OUTPUT("lw "+ pre.first + "," + to_string(pa.second - sp) + "($k0)")
				OUTPUT("ADD r11,r11,SP");
				OUTPUT("LDR " + regMap(pre.first) + ",[r11,#" + to_string(pa.second - sp) + "]")
			}
		}
		else {
			if (is_con(q.b)) {
				string boff = to_string(stoi(q.b) * 4);
				//OUTPUT("lw " + pre.first + "," + pa.first + "+" + boff)
				OUTPUT("LDR " + regMap(pre.first) + "," + pa.first + "+" + boff)
			}
			else {
				auto pb = get_var_addr(q.b);
				//string reg = "$k0";
				string reg = "r11";
				if (pb.second != -1) {
					//OUTPUT("lw $k0," + to_string(pb.second - sp) + "($sp)")
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11,[SP,#" + to_string(pb.second - sp) + "]")
					OUTPUT("LSL r11,r11,#2")
				}
				else if (pb.first[0] == '$') {
					//OUTPUT("sll $k0," + pb.first + ",2")
					OUTPUT("LSL r11," + regMap(pb.first) + ",#2")
				}
				else {
					//OUTPUT("lw $k0," + pb.first)
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11," + pb.first)
					OUTPUT("LSL r11,r11,#2")
				}
				//OUTPUT("lw " + pre.first + "," + pa.first + "($k0)")
				OUTPUT("LDR " + regMap(pre.first) + ",[r11,#" + pa.first + "]")
			}
		}
	}
	else {
		auto pa = get_var_addr(q.a);
		if (pa.second != -1) {
			if (is_con(q.b)) {
				string boff = to_string(stoi(q.b) * 4);
				int off = pa.second - sp + stoi(q.b) * 4;
				//OUTPUT("lw $k0," + to_string(off) + "($sp)")
				//OUTPUT("sw $k0," + pre.first)
				OUTPUT("LDR r11,[SP,#" + to_string(off) + "]")
				OUTPUT("STR r11," + pre.first)
			}
			else {
				auto pb = get_var_addr(q.b);
				//string reg = "$k0";
				string reg = "r11";
				if (pb.second != -1) {
					//OUTPUT("lw $k0," + to_string(pb.second - sp) + "($sp)")
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11,[SP,#" + to_string(pb.second - sp) + "]")
					OUTPUT("LSL r11,r11,#2")
				}
				else if (pb.first[0] == '$') {
					//OUTPUT("sll $k0," + pb.first + ",2")
					OUTPUT("LSL r11," + regMap(pb.first) + ",#2")
				}
				else {
					//OUTPUT("lw $k0," + pb.first)
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11," + pb.first)
					OUTPUT("LSL r11,r11,#2")
				}
				//OUTPUT("addu $k0,$k0,$sp");
				//OUTPUT("lw $k0," + to_string(pa.second - sp) + "($k0)")
				//OUTPUT("sw $k0," + pre.first)
				OUTPUT("ADD r11,r11,SP");
				OUTPUT("LDR r11,[r11,#" + to_string(pa.second - sp) + "]")
				OUTPUT("STR r11," + pre.first)
			}
		}
		else {
			if (is_con(q.b)) {
				string boff = to_string(stoi(q.b) * 4);
				//OUTPUT("lw $k0," + pa.first + "+" + boff);
				//OUTPUT("sw $k0," + pre.first)
				OUTPUT("LDR r11," + pa.first + "+" + boff);
				OUTPUT("STR r11," + pre.first)
			}
			else {
				auto pb = get_var_addr(q.b);
				//string reg = "$k0";
				string reg = "r11";
				if (pb.second != -1) {
					//OUTPUT("lw $k0," + to_string(pb.second - sp) + "($sp)")
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11,[SP,#" + to_string(pb.second - sp) + "]")
					OUTPUT("LSL r11,r11,#2")
				}
				else if (pb.first[0] == '$') {
					//OUTPUT("sll $k0," + pb.first + ",2")
					OUTPUT("LSL r11," + regMap(pb.first) + ",#2")
				}
				else {
					//OUTPUT("lw $k0," + pb.first)
					//OUTPUT("sll $k0,$k0,2")
					OUTPUT("LDR r11," + pb.first)
					OUTPUT("LSL r11,r11,#2")
				}
				//OUTPUT("lw $k0," + pa.first + "($k0)")
				//OUTPUT("sw $k0," + pre.first)
				OUTPUT("LDR r11,[r11,#" + pa.first + "]")
				OUTPUT("STR r11," + pre.first)
			}
		}
	}
}

void optimized_mips_generate()
{
	resetPointer();
	OUTPUT(".data\n int:.asciiz \"\\n\""); // 回车，命名为int是防止与全局变量重复
	OUTPUT(".data\n char:.asciiz \"\\n\""); //为了对齐
	OUTPUT(".text\n");
	OUTPUT("ENTRY\n");
	for (int i = 0; i < optimized_quaternions.size(); i++) {
		quaternion q = optimized_quaternions[i];
		switch (q.op)
		{
		case CONST2TABLE:
			const2table(q);
			break;
		case VAR2TABLE:
			var2table(q);
			break;
		case ARR2TABLE:
			arr2table(q);
			break;
		case FUNC2TABLE:
			func2table(q);
			break;
		case PUSH:
			push(q);
			break;
		case CALL:
			call(q);
			break;
		case RET:
			ret(q);
			break;
		case GETRET:
			getret(q);
			break;
		case PRINT:
			print(q);
			break;
		case EXIT:
			exit(q);
			break;
		case SCANF:
			scanf(q);
			break;
		case LABEL:
			label(q);
			break;
		case GOTO:
			j(q);
			break;
		case BEQ:
			beq(q);
			break;
		case BGEZ:
			bgez(q);
			break;
		case BGTZ:
			bgtz(q);
			break;
		case BNE:
			bne(q);
			break;
		case BLTZ:
			bltz(q);
			break;
		case BLEZ:
			blez(q);
			break;
		case SLT:
			slt(q);
			break;
		case ASSIGNOP:
			assignop(q);
			break;
		case ARRASSIGN:
			arrassign(q);
			break;
		case ADD:
			add(q);
			break;
		case SUB:
			sub(q);
			break;
		case TIMES:
			times(q);
			break;
		case DIVIDE:
			divide(q);
			break;
		case BRACK:
			brack(q);
			break;
		case STORE:
			store(q,i);
			break;
		case RECOVER:
			recover(q);
			break;
		default:
			break;
		}
	}
}
