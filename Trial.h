#pragma once
#include"BPlusTree.h"
#include<algorithm>
class Trial
{
public:
	Trial(BPlusTree &bp);
	void Manual() {
		char op;
		while (std::cin >> op) {
			switch (op)
			{
			case 'i': {
				
				int data;
				std::cin >> data;
				std::cout << "插入:" <<data<< std::endl;
				bp_->Insert(data);
				break;
			}
			case 'd': {
				int data;
				std::cin >> data;
				std::cout << "删除:" << data << std::endl;
				bp_->Delete(data);
				break;
			}
			}
			bp_->Print();
		}
	}
	void Artificial(const int frequency) {
		std::vector<int> vec;
		srand((unsigned)time(NULL));
		for (int i = 0; i < frequency; i++)
			vec.push_back(i + 1);
		std::random_shuffle(vec.begin(), vec.end());
		for (int i = 0; i < frequency; i++)
			bp_->Insert(vec[i]);
		std::cout << frequency << "次随机插入操作" << std::endl;
		bp_->Print();
		std::random_shuffle(vec.begin(), vec.end());
		for (int i = 0; i < frequency; i++)
			bp_->Delete(vec[i]);
		std::cout << frequency << "次随机删除操作" << std::endl;
		bp_->Print();
		std::cout << std::endl;
	}
	~Trial();

private:
	BPlusTree* bp_;
};

Trial::Trial(BPlusTree &bp)
{
	bp_ = &bp;
}

Trial::~Trial()
{
}