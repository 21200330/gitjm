#pragma once
#include <vector>
#include <cmath>
#include<iostream>


const int ORDER = 7; //阶b+树，支持的3阶及其以上的b+树，1、2阶不考虑，而且没有完善这样的情况
const int MAX_M = ORDER; // 节点最多元素个数
const int MIN_M = ceil(ORDER / 2.0); // 节点最少元素个数

enum OpMode {//操作模式，这里dosearch没有实现
	doinsert,doremove,dosearch
};

class BPlusTreeNode {
public:
	bool is_leaf_; // 是否是叶子节点
	int num_keys_; // 节点中存储的元素数量
	std::vector<int> keys_; // 存储元素的数组
	std::vector<BPlusTreeNode*> children_; // 存储子节点的数组
	BPlusTreeNode* parent_;//父节点
	BPlusTreeNode* next;

	// 构造函数
	BPlusTreeNode(bool leaf = false, BPlusTreeNode* parent = nullptr) {
		num_keys_ = 0;
		next = nullptr;
		is_leaf_ = leaf;
		parent_ = parent;
		keys_.reserve(MAX_M + 2);
		children_.reserve(MAX_M + 2);
	}

	void AddKey(int key);
};

class BPlusTree
{
public:
	BPlusTree() {
		root_ = nullptr;
	}
	void Insert(int key);
	void Delete(int key);
	BPlusTreeNode* Search(int key,OpMode om);
	void Print();

private:
	BPlusTreeNode* root_;
	void Split(BPlusTreeNode* node);//将一个满节点分裂成两个节点。
	void Merge(BPlusTreeNode* node ,BPlusTreeNode* other_node);//将一个节点与其相邻的节点合并。
	void SubtituteWord(BPlusTreeNode* node, int sub, int key);//删除时若是最后一个关键词，则需要递归更改父节点关键词
	void AdjustDelete(BPlusTreeNode* node);//删除元素后，元素个数少于MIN_M，进行调整
	BPlusTreeNode* FindLeftSibling(BPlusTreeNode* node);//查找一个节点的左侧相邻节点。
	BPlusTreeNode* FindRightSibling(BPlusTreeNode* node);//查找一个节点的右侧相邻节点。
	void PrintNode(BPlusTreeNode* node);//可视化方式打印一个节点的结构。
};

