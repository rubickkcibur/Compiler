#include "optimize.h"

vector<quaternion> optimized_quaternions;
map<string, funcBlock> name2fb;
void regenerate_quaternions() {
	int i = 0;
	for (funcBlock& fb : funcBlocks) {
		name2fb[fb.name] = fb;
		int start = fb.start;
		int end = fb.end;
		while (i <= start) {
			if (quaternions[i].op != NOP)
				optimized_quaternions.push_back(quaternions[i]);
			i++;
		}
		for (basicBlock& bb : fb.bas) {
			for (quaternion q : bb.quas) {
				if (q.op != NOP)
					optimized_quaternions.push_back(q);
			}
		}
		optimized_quaternions.push_back(quaternions[end]);
		i = end + 1;
	}
}