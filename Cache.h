#ifndef CACHE_H
#define CACHE_H

#include "main.h"


class ReplacementPolicy {
protected:
	int count;
	Elem** arr;
public:
	virtual int insert(Elem* e, int idx) = 0;//insert e into arr[idx] if idx != -1 else into the position by replacement policy
	virtual void access(int idx) = 0;//idx is index in the cache of the accessed element 
	virtual int remove() = 0;
	virtual void print() = 0;
	virtual Elem* dequeuevalue() = 0;
	virtual int getCount() = 0;

	bool isFull() { return count == MAXSIZE; }
	bool isEmpty() { return count == 0; }
	Elem* getValue(int idx) { return arr[idx]; }

	virtual void replace(int idx, Elem* e) = 0;
	ReplacementPolicy(){
		count = 0;
		arr = NULL;
	}
	virtual ~ReplacementPolicy() {}

};

class SearchEngine {
	public:
	virtual int search(int key) = 0; // -1 if not found
	virtual void insert(int key,int idx) = 0;
	virtual	void deleteNode(int key) = 0;
	virtual void print(ReplacementPolicy* r) = 0;
	virtual void update_index(ReplacementPolicy* rp) = 0;
	//SearchEngine(){}
	virtual ~SearchEngine() {}

};

class FIFO : public ReplacementPolicy {
	public:
	FIFO(){count = 0; arr = new Elem*[MAXSIZE];}
	~FIFO(){
		delete[]arr;
	}
	int insert(Elem* e,int idx)
	{
		arr[idx] = e;
		count++;
		return idx;
	}
	void access(int idx) {}
	int remove()
	{	
		int temp = arr[0]->addr;

		for (int i = 0; i < MAXSIZE; i++)
		{
			arr[i] = arr[i + 1];
		}
		count--;
		return temp; 
	}
	void print()
	{
		for (int i = 0; i < count; i++)
		{
			arr[i]->print();
		}
	}
	int getCount()
	{
		return count;
	}
	Elem* dequeuevalue() 
	{ 
		
		return arr[0]; 
	}

	void replace(int idx, Elem* e)
	{
		delete arr[idx];
		arr[idx] = e;
		access(idx);
	}
};
	
class MRU : public ReplacementPolicy {
	public:
	MRU()
	{
		count = 0; arr = new Elem * [MAXSIZE];
	}
	~MRU()
	{
		delete[]arr;
	}

	int insert(Elem* e,int idx)
	{
		for (int i=idx;i>0;i--)
		{
			arr[i] = arr[i - 1];
		}
		arr[0] = e;
		count++;
		return idx;
	}
	void access(int idx) 
	{
		Elem* temp = arr[idx];
		for (int i = idx; i > 0; i--)
		{
			arr[i] = arr[i - 1];
		}
		arr[0] = temp;
	}
	int remove()
	{
		for (int i = 0; i < count-1; i++)
		{
			arr[i] = arr[i + 1];
		}
		arr[count-1] = NULL;
		count--;
		return count;

	}
	void print()

	{
		for (int i = 0; i < count; i++)
		{
			arr[i]->print();
		}

	}
	int getCount()
	{
		return count;
	}

	Elem* dequeuevalue() 
	{
		return arr[0];
	}
	void replace(int idx, Elem* e)
	{
		delete arr[idx];
		arr[idx] = e;
		access(idx);
	}

};
class LRU : public MRU {
public:
	int remove() override 
	{
		arr[MAXSIZE - 1] = NULL;
		count--;
		return count;
	}

	Elem* dequeuevalue() override 
	{
		return arr[MAXSIZE-1];
	}

};

class LFU : public ReplacementPolicy {
public:
	class LFU_elem {
	public:
		Elem* ele;
		int freq;
		LFU_elem(Elem* e)
		{
			ele = e;
			freq = 0;
		};
		~LFU_elem()
		{
			delete ele;
		};
	};

	LFU_elem** heap;
	LFU()
	{
		count = 0;
		arr = new Elem * [MAXSIZE];
		heap = new LFU_elem * [MAXSIZE];
	}
	~LFU()
	{
		delete[]arr;
		delete[]heap;

	}
	int insert(Elem* e, int idx)
	{
		arr[idx] = e;
		heap[idx] = new LFU_elem(e);
		heap[idx]->freq++;
		count++;
		reheap_up(count);
		for (int i = 0; i < count; i++) {
			arr[i] = heap[i]->ele;
		}		
		return idx;
	}

