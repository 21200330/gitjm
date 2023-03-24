#include "BPlusTree.h"
#include <iostream>
#include <queue>
using namespace std;

void BPlusTree::Insert(int key) {
	if (root_ == nullptr) {
		root_ = new BPlusTreeNode(true);
		root_->AddKey(key);
		return;
	}
	//root_����,���в�ѯ
	BPlusTreeNode* node = Search(key, doinsert);
	for (auto i : node->keys_) {
		if (i == key) {
			cout << "�����ظ�" << endl;
			return;//���࣬����
		}
	}
	vector<int>::iterator it = node->keys_.begin();
	for (; it < node->keys_.end(); it++)
		if (*it > key) break;
	node->keys_.insert(it, key);
	if (++(node->num_keys_) > MAX_M)Split(node);
}

void BPlusTree::Delete(int key)
{
	BPlusTreeNode* node = Search(key, doremove);
	int flag = 0;
	//δ�ҵ��ýڵ㣬ɾ��ʧ��
	if (node == nullptr)flag = 0;
	else {
		for (auto i : node->keys_) {
			if (i == key)flag = 1;
		}
	}
	if (flag == 0) {
		cout << "δ�ҵ���Ԫ�أ�ɾ��ʧ��" << endl;
		return;
	}
	//�ҵ��ýڵ��Ӧ��Ҷ�ڵ�
	// һ�����ڵ�
	if (node->parent_ == nullptr && node->num_keys_ == 1) {
		delete node;
		root_ = nullptr;
		return;
	}
	//����Ǹ��ڵ�Ĺؼ���
	vector<int>::iterator it = find(node->keys_.begin(), node->keys_.end(), key);
	if (it == node->keys_.end() - 1) {
		int sub = node->keys_[node->num_keys_ - 2];
		if (node->parent_)SubtituteWord(node->parent_, sub, key);
	}
	node->keys_.erase(it);
	node->num_keys_--;
	if (node->parent_ != nullptr) {//�Ǹ��ڵ�
		if (node->num_keys_ < MIN_M)AdjustDelete(node);
	}
}

BPlusTreeNode* BPlusTree::Search(int key, OpMode om)
{
	BPlusTreeNode* node = root_;
	while (node != nullptr) {
		if (node->is_leaf_) {//Ҷ�ڵ�ֱ��Ѱ�Ҳ��ҷ��أ����������Ϊ�˲����Ƿ���ڣ����ǲ�ѯ����û��д
			for (int i = 0; i < node->num_keys_; i++) {
				if (node->keys_[i] == key)return node;
			}
			return node;//��������ɾ��ȷ��λ��
		}
		else {
			vector<int>::iterator i = node->keys_.begin();
			vector<int>::iterator iend = node->keys_.end();
			for (; i != node->keys_.end(); i++) {
				if (key <= *i) {
					int index = (int)(i - node->keys_.begin());
					node = node->children_[index];
					break;
				}
			}

			if (i == iend) {//��������������ֵ������Ҫ�޸���;��ֵ
				if (om == doremove || om == dosearch) {//ɾ���������ѯ�������ֵ����,˵��������,ֱ�ӷ��ؿ�
					return nullptr;
				}
				else {
					node->keys_[node->num_keys_ - 1] = key;
					node = node->children_[node->num_keys_ - 1];
				}

			}
		}
	}

	return node;
}

