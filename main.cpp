#include"BPlusTree.h"
#include<algorithm>
#include"Trial.h"
int main() {
	BPlusTree bp;//����һ��b+��
	Trial t(bp);//����һ������
	//t.Manual();//�ֶ�����
	//�淶���������룺i ��һ��int Ȼ��س�
	//			ɾ����d ��һ��int Ȼ��س�
	int fre = 50;
	t.Artificial(fre);//Ƶ��Ϊfre���Զ�����
	return 0;
}
