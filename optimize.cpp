#include "optimize.h"
//constReplace->blockDivide->setRelation->build_dfa->allocate_s->allocate_t->allocate_a
void optimize() {
	constReplace();
	blockDivide();
	funcInsert();//��ɾ�����ǵ��޸�t�Ĵ������䲿��
	setRelation();
	constPropagate();
	assign_optimize();
	build_dfa();
	allocate_global_register();
	allocate_local_register();
	allocate_para_register();
	regenerate_quaternions();
	t_register_scale();
}