void BPlusTree::Split(BPlusTreeNode* node) {

	// ����һ���µĽڵ�new_node
	BPlusTreeNode* new_node = new BPlusTreeNode(node->is_leaf_);

	// ��ԭ�ڵ��һ���key��child�ֱ��Ƶ�new_node�У�������ԭ�ڵ��num_keys_
	int mid = node->num_keys_ / 2;
	for (int i = 0; i < mid; i++) {
		new_node->keys_.push_back(node->keys_[0]);
		node->keys_.erase(node->keys_.begin());
		if (node->is_leaf_ == false) {//��Ҷ�ڵ�
			new_node->children_.push_back(node->children_[0]);
			node->children_[0]->parent_ = new_node;
			node->children_.erase(node->children_.begin());
		}
	}
	new_node->num_keys_ = mid;
	node->num_keys_ = node->num_keys_ - mid;
	//���ڵ�
	if (node->parent_ == nullptr) {
		BPlusTreeNode* new_root = new BPlusTreeNode();
		new_root->AddKey(*(new_node->keys_.end() - 1));
		new_root->AddKey(*(node->keys_.end() - 1));
		new_root->children_.push_back(new_node);
		new_root->children_.push_back(node);
		new_node->parent_ = new_root;
		node->parent_ = new_root;
		new_node->next = node;

		root_ = new_root;
		return;
	}
	//�Ǹ��ڵ�
	BPlusTreeNode* node_parent = node->parent_;
	//itָ��ǰ�ڵ��Ӧ��parent��child
	vector<int>::iterator it = node_parent->keys_.begin();
	for (; it != node_parent->keys_.end(); it++) {
		if (*it == node->keys_[node->num_keys_ - 1])break;
	}
	new_node->parent_ = node->parent_;
	//Ҷ�ڵ�
	if (node->is_leaf_) {
		new_node->next = node;
		if ((it - node_parent->keys_.begin()) != 0) {//�����㲻�Ǳ�ͷ
			node_parent->children_[(it - node_parent->keys_.begin()) - 1]->next = new_node;
		}
	}
	//��Ӹ��ڵ�
	int index = (int)(it - node_parent->keys_.begin());//���������ֱ�ӷ�������б���Ӧ����û�а취��ʽת����������index���м������������������index��������
	node_parent->keys_.insert(it, new_node->keys_[new_node->num_keys_ - 1]);
	node_parent->children_.insert(node_parent->children_.begin() + index, new_node);
	if (++(node_parent->num_keys_) > MAX_M)Split(node_parent);
}

void BPlusTree::Merge(BPlusTreeNode* node, BPlusTreeNode* other_node)//node��ߣ�other_node�ұ�
{
	//�ϲ����ұߵĽڵ�
	BPlusTreeNode* parent = node->parent_;
	//ɾ����߶�Ӧ�ĸ��ڵ�ؼ���
	vector<int>::iterator it = find(parent->keys_.begin(), parent->keys_.end(), *(node->keys_.end() - 1));
	int index = (int)(it - parent->keys_.begin());
	it = parent->keys_.erase(it);
	parent->num_keys_--;
	//ɾ�����ڵ�ĺ���ָ��
	parent->children_.erase(find(parent->children_.begin(), parent->children_.end(), parent->children_[index]));
	//�ҵ�nodeǰһ���ڵ�
	if (node->is_leaf_)//Ҷ�ڵ�
		if (it != parent->keys_.begin()) {//��ǰ���ڵ�
			it--;
			index = (int)(it - parent->keys_.begin());
			BPlusTreeNode* pre_node = parent->children_[index];
			pre_node->next = other_node;
		}
	//�ϲ����ҽڵ�
	for (int i = node->num_keys_ - 1; i >= 0; i--) {
		other_node->keys_.insert(other_node->keys_.begin(), node->keys_[i]);
		if (!node->is_leaf_) {
			//��Ҷ�ڵ�
			other_node->children_.insert(other_node->children_.begin(), node->children_[i]);
			node->children_[i]->parent_ = other_node;
		}
		other_node->num_keys_++;
	}
	//�ͷ��ڴ�
	delete node;
	if (parent->num_keys_ < MIN_M)AdjustDelete(parent);
}


void BPlusTreeNode::AddKey(int key)
{
	keys_.push_back(key);
	num_keys_++;
}

BPlusTreeNode* BPlusTree::FindLeftSibling(BPlusTreeNode* node)
{
	if (node->parent_ == nullptr)return nullptr;
	vector<int>::iterator it = find(node->parent_->keys_.begin(), node->parent_->keys_.end(), node->keys_[node->num_keys_ - 1]);
	if (it == node->parent_->keys_.begin())return nullptr;
	int index = it - node->parent_->keys_.begin();
	return node->parent_->children_[index - 1];
}

BPlusTreeNode* BPlusTree::FindRightSibling(BPlusTreeNode* node)
{
	if (node->parent_ == nullptr)return nullptr;
	vector<int>::iterator it = find(node->parent_->keys_.begin(), node->parent_->keys_.end(), node->keys_[node->num_keys_ - 1]);
	if (it == node->parent_->keys_.end() - 1)return nullptr;
	int index = it - node->parent_->keys_.begin();
	return node->parent_->children_[index + 1];
}

