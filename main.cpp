#include"BPlusTree.h"
#include<algorithm>
#include"Trial.h"
int main() {
	BPlusTree bp;//创建一个b+树
	Trial t(bp);//创建一个测试
	//t.Manual();//手动测试
	//规范操作：插入：i 跟一个int 然后回车
	//			删除：d 跟一个int 然后回车
	int fre = 50;
	t.Artificial(fre);//频数为fre的自动测试
	return 0;
}
