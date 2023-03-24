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
	//root_不空,进行查询
	BPlusTreeNode* node = Search(key, doinsert);
	for (auto i : node->keys_) {
		if (i == key) {
			cout << "插入重复" << endl;
			return;//冗余，返回
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
	//未找到该节点，删除失败
	if (node == nullptr)flag = 0;
	else {
		for (auto i : node->keys_) {
			if (i == key)flag = 1;
		}
	}
	if (flag == 0) {
		cout << "未找到该元素，删除失败" << endl;
		return;
	}
	//找到该节点对应的叶节点
	// 一个根节点
	if (node->parent_ == nullptr && node->num_keys_ == 1) {
		delete node;
		root_ = nullptr;
		return;
	}
	//如果是父节点的关键词
	vector<int>::iterator it = find(node->keys_.begin(), node->keys_.end(), key);
	if (it == node->keys_.end() - 1) {
		int sub = node->keys_[node->num_keys_ - 2];
		if (node->parent_)SubtituteWord(node->parent_, sub, key);
	}
	node->keys_.erase(it);
	node->num_keys_--;
	if (node->parent_ != nullptr) {//非根节点
		if (node->num_keys_ < MIN_M)AdjustDelete(node);
	}
}

BPlusTreeNode* BPlusTree::Search(int key, OpMode om)
{
	BPlusTreeNode* node = root_;
	while (node != nullptr) {
		if (node->is_leaf_) {//叶节点直接寻找并且返回，这里遍历是为了查找是否存在，但是查询功能没有写
			for (int i = 0; i < node->num_keys_; i++) {
				if (node->keys_[i] == key)return node;
			}
			return node;//方便增加删除确定位置
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

			if (i == iend) {//插入操作，比最大值还大需要修改沿途的值
				if (om == doremove || om == dosearch) {//删除操作或查询，比最大值还大,说明不存在,直接返回空
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

	// 创建一个新的节点new_node
	BPlusTreeNode* new_node = new BPlusTreeNode(node->is_leaf_);

	// 将原节点的一半的key和child分别复制到new_node中，并更新原节点的num_keys_
	int mid = node->num_keys_ / 2;
	for (int i = 0; i < mid; i++) {
		new_node->keys_.push_back(node->keys_[0]);
		node->keys_.erase(node->keys_.begin());
		if (node->is_leaf_ == false) {//非叶节点
			new_node->children_.push_back(node->children_[0]);
			node->children_[0]->parent_ = new_node;
			node->children_.erase(node->children_.begin());
		}
	}
	new_node->num_keys_ = mid;
	node->num_keys_ = node->num_keys_ - mid;
	//根节点
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
	//非根节点
	BPlusTreeNode* node_parent = node->parent_;
	//it指向当前节点对应的parent的child
	vector<int>::iterator it = node_parent->keys_.begin();
	for (; it != node_parent->keys_.end(); it++) {
		if (*it == node->keys_[node->num_keys_ - 1])break;
	}
	new_node->parent_ = node->parent_;
	//叶节点
	if (node->is_leaf_) {
		new_node->next = node;
		if ((it - node_parent->keys_.begin()) != 0) {//这个结点不是表头
			node_parent->children_[(it - node_parent->keys_.begin()) - 1]->next = new_node;
		}
	}
	//添加父节点
	int index = (int)(it - node_parent->keys_.begin());//迭代器相减直接放入参数中报错，应该是没有办法隐式转换，这里用index做中间变量，本程序中其他index作用类似
	node_parent->keys_.insert(it, new_node->keys_[new_node->num_keys_ - 1]);
	node_parent->children_.insert(node_parent->children_.begin() + index, new_node);
	if (++(node_parent->num_keys_) > MAX_M)Split(node_parent);
}

void BPlusTree::Merge(BPlusTreeNode* node, BPlusTreeNode* other_node)//node左边，other_node右边
{
	//合并到右边的节点
	BPlusTreeNode* parent = node->parent_;
	//删除左边对应的父节点关键词
	vector<int>::iterator it = find(parent->keys_.begin(), parent->keys_.end(), *(node->keys_.end() - 1));
	int index = (int)(it - parent->keys_.begin());
	it = parent->keys_.erase(it);
	parent->num_keys_--;
	//删除父节点的孩子指针
	parent->children_.erase(find(parent->children_.begin(), parent->children_.end(), parent->children_[index]));
	//找到node前一个节点
	if (node->is_leaf_)//叶节点
		if (it != parent->keys_.begin()) {//有前驱节点
			it--;
			index = (int)(it - parent->keys_.begin());
			BPlusTreeNode* pre_node = parent->children_[index];
			pre_node->next = other_node;
		}
	//合并到右节点
	for (int i = node->num_keys_ - 1; i >= 0; i--) {
		other_node->keys_.insert(other_node->keys_.begin(), node->keys_[i]);
		if (!node->is_leaf_) {
			//非叶节点
			other_node->children_.insert(other_node->children_.begin(), node->children_[i]);
			node->children_[i]->parent_ = other_node;
		}
		other_node->num_keys_++;
	}
	//释放内存
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
	//根节点
	if (node->parent_ == nullptr) {
		if (node->num_keys_ == 1) {
			BPlusTreeNode* p = root_;
			root_ = root_->children_[0];
			root_->parent_ = nullptr;
			delete p;
		}
		return;
	}
	//非根节点
	//左右兄弟借
	BPlusTreeNode* left_node = FindLeftSibling(node);
	BPlusTreeNode* right_node = FindRightSibling(node);
	if (left_node != nullptr && left_node->num_keys_ > MIN_M) {//左兄弟节点可以借
		int borrow = left_node->keys_[left_node->num_keys_ - 1];
		//插入key
		node->keys_.insert(node->keys_.begin(), borrow);
		node->num_keys_++;
		//删除key
		left_node->keys_.erase(left_node->keys_.end() - 1);
		left_node->num_keys_--;
		SubtituteWord(left_node->parent_, left_node->keys_[left_node->num_keys_ - 1], borrow);
		if (!node->is_leaf_) {//非叶节点
			//插入孩子
			node->children_.insert(node->children_.begin(), left_node->children_[left_node->num_keys_]);//numkey-1后，位置有所改动.因此numkeys下标指向借出去的关键词的孩子,而不是numkeys-1
			left_node->children_[left_node->num_keys_]->parent_ = node;
			//删除孩子
			left_node->children_.erase(left_node->children_.end() - 1);
		}//没有孩子，没影响
	}
	else if (right_node != nullptr && right_node->num_keys_ > MIN_M) {//左兄弟借不了找右兄弟借
		int borrow = *(right_node->keys_.begin());
		//插入key
		node->keys_.insert(node->keys_.end(), borrow);
		node->num_keys_++;
		//删除key
		right_node->keys_.erase(right_node->keys_.begin());
		right_node->num_keys_--;

		SubtituteWord(node->parent_, *(node->keys_.end() - 1), *(node->keys_.end() - 2));

		if (!node->is_leaf_) {//非叶节点
			//插入孩子
			node->children_.push_back(*(right_node->children_.begin()));
			right_node->children_[0]->parent_ = node;
			//删除孩子
			right_node->children_.erase(right_node->children_.begin());
		}
	}
	//左右兄弟借不了
	//合并左兄弟
	else if (left_node != nullptr)Merge(left_node, node);
	else if (right_node != nullptr)Merge(node, right_node);
	else {
		cout << "删除错误！！" << endl;
	}
}