void BPlusTree::PrintNode(BPlusTreeNode* node) {
	cout << "{  ";
	if (node->is_leaf_)cout << "L:";
	else cout << "N:";
	vector<int>::iterator it = node->keys_.begin();
	for (; it != node->keys_.end(); it++) {
		cout << *it << "  ";
	}
	cout << "}";
}

void BPlusTree::Print() {
	if (root_ == nullptr) {
		cout << "\tEmpty B+Tree\t" << endl;
		return;
	}
	queue<BPlusTreeNode*> q;
	q.push(root_);
	q.push(nullptr);
	while (!q.empty()) {
		BPlusTreeNode* r = q.front();
		q.pop();
		if (r == nullptr) {
			cout << endl;
			if (q.empty())return;
			else {
				q.push(nullptr);
				continue;
			}
		}

		PrintNode(r);
		//if(r->parent_){
		//	cout << "->p:";
		//	PrintNode(r->parent_);
		//}


		if (!r->is_leaf_)
		{
			vector<BPlusTreeNode*>::iterator it;
			for (it = r->children_.begin(); it != r->children_.end(); it++) {
				q.push(*it);
			}
		}
	}
}

void BPlusTree::SubtituteWord(BPlusTreeNode* node, int sub, int key)
{
	vector<int>::iterator it = find(node->keys_.begin(), node->keys_.end(), key);
	*it = sub;
	if (it == node->keys_.end() - 1) {
		if (node->parent_)SubtituteWord(node->parent_, sub, key);
	}
}

void BPlusTree::AdjustDelete(BPlusTreeNode* node)
{
	//���ڵ�
	if (node->parent_ == nullptr) {
		if (node->num_keys_ == 1) {
			BPlusTreeNode* p = root_;
			root_ = root_->children_[0];
			root_->parent_ = nullptr;
			delete p;
		}
		return;
	}
	//�Ǹ��ڵ�
	//�����ֵܽ�
	BPlusTreeNode* left_node = FindLeftSibling(node);
	BPlusTreeNode* right_node = FindRightSibling(node);
	if (left_node != nullptr && left_node->num_keys_ > MIN_M) {//���ֵܽڵ���Խ�
		int borrow = left_node->keys_[left_node->num_keys_ - 1];
		//����key
		node->keys_.insert(node->keys_.begin(), borrow);
		node->num_keys_++;
		//ɾ��key
		left_node->keys_.erase(left_node->keys_.end() - 1);
		left_node->num_keys_--;
		SubtituteWord(left_node->parent_, left_node->keys_[left_node->num_keys_ - 1], borrow);
		if (!node->is_leaf_) {//��Ҷ�ڵ�
			//���뺢��
			node->children_.insert(node->children_.begin(), left_node->children_[left_node->num_keys_]);//numkey-1��λ�������Ķ�.���numkeys�±�ָ����ȥ�Ĺؼ��ʵĺ���,������numkeys-1
			left_node->children_[left_node->num_keys_]->parent_ = node;
			//ɾ������
			left_node->children_.erase(left_node->children_.end() - 1);
		}//û�к��ӣ�ûӰ��
	}
	else if (right_node != nullptr && right_node->num_keys_ > MIN_M) {//���ֵܽ費�������ֵܽ�
		int borrow = *(right_node->keys_.begin());
		//����key
		node->keys_.insert(node->keys_.end(), borrow);
		node->num_keys_++;
		//ɾ��key
		right_node->keys_.erase(right_node->keys_.begin());
		right_node->num_keys_--;

		SubtituteWord(node->parent_, *(node->keys_.end() - 1), *(node->keys_.end() - 2));

		if (!node->is_leaf_) {//��Ҷ�ڵ�
			//���뺢��
			node->children_.push_back(*(right_node->children_.begin()));
			right_node->children_[0]->parent_ = node;
			//ɾ������
			right_node->children_.erase(right_node->children_.begin());
		}
	}
	//�����ֵܽ費��
	//�ϲ����ֵ�
	else if (left_node != nullptr)Merge(left_node, node);
	else if (right_node != nullptr)Merge(node, right_node);
	else {
		cout << "ɾ�����󣡣�" << endl;
	}
}
