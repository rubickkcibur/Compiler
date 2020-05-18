//
//#include "mips.h"
//#include "tempCode.h"
//#include "symbolTable.h"
//#include "heads.h"
//#define OUTPUT(s) outputBuffer.push_back(s);
//
//vector<string> outputBuffer;
//map<string, int> var2addr;
//int sp = 0x7fffeffc;
//int orisp;
//vector<int> store_sp;
//
//string chulifanxiegang(string s) {
//	for (int i = 0; i < s.size(); i++) {
//		if (s[i] == '\\') {
//			s.insert(i, "\\");
//			i++;
//		}
//	}
//	return s;
//}
//
//string getNewSname() {
//	static int i = 1;
//	return "string" + to_string(i++);
//}
//
//void const2table(quaternion q)
//{
//	if (mapPointer == &globalMap) {
//		OUTPUT(".data\n" + q.a + ": .word " + q.b)
//		OUTPUT(".text\n");
//	}
//	else {
//		OUTPUT("li $t8," + q.b);
//		int addr = var2addr[q.a];
//		int off = (addr - sp);
//		OUTPUT("sw $t8,"+to_string(off)+"($sp)");
//	}
//}
//
//void var2table(quaternion q) 
//{
//	if (mapPointer == &globalMap) {
//		OUTPUT(".data\n" + q.a + ":.space 4");
//		OUTPUT(".text\n");
//	}
//	else {
//		
//	}
//}
//
//void arr2table(quaternion q) 
//{
//	int length = stoi(q.b);
//	if (mapPointer == &globalMap) {
//		OUTPUT(".data\n" + q.a + ":.space " + to_string(length * 4));
//		OUTPUT(".text\n");
//	}
//	else {
//	}
//}
//
//void func2table(int i) {
//	quaternion q = quaternions[i];
//	resetPointer();
//	tableEntry te = (*mapPointer)[q.a];
//	vector<string> names = te.paraNames;
//	movePointer(q.a);
//	var2addr.clear();
//	orisp = sp;
//	for (string para : names) {
//		sp -= 4;
//		var2addr[para] = sp;
//	}
//	int sp_without_paras = sp;
//	bool flag = true;
//	for (int j = i+1;flag && j < quaternions.size(); j++) {
//		quaternion q = quaternions[j];
//		Operation op = quaternions[j].op;
//		switch (op)
//		{
//		case CONST2TABLE:
//			sp -= 4;
//			var2addr[q.a] = sp;
//			break;
//		case VAR2TABLE:
//			sp -= 4;
//			var2addr[q.a] = sp;
//			break;
//		case ARR2TABLE: {
//			int length = stoi(q.b);
//			sp -= length * 4;
//			var2addr[q.a] = sp;
//		}
//			break;
//		case FUNC2TABLE:
//			break;
//		case PUSH:
//			break;
//		case CALL:
//			break;
//		case RET:
//			break;
//		case GETRET:
//			if ((var2addr.find(q.re) == var2addr.end())
//				&& (q.re[0] == '#')) {
//				sp -= 4;
//				var2addr[q.re] = sp;
//			}
//			break;
//		case PRINT:
//			break;
//		case EXIT:
//			break;
//		case SCANF:
//			break;
//		case LABEL:
//			break;
//		case GOTO:
//			break;
//		case BEQ:
//			break;
//		case BGEZ:
//			break;
//		case BGTZ:
//			break;
//		case BNE:
//			break;
//		case BLTZ:
//			break;
//		case BLEZ:
//			break;
//		case ARRASSIGN:
//			break;
//		case SLT:
//		case ASSIGNOP:
//		case ADD:
//		case SUB:
//		case TIMES:
//		case DIVIDE:
//		case BRACK:
//			if ((var2addr.find(q.re) == var2addr.end())
//				&& (q.re[0] == '#')) {
//				sp -= 4;
//				var2addr[q.re] = sp;
//			}
//			break;
//		case STORE:
//			break;
//		case RECOVER:
//			break;
//		case FUNCEND:
//			flag = false;
//			break;
//		default:
//			break;
//		}
//	}
//
//	OUTPUT(".text\n");
//	OUTPUT("addu $sp,$sp," + to_string(sp - sp_without_paras));
//}
//
//void store(quaternion q) {
//	OUTPUT("addu $sp,$sp,-4");
//	sp -= 4;
//	store_sp.push_back(sp);
//	OUTPUT("sw $ra,0($sp)");
//}
//
//void recover(quaternion q) {
//	OUTPUT("lw $ra,0($sp)");
//	sp = store_sp.back();
//	store_sp.pop_back();
//	sp += 4;
//	OUTPUT("addu $sp,$sp,4");
//}
//
//void push(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//		OUTPUT("addu $sp,$sp,-4");
//		sp -= 4;
//		OUTPUT("sw $t8,0($sp)");
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//			OUTPUT("addu $sp,$sp,-4");
//			sp -= 4;
//			OUTPUT("sw $t8,0($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//			OUTPUT("addu $sp,$sp,-4");
//			sp -= 4;
//			OUTPUT("sw $t8,0($sp)");
//		}
//	}
//}
//
//void call(quaternion q) {
//	OUTPUT("jal " + q.a);
//}
//
//void ret(quaternion q) {
//	int off = orisp - sp;
//	if (isdigit(q.re[0]) || q.re[0] == '\'' || q.re[0] == '-' || q.re[0] == '+') {
//		OUTPUT("li $v0," + q.re);
//	}
//	else if (q.re != ""){
//		if (var2addr.find(q.re) != var2addr.end()) {
//			int addr = var2addr[q.re];
//			int off = (addr - sp);
//			OUTPUT("lw $v0," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.re);
//			OUTPUT("lw $v0,0($t8)");
//		}
//	}
//	OUTPUT("addu,$sp,$sp," + to_string(off));
//	OUTPUT("jr $ra");
//}
//
//void getret(quaternion q) {
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $v0," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#'){
//		OUTPUT("la $t8," + q.re);
//		OUTPUT("sw $v0,0($t8)");
//	}
//	else {
//	}
//}
//
//void print(quaternion q) {
//	OUTPUT(".data\n");
//	string sname = getNewSname();
//	OUTPUT(sname + ":.asciiz \"" + chulifanxiegang(q.a) + "\"");
//	OUTPUT(".text\n");
//	OUTPUT("la $a0," + sname);
//	OUTPUT("li $a1," + to_string(q.a.size()));
//	OUTPUT("li $v0,4");
//	OUTPUT("syscall");
//	if (q.re != "void") {
//		if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//			OUTPUT("li $a0," + q.b);
//		}
//		else {
//			if (var2addr.find(q.b) != var2addr.end()) {
//				int addr = var2addr[q.b];
//				int off = (addr - sp);
//				OUTPUT("lw $a0," + to_string(off) + "($sp)");
//			}
//			else {
//				OUTPUT("la $t8," + q.b);
//				OUTPUT("lw $a0,0($t8)");
//			}
//		}
//		if (q.re == "int") {
//			OUTPUT("li $v0,1");
//		}
//		else if (q.re == "char") {
//			OUTPUT("li $v0,11");
//		}
//		OUTPUT("syscall");
//	}
//	OUTPUT("la $a0,int");
//	OUTPUT("li $v0,4");
//	OUTPUT("syscall");
//}
//
//void exit(quaternion q) {
//	OUTPUT("li $v0,10");
//	OUTPUT("syscall");
//}
//
//void scanf(quaternion q) {
//	if (q.a == "int") {
//		OUTPUT("li $v0,5")
//	}
//	else if (q.a == "char") {
//		OUTPUT("li $v0,12")
//	}
//	OUTPUT("syscall");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $v0," + to_string(off) + "($sp)");
//	}
//	else {
//		OUTPUT("la $t8," + q.re);
//		OUTPUT("sw $v0,0($t8)");
//	}
//}
//
//void label(quaternion q) {
//	OUTPUT(q.re + ":");
//}
//
//void j(quaternion q) {
//	OUTPUT("j " + q.re);
//}
//
//void beq(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("beq $t8,$t9," + q.re);
//}
//
//void bne(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("bne $t8,$t9," + q.re);
//}
//
//void bgez(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	OUTPUT("bgez $t8," + q.re);
//}
//
//void bgtz(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	OUTPUT("bgtz $t8," + q.re);
//}
//
//void bltz(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	OUTPUT("bltz $t8," + q.re);
//}
//
//void blez(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	OUTPUT("blez $t8," + q.re);
//}
//
//void slt(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("slt $t8,$t8,$t9");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void assignop(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void arrassign(quaternion q) {
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("addu $t8,$sp," + to_string(off));
//	}
//	else {
//		OUTPUT("la $t8," + q.re);
//	}
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t9," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.a);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("sll $t9,$t9,2");
//	OUTPUT("add $t8,$t8,$t9");
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("sw ,$t9,0($t8)");
//}
//
//void add(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("add $t8,$t8,$t9");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void sub(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' ||q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("sub $t8,$t8,$t9");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void times(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("mul $t8,$t8,$t9");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void divide(quaternion q) {
//	if (isdigit(q.a[0]) || q.a[0] == '\'' || q.a[0] == '-' || q.a[0] == '+') {
//		OUTPUT("li $t8," + q.a);
//	}
//	else {
//		if (var2addr.find(q.a) != var2addr.end()) {
//			int addr = var2addr[q.a];
//			int off = (addr - sp);
//			OUTPUT("lw $t8," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t8," + q.a);
//			OUTPUT("lw $t8,0($t8)");
//		}
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("div $t8,$t9");
//	OUTPUT("mflo $t8");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void brack(quaternion q) {
//	if (var2addr.find(q.a) != var2addr.end()) {
//		int addr = var2addr[q.a];
//		int off = (addr - sp);
//		OUTPUT("addu $t8,$sp," + to_string(off));
//	}
//	else {
//		OUTPUT("la $t8," + q.a);
//	}
//	if (isdigit(q.b[0]) || q.b[0] == '\'' || q.b[0] == '-' || q.b[0] == '+') {
//		OUTPUT("li $t9," + q.b);
//	}
//	else {
//		if (var2addr.find(q.b) != var2addr.end()) {
//			int addr = var2addr[q.b];
//			int off = (addr - sp);
//			OUTPUT("lw $t9," + to_string(off) + "($sp)");
//		}
//		else {
//			OUTPUT("la $t9," + q.b);
//			OUTPUT("lw $t9,0($t9)");
//		}
//	}
//	OUTPUT("sll $t9,$t9,2");
//	OUTPUT("add $t8,$t8,$t9");
//	OUTPUT("lw $t8,0($t8)");
//	if (var2addr.find(q.re) != var2addr.end()) {
//		int addr = var2addr[q.re];
//		int off = (addr - sp);
//		OUTPUT("sw $t8," + to_string(off) + "($sp)");
//	}
//	else if (q.re[0] != '#') {
//		OUTPUT("la $t9," + q.re);
//		OUTPUT("sw $t8,0($t9)");
//	}
//	else {
//	}
//}
//
//void mips_generate()
//{
//	resetPointer();
//	OUTPUT(".data\n int:.asciiz \"\\n\""); // 回车，命名为int是防止与全局变量重复
//	OUTPUT(".data\n char:.asciiz \"\\n\""); //为了对齐
//	OUTPUT(".text\n");
//	for (int i = 0; i < quaternions.size();i++) {
//		quaternion q = quaternions[i];
//		switch (q.op)
//		{
//		case CONST2TABLE:
//			const2table(q);
//			break;
//		case VAR2TABLE:
//			var2table(q);
//			break;
//		case ARR2TABLE:
//			arr2table(q);
//			break;
//		case FUNC2TABLE:
//			func2table(i);
//			break;
//		case PUSH:
//			push(q);
//			break;
//		case CALL:
//			call(q);
//			break;
//		case RET:
//			ret(q);
//			break;
//		case GETRET:
//			getret(q);
//			break;
//		case PRINT:
//			print(q);
//			break;
//		case EXIT:
//			exit(q);
//			break;
//		case SCANF:
//			scanf(q);
//			break;
//		case LABEL:
//			label(q);
//			break;
//		case GOTO:
//			j(q);
//			break;
//		case BEQ:
//			beq(q);
//			break;
//		case BGEZ:
//			bgez(q);
//			break;
//		case BGTZ:
//			bgtz(q);
//			break;
//		case BNE:
//			bne(q);
//			break;
//		case BLTZ:
//			bltz(q);
//			break;
//		case BLEZ:
//			blez(q);
//			break;
//		case SLT:
//			slt(q);
//			break;
//		case ASSIGNOP:
//			assignop(q);
//			break;
//		case ARRASSIGN:
//			arrassign(q);
//			break;
//		case ADD:
//			add(q);
//			break;
//		case SUB:
//			sub(q);
//			break;
//		case TIMES:
//			times(q);
//			break;
//		case DIVIDE:
//			divide(q);
//			break;
//		case BRACK:
//			brack(q);
//			break;
//		case STORE:
//			store(q);
//			break;
//		case RECOVER:
//			recover(q);
//			break;
//		default:
//			break;
//		}
//	}
//}