	void access(int idx)
	{
		heap[idx]->freq++;
		reheap_down(idx);
		for (int i = 0; i < count; i++) 
		{
			arr[i] = heap[i]->ele;
		}
	} 

	int remove()
	{	
		LFU_elem* temp = heap[0];	
		heap[0] = heap[count-1];
		heap[count-1] = temp;
		reheap_down(0); 
		for (int i = 0; i < count-1; i++) 
		{
			arr[i] = heap[i]->ele;
		}
		arr[count-1] = NULL;
		count--;
		return -1;
	}

	void print()
	{
		for (int i = 0; i < count; i++) 
		{
			arr[i]->print();
		}

	}
	void reheap_up(int hs) 
	{

		int i = hs - 1;
		LFU_elem* temp = heap[0];

		while ((i - 1) / 2 >= 0) 
		{
			if (heap[i]->freq < heap[(i - 1) / 2]->freq) 
			{
				temp = heap[i];
				heap[i] = heap[(i - 1) / 2];
				heap[(i - 1) / 2] = temp;
				i = (i - 1) / 2;
			}
			else 
			{
				break;
			}
		}
		
	};

	
	void reheap_down(int hs) 
	{

		int i = hs;
		LFU_elem* temp = heap[i];
		while (2 * i + 2 < count) 
		{
			if ((heap[i]->freq >= heap[2 * i + 1]->freq) && (heap[2 * i + 1]->freq > heap[2 * i + 2]->freq)) 
			{
				temp = heap[i];
				heap[i] = heap[2 * i + 2];
				heap[2 * i + 2] = temp;
				i = 2 * i + 2;
			}
			else if (heap[i]->freq >= heap[2 * i + 1]->freq) 
			{
				temp = heap[i];
				heap[i] = heap[2 * i + 1];
				heap[2 * i + 1] = temp;
				i = 2 * i + 1;
			}
			else if (heap[i]->freq >= heap[2 * i + 2]->freq) 
			{
				temp = heap[i];
				heap[i] = heap[2 * i + 2];
				heap[2 * i + 2] = temp;
				i = 2 * i + 2;
			}
			else break;
		}
		if (2 * i + 1 < count) 
		{
			if (heap[i]->freq >= heap[2 * i + 1]->freq) 
			{
				temp = heap[i];
				heap[i] = heap[2 * i + 1];
				heap[2 * i + 1] = temp;
				i = 2 * i + 1;
			}
		}
	};


	Elem* dequeuevalue() { return heap[0]->ele; }
	int getCount()
	{
		return count;
	}

	void replace(int idx, Elem* e)
	{
		access(idx);
		for (int i = 0; i < count; i++) 
		{
			if (arr[i]->addr == e->addr) 
			{
				arr[i] = e;
				heap[i]->ele = e;
			}
		}
	}
};
class Node {
public:
	Node* left;
	Node* right;
	int idx;
	int key;
	int height;
	Node(int k, int id) {
		key = k;
		idx = id;
		left = right = NULL;
		height = 1;
	}
	~Node()
	{
		left = right = NULL;
	}
};

class DBHashing: public SearchEngine {public:

	Node** hashtable;
	int size;
	int cnt;
	int (*ha1)(int);
	int (*ha2)(int);

	DBHashing(int (*h1)(int),int (*h2)(int),int s){
		
		hashtable = new Node * [s];
		size = s;
		cnt = 0;
		ha1 = h1;
		ha2 = h2;
		for (int i = 0; i < s; i++)
		{
			hashtable[i] = new Node(-1, -1);
		}
	
	}
	~DBHashing(){
		delete[] hashtable;
	}
	void insert(int key, int i);
	void deleteNode(int key);
	void print(ReplacementPolicy* q);
	int search(int key);
	void update_index(ReplacementPolicy* rp);

	bool collision_check(int k_c) {
		if (hashtable[k_c % size ]->key != -1)
			return true;
		else return false;
	}
};


void DBHashing:: insert(int key, int i) {
	//if (cnt == size) return; 

	int idx1 = ha1(key) ;
	int idx2 = ha2(key) ;
	
	if (!collision_check(idx1))
	{
		hashtable[idx1 % size]->key = key;
		hashtable[idx1 % size]->idx = i;
	}
	else
	{
		int j = 1;
		while (true)
		{
			int idx_temp = (idx1 + j*idx2) % size;
			if (collision_check(idx_temp))
			{
				j++;
			}
			else
			{
				hashtable[idx_temp]->idx = i;
				hashtable[idx_temp]->key = key;
				break;
			}

		}
	}
	cnt++;
	return;


}






void DBHashing::deleteNode(int key) {


	for (int i = 0; i < size; i++)
	{
		if (hashtable[i]->key == key)
		{
			hashtable[i]->key = -1;
			hashtable[i]->idx = -1;
			break;
		}
	}
	cnt--;
	return;
}


void DBHashing::print(ReplacementPolicy* q) {
	cout << "Prime memory:" << endl;
	for (int i = 0; i < size; i++)
	{
		if (hashtable[i]->key != -1 )
		{
			q->getValue(hashtable[i]->idx)->print();
		}
	}
	return;
}


int DBHashing::search(int key) 
{ 
	for (int i = 0; i < size; i++)
	{
		if (hashtable[i]->key == key)
			return hashtable[i]->idx;
	}
	return -1;
	
}


void DBHashing::update_index(ReplacementPolicy* rp) {

	for (int i = 0; i < rp->getCount(); i++)
	{
		for (int j = 0; j < size; j++)
		{
				if (hashtable[j]->key == rp->getValue(i)->addr)
				{
					hashtable[j]->idx = i;
					break;
				}

		}
		

	}
	return;



};

//*****************************************************************************
//*************************  OLD_AVL  *****************************************
//*****************************************************************************
class AVL : public SearchEngine {
public:

	//________________
public:
	Node* root;
	AVL() {
		root = NULL;
	}
	~AVL() {}
	void insert(int key, int i);
	void deleteNode(int key);

	void print(ReplacementPolicy* q) {
		cout << "Print AVL in inorder:" << endl;
		InPrint(root, q);
		cout << "Print AVL in preorder:" << endl;
		PrePrint(root, q);
	}
	int search(int key) {
		return search_AVL(root, key);
	}

	int findH(Node* N);
	Node* rightRotate(Node* y);
	Node* leftRotate(Node* x);
	Node* minValueNode(Node* node);
	int getBalance(Node* N);

	Node* insertNode(Node* node, int k, int id);
	Node* deleteNode2(Node* root, int k);

	void PrePrint(Node* root, ReplacementPolicy* q)
	{
		if (root == NULL)
			return;
		q->getValue(root->idx)->print();
		PrePrint(root->left, q);
		PrePrint(root->right, q);
	}
	void InPrint(Node* root, ReplacementPolicy* q)
	{
		if (root == NULL)
			return;
		InPrint(root->left, q);
		q->getValue(root->idx)->print();
		InPrint(root->right, q);
	}

	int search_AVL(Node* root, int key);
	void update_index(ReplacementPolicy* rp)
	{

		for (int i = 0; i < rp->getCount(); i++)
		{
			Node* temp = root;

			while (temp)
			{

				if (temp->key == rp->getValue(i)->addr)
				{
					temp->idx = i;
					break;
				}

				if (temp->key > rp->getValue(i)->addr)
				{
					temp = temp->left;
					if (temp == NULL) break;
				}

				if (temp->key < rp->getValue(i)->addr)
				{
					temp = temp->right;
					if (temp == NULL) break;

				}
			}

		}



	}
};

int AVL::findH(Node* N)
{
	if (N == NULL)return 0;
	else return N->height;

}

Node* AVL::rightRotate(Node* y)
{
	Node* x = y->left;
	Node* T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = max(findH(y->left), findH(y->right)) + 1;
	x->height = max(findH(x->left), findH(x->right)) + 1;

	return x;
}

Node* AVL::leftRotate(Node* x)
{
	Node* y = x->right;
	Node* T2 = y->left;

	y->left = x;
	x->right = T2;

	x->height = max(findH(x->left), findH(x->right)) + 1;
	y->height = max(findH(y->left), findH(y->right)) + 1;

	return y;
}

