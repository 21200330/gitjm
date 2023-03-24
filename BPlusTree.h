#pragma once
#include <vector>
#include <cmath>
#include<iostream>


const int ORDER = 7; //��b+����֧�ֵ�3�׼������ϵ�b+����1��2�ײ����ǣ�����û���������������
const int MAX_M = ORDER; // �ڵ����Ԫ�ظ���
const int MIN_M = ceil(ORDER / 2.0); // �ڵ�����Ԫ�ظ���

enum OpMode {//����ģʽ������dosearchû��ʵ��
	doinsert,doremove,dosearch
};

class BPlusTreeNode {
public:
	bool is_leaf_; // �Ƿ���Ҷ�ӽڵ�
	int num_keys_; // �ڵ��д洢��Ԫ������
	std::vector<int> keys_; // �洢Ԫ�ص�����
	std::vector<BPlusTreeNode*> children_; // �洢�ӽڵ������
	BPlusTreeNode* parent_;//���ڵ�
	BPlusTreeNode* next;

	// ���캯��
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
	void Split(BPlusTreeNode* node);//��һ�����ڵ���ѳ������ڵ㡣
	void Merge(BPlusTreeNode* node ,BPlusTreeNode* other_node);//��һ���ڵ��������ڵĽڵ�ϲ���
	void SubtituteWord(BPlusTreeNode* node, int sub, int key);//ɾ��ʱ�������һ���ؼ��ʣ�����Ҫ�ݹ���ĸ��ڵ�ؼ���
	void AdjustDelete(BPlusTreeNode* node);//ɾ��Ԫ�غ�Ԫ�ظ�������MIN_M�����е���
	BPlusTreeNode* FindLeftSibling(BPlusTreeNode* node);//����һ���ڵ��������ڽڵ㡣
	BPlusTreeNode* FindRightSibling(BPlusTreeNode* node);//����һ���ڵ���Ҳ����ڽڵ㡣
	void PrintNode(BPlusTreeNode* node);//���ӻ���ʽ��ӡһ���ڵ�Ľṹ��
};

