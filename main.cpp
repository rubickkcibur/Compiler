// homework2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "heads.h"
#include "type.h"
#include "gtype.h"
#include "error.h"
#include "tempCode.h"
#include "mips.h"
#include "optimize.h"

vector<type> types;
vector<gtype> gtypes;
ofstream fdebug("debug.txt");
ofstream ferr("error.txt");

void outputQuas(vector<quaternion>& list, ofstream& fout) {
	for (quaternion& q : list) {
		auto op = q.op;
		switch (op)
		{
		case CONST2TABLE:
			break;
		case VAR2TABLE:
			break;
		case ARR2TABLE:
			break;
		case FUNC2TABLE: {
			fout << q.re << " " << q.a << "\n";
			resetPointer();
			tableEntry t = (*mapPointer)[q.a];
			for (int i = 0; i < t.paraNames.size(); i++) {
				fout << "para" << " " << dataType2string(t.v[i]) << " " << t.paraNames[i] << "\n";
			}
		}
			break;
		case PUSH:
			fout << "push" << " " << q.a << "\n";
			break;
		case CALL:
			fout << "call" << " " << q.a << "\n";
			break;
		case RET:
			fout << "ret" << " " << q.re << "\n";
			break;
		case GETRET:
			fout << q.re << "= RET\n";
			break;
		case LABEL:
			fout << q.re << ":\n";
			break;
		case BEQ:
			fout << "beq" << q.a << " " << q.b << " " << q.re << "\n";
			break;
		case BNE:
			fout << "bne" << q.a << " " << q.b << " " << q.re << "\n";
			break;
		case SLT:
			fout << q.re << "=" << q.a << "<" << q.b << "\n";
			break;
		case ASSIGNOP:
			fout << q.re << "=" << q.a << "\n";
			break;
		case ARRASSIGN:
			fout << q.re << "[" << q.a << "] = " << q.b << "\n";
			break;
		case ADD:
			fout << q.re << "=" << q.a << "+" << q.b << "\n";
			break;
		case SUB:
			fout << q.re << "=" << q.a << "-" << q.b << "\n";
			break;
		case TIMES:
			fout << q.re << "=" << q.a << "*" << q.b << "\n";
			break;
		case DIVIDE:
			fout << q.re << "=" << q.a << "\\" << q.b << "\n";
			break;
		case BRACK:
			fout << q.re << "=" << q.a << "[" << q.b << "]\n";
			break;
		case PRINT:
			fout << "printf " << q.a << "," << q.b << "\n";
			break;
		case SCANF:
			fout << "scanf" << " " << q.a << "\n";
			break;
		case GOTO:
			fout << "goto " << q.re << "\n";
		case EXIT:	
		case BGEZ:
		case BGTZ:
		case BLTZ:
		case BLEZ:
		case STORE:
		case RECOVER:
		case FUNCEND:
		case NOP:
		default:
			break;
		}
	}
}

void gtypes_output(ofstream fout) {
	for (int i = 0, j = 0; i <= types.size(); i++) {
		bool flag = false;
		while (j < gtypes.size() && i == gtypes[j].pos) {
			fout << gtypes[j].value << "\n";
			j++;
		}
		if (i >= types.size())
			break;
		fout << typeCode2String(types[i].code)
			<< " " << types[i].value << "\n";
	}
}

void quaternions_output(ofstream& fdebug){
	for (int i = 0; i < quaternions.size(); i++)
		fdebug << quaternion2string(quaternions[i]);
	fdebug << "----------------\n";
	for (int i = 0; i < optimized_quaternions.size(); i++)
		fdebug << quaternion2string(optimized_quaternions[i]);
}

void funcBlock_output(ofstream& fout) {
	for (funcBlock& fb : funcBlocks) {
		/*for (auto p : fb.bbrelation) {
			for (int i : p.first)
				fout << to_string(i) << " ";
			fout << "\n";
			for (int i : p.second)
				fout << to_string(i) << " ";
			fout << "\n";
			fout << "-----block-----\n";
		}*/
		fout << fb.name << "\n";
		for (auto p : fb.v2r) {
			fout << p.first << " " << p.second << "\n";
		}
		for (auto p : fb.pvs) {
			fout << p << " ";
		}
		//int cnt = 0;
		for (int i = 0; i < fb.bas.size(); i++) {
			basicBlock& bb = fb.bas[i];
			/*for (string s : fb.dfa.use[i])
				fout << s << " ";
			fout << "\n";
			for (string s : fb.dfa.def[i])
				fout << s << " ";
			fout << "\n";
			for (string s : fb.dfa.in[i])
				fout << s << " ";
			fout << "\n";
			for (string s : fb.dfa.out[i])
				fout << s << " ";
			fout << "\n";
			fout << "-------one block-------\n";*/
			//fout << to_string(cnt++) << "\n";
			for (quaternion& q : bb.quas) {
				fout << quaternion2string(q);
			}
			fout << "\n";
		}
		fout << "\n";
	}
}

int main()
{
	ifstream fin("testfile.txt");
	ofstream fout("output.txt");
	ofstream fm("mips.txt");
	ofstream fqbefore("17373488_姜春阳_优化前中间代码.txt");
	ofstream fqafter("17373488_姜春阳_优化后中间代码.txt");
	errorInit();
	wordAnalyze(fin);
	grammarAnalyze();
	optimize();
	optimized_mips_generate();
	//mips_generate();
	//quaternions_output(fdebug);
	//funcBlock_output(fout);
	fm << ".text\n";
	for (string s : outputBuffer) {
		fm << s << "\n";
	}
	//outputQuas(quaternions, fqbefore);
	//outputQuas(optimized_quaternions, fqafter);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