int AVL::getBalance(Node* N)
{
	if (N == NULL)
		return 0;
	return findH(N->left) - findH(N->right);
}

Node* AVL::minValueNode(Node* node)
{
	Node* current = node;

	/* loop down to find the leftmost leaf */
	while (current->left != NULL)
		current = current->left;

	return current;
}

Node* AVL::insertNode(Node* node, int k, int id) {
	if (node == NULL) {
		node = new Node(k, id);
		return node;
	}

	if (k < node->key) {
		node->left = insertNode(node->left, k, id);
	}
	else if (k > node->key) {
		node->right = insertNode(node->right, k, id);
	}
	else { // Equal keys are not allowed in BST
		return node;
	}

	node->height = 1 + max(findH(node->left), findH(node->right));

	int balance = getBalance(node);

	// Left Left Case
	if (balance > 1 && k < node->left->key) {
		return rightRotate(node);
	}

	// Right Right Case
	if (balance < -1 && k > node->right->key) {
		return leftRotate(node);
	}

	// Left Right Case
	if (balance > 1 && k > node->left->key) {
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	// Right Left Case
	if (balance < -1 && k < node->right->key) {
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}

Node* AVL::deleteNode2(Node* root, int k)
{

	// STEP 1: PERFORM STANDARD BST DELETE 
	if (root == NULL)
		return root;

	// If the key to be deleted is smaller 
	// than the root's key, then it lies
	// in left subtree 
	if (k < root->key)
		root->left = deleteNode2(root->left, k);

	// If the key to be deleted is greater 
	// than the root's key, then it lies 
	// in right subtree 
	else if (k > root->key)
		root->right = deleteNode2(root->right, k);

	// if key is same as root's key, then 
	// This is the node to be deleted 
	else
	{
		// node with only one child or no child 
		if ((root->left == NULL) ||
			(root->right == NULL))
		{
			Node* temp = root->left ? root->left : root->right;

			// No child case 
			if (temp == NULL)
			{
				temp = root;
				root = NULL;
			}
			else // One child case 
				*root = *temp; // Copy the contents of 
							   // the non-empty child 
			//free(temp);
		}
		else
		{
			// node with two children: Get the inorder 
			// successor (smallest in the right subtree) 
			Node* temp = minValueNode(root->right);

			// Copy the inorder successor's 
			// data to this node 
			root->key = temp->key;

			// Delete the inorder successor 
			root->right = deleteNode2(root->right,
				temp->key);
		}
	}

	// If the tree had only one node
	// then return 
	if (root == NULL)
		return root;

	// STEP 2: UPDATE HEIGHT OF THE CURRENT NODE 
	root->height = 1 + max(findH(root->left),
		findH(root->right));

	// STEP 3: GET THE BALANCE FACTOR OF 
	// THIS NODE (to check whether this 
	// node became unbalanced) 
	int balance = getBalance(root);

	// If this node becomes unbalanced, 
	// then there are 4 cases 

	// Left Left Case 
	if (balance > 1 &&
		getBalance(root->left) >= 0)
		return rightRotate(root);

	// Left Right Case 
	if (balance > 1 &&
		getBalance(root->left) < 0)
	{
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}

	// Right Right Case 
	if (balance < -1 &&
		getBalance(root->right) <= 0)
		return leftRotate(root);

	// Right Left Case 
	if (balance < -1 &&
		getBalance(root->right) > 0)
	{
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}

	return root;
}

void AVL::insert(int key, int i)
{
	root = insertNode(root, key, i);
}

void AVL::deleteNode(int key)
{
	root = deleteNode2(root, key);
}

int AVL::search_AVL(Node* root, int key)
{
	while (root != NULL) {
		if (key == root->key) {
			return root->idx;
		}
		else if (key < root->key) {
			return search_AVL(root->left, key);
		}
		else if (key > root->key) {
			return search_AVL(root->right, key);
		}
	}
	return -1;
}

//*****************************************************************************
//*************************   END OF AVL    ***********************************
//*****************************************************************************



//*****************************************************************************
//*************************    HASH    ****************************************
//*****************************************************************************

#endif