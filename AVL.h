
/*
this header file contains the implementation of an AVL rank tree.

the tree is implemented using a template class, so it can be used with any type of data.

the speciality of this tree is the fact that the nodes location in memory is not changing,
thus it is possible to save pointers to nodes and use them later, without the need to find the node again.

all of the trees i found online, dont allow this, so i decided to implement my own, and upload it to github.
i hope it will be useful for you.

written by: Ron Raphaeli , 2023. 
contact me at: ronraphaeli+AVLTREE@technion.ac.il

dedicated to Onika Tanya Maraj-Petty, who taught me how to code, how to love, and how to live.
"He was Adam. I think I was Eve. But my vision ends with the apple on the tree". ~Onika Tanya Maraj

*/



#ifndef AVL_H
#define AVL_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// helper classes
enum struct taskStatus {
	SUCCESS          = 0,
	ALLOCATION_ERROR = 1,
	INVALID_INPUT    = 2,
	FAILURE          = 3,
};
template<class T>
class tupleOutput;
template<class T>
class Node;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class Tree{
    private:
    Node<T>* root_m;

    public:
    //basic methods
    Tree():root_m(nullptr){}
    ~Tree(); // we will use a recursive method to destroy the tree.
    tupleOutput<Node<T>*> insert(const T& data); // complexity O(logn)
    taskStatus remove(const T& data); // complexity O(logn)
    tupleOutput<Node<T>*> find(const T& data); // complexity O(logn)
    void printTree(Node<T>* node); // complexity O(n)

    // advanced methods
    Node<T>* findMin(Node<T>* node); // complexity O(logn)
    Node<T>* findMax(Node<T>* node); // complexity O(logn)
    Node<T>* findParent(Node<T>* node); // complexity O(1)
    Node<T>* findSuccessor(Node<T>* node); // complexity O(logn)
    Node<T>* findPredecessor(Node<T>* node); // complexity O(logn)
    taskStatus inOrder(Node<T>* node, void (*func)(T,int*,void*), int* counter, void* array); // complexity O(n)
    taskStatus inOrderArray(void (*func)(T,int*,void*), void* array); // complexity O(n)
    Node<T>* createTreeFromSortedArray(Node<T>** array, int start, int end,Node<T>* parent = nullptr); // complexity O(n)
    tupleOutput<Node<T>*> findKthElement(int k); // complexity O(logn)
    
    
    
    // helper methods
    Node<T>* getRoot() const {return root_m;} // we will use this method to get the root of the tree.
    taskStatus setRoot(Node<T>* root) {this->root_m = root; return taskStatus::SUCCESS;} // we will use this method to set the root of the tree.
    taskStatus RRrotation(Node<T>* node); // complexity O(1)
    taskStatus LLrotation(Node<T>* node); // complexity O(1)
    taskStatus RLrotation(Node<T>* node); // complexity O(1)
    taskStatus LRrotation(Node<T>* node); // complexity O(1)
    taskStatus rebalance(Node<T>* node); // complexity O(1)
    taskStatus updateHeight(Node<T>* node); // complexity O(1)
    taskStatus switchNodesLocation(Node<T>* node1, Node<T>* node2); // complexity O(1)
    int getBalanceFactor(Node<T>* node); // complexity O(1)
    int getBalance(Node<T>* node); // complexity O(1)
    void DestroyRecursive(Node<T>* node); // complexity O(n)
    taskStatus updateNodesInSubTree(Node<T> *node); // complexity O(1)
    int getSize() const {
        if (root_m == nullptr){
            return 0;
        }
        return root_m->getNodesInSubtree();
    }
    

};


////////////// basic methods //////////////
/*
Tree():root_m(nullptr){}
    ~Tree(); // we will use a recursive method to destroy the tree.
    tupleOutput<Node<T>*> insert(T data); // we will use this method to insert a new node to the tree.
    taskStatus remove(T data); // we will use this method to remove a node from the tree.
    tupleOutput<Node<T>*> find(T data); // we will use this method to find a node in the tree.
    void printTree(Node<T>* node); // complexity O(n)
*/

template<class T>
Tree<T>::~Tree(){
    DestroyRecursive(root_m);
}

template<class T>
tupleOutput<Node<T>*> Tree<T>::insert(const T& data){
    Node<T>* newNode = new Node<T>(data);
    newNode->NodesInSubtree_m = 1;
    if(newNode == nullptr){
        return tupleOutput<Node<T>*>(taskStatus::ALLOCATION_ERROR);
    }
    if(root_m == nullptr){
        root_m = newNode;
        return tupleOutput<Node<T>*>(newNode);
    }
    Node<T>* current = root_m;
    Node<T>* parent = nullptr;
    while(current != nullptr){
        parent = current;
        if(data < current->getData()){
            current = current->getLeft();
        }
        else if(data > current->getData()){
            current = current->getRight();
        }
        else{
            delete newNode;
            return tupleOutput<Node<T>*>(taskStatus::FAILURE);
        }
    }
    newNode->setParent(parent);
    
    if(data < parent->getData()){
        parent->setLeft(newNode);
    }
    else{
        parent->setRight(newNode);
    }
    while (parent != nullptr){
        parent->setNodesInSubtree(parent->getNodesInSubtree() + 1);
        parent = parent->getParent();
    }
    if (root_m != nullptr){
        int rootNodesInSubtree=0;
        if (root_m->getLeft() != nullptr){
            rootNodesInSubtree += root_m->getLeft()->getNodesInSubtree();
        }
        if (root_m->getRight() != nullptr){
            rootNodesInSubtree += root_m->getRight()->getNodesInSubtree();
        }
        root_m->setNodesInSubtree(rootNodesInSubtree + 1);
    }
    taskStatus status = rebalance(newNode);
    if (status != taskStatus::SUCCESS){
        return tupleOutput<Node<T>*>(status);
    }
    return tupleOutput<Node<T>*>(newNode);
}

template<class T>
taskStatus Tree<T>::remove(const T& data){
    tupleOutput<Node<T>*> output= find(data);
    if (output.status() != taskStatus::SUCCESS){
        return output.status();
    }
    Node<T>* node = output.ans();
    if(node == nullptr){
        return taskStatus::FAILURE;
    }
    Node<T>* parent = node->getParent();
    if(node->getLeft() == nullptr && node->getRight() == nullptr){
        if(parent == nullptr){
            setRoot(nullptr);
        }
        else if(parent->getLeft() == node){
            parent->setLeft(nullptr);
            parent -> setNodesInSubtree(parent->getNodesInSubtree() - 1);
        }
        else{
            parent->setRight(nullptr);
            parent -> setNodesInSubtree(parent->getNodesInSubtree() - 1);
        }
        T temp = T();
        temp = node->data_m;
        delete node;
        return rebalance(parent);
    }
    else if(node->getLeft() == nullptr || node->getRight() == nullptr){
        Node<T>* child = node->getLeft() == nullptr ? node->getRight() : node->getLeft();
        if(parent == nullptr){
            setRoot(child);
        }
        else if(parent->getLeft() == node){
            parent->setLeft(child);
            parent -> setNodesInSubtree(parent->getNodesInSubtree() - 1);
        }
        else{
            parent->setRight(child);
            parent -> setNodesInSubtree(parent->getNodesInSubtree() - 1);
        }
        child->setParent(parent);
        T temp;
        temp = node->data_m;
        
        delete node;
        return rebalance(parent);
    }
    else{
        // if we got here, it means that the node has two children.
        Node<T>* successor = node->getRight();
        while(successor->getLeft() != nullptr){
            successor = successor->getLeft();
        }
        T temp = T();
        temp = successor->data_m;
        switchNodesLocation(node, successor);
        remove(temp);
        T temp2 = T();
        temp2 =temp;
        successor->data_m = temp2;
        return taskStatus::SUCCESS;

    }
}

template<class T>
tupleOutput<Node<T>*> Tree<T>::find(const T& data){
    Node<T>* current = root_m;
    while(current != nullptr){
        if(current->data_m == data){
            return tupleOutput<Node<T>*>(current);
        }
        if(current->data_m > data){
            current = current->getLeft();
        }
        else if(current->data_m < data ){
            current = current->getRight();
        }
        else{
            return tupleOutput<Node<T>*>(current);
        }
    }
    return tupleOutput<Node<T>*>(taskStatus::FAILURE);
}

template<class T>
void Tree<T>::printTree(Node<T>* node){
    if (node == nullptr){
        return;
    }
    if (node == root_m){
        std::cout << "-----------------" << std::endl;
        std::cout << root_m->data_m ; 
    }
    printTree(node->getLeft());
    std::cout << node->data_m ;
    printTree(node->getRight());
    if (node == root_m){
        std::cout << "-----------------" << std::endl;
    }
}

//////// advanced methods ////////
/*
// advanced methods
    Node<T>* findMin(Node<T>* node); // complexity O(logn)
    Node<T>* findMax(Node<T>* node); // complexity O(logn)
    Node<T>* findParent(Node<T>* node); // complexity O(1)
    Node<T>* findSuccessor(Node<T>* node); // complexity O(logn)
    Node<T>* findPredecessor(Node<T>* node); // complexity O(logn)
    taskStatus inOrder(Node<T>* node, void (*func)(T,int*,void*), int* counter, void* array); // complexity O(n)
    taskStatus inOrderArray(void (*func)(T,int*,void*), void* array); // complexity O(n)
    Node<T>* createTreeFromSortedArray(Node<T>** array, int start, int end,Node<T>* parent = nullptr); // complexity O(n)
    tupleOutput<Node<T>*> findKthElement(int k); // complexity O(logn)
*/

template<class T>
Node<T>* Tree<T>::findMin(Node<T>* node){
    Node<T>* curr = node;
    while(curr->getLeft() != nullptr){
        curr = curr->getLeft();
    }
    return curr;
}

template<class T>
Node<T>* Tree<T>::findMax(Node<T>* node){
    Node<T>* curr = node;
    if(curr == nullptr){
        return nullptr;
    }
    while(curr->getRight() != nullptr){
        curr = curr->getRight();
    }
    return curr;
}

template<class T>
Node<T>* Tree<T>::findParent(Node<T>* node){
        if(node == nullptr){
            return nullptr;
        }
        return node->getParent();
}

template<class T>
Node<T>* Tree<T>::findSuccessor(Node<T>* node){
    if(node == nullptr){
        return nullptr;
    }
    if(node->getRight() != nullptr){
        return findMin(node->getRight());
    }
    Node<T>* parent = node->getParent();
    while(parent != nullptr && node == parent->getRight()){
        node = parent;
        parent = parent->getParent();
    }
    return parent;
}

template<class T>
Node<T>* Tree<T>::findPredecessor(Node<T>* node){
    if(node == nullptr){
        return nullptr;
    }

    if (node->left_m != nullptr){
        
        return findMax(node->getLeft());
    }
    
    Node<T>* parent = node->getParent();
    
    while(parent != nullptr && node == parent->getLeft()){
        node = parent;
        
        parent = node->getParent();
    }
    
    return parent;
}

template<class T>
Node<T>* findNext(Node<T>* node){
    if(node == nullptr){
        return nullptr;
    }
    if(node->getRight() != nullptr){
        return findMin(node->getRight());
    }
    Node<T>* parent = node->getParent();
    while(parent != nullptr && node == parent->getRight()){
        node = parent;
        parent = parent->getParent();
    }
    return parent;
}

template<class T>
Node<T>* findPrev(Node<T>* node){
    if(node == nullptr){
        return nullptr;
    }
    if(node->getLeft() != nullptr){
        return findMax(node->getLeft());
    }
    Node<T>* parent = node->getParent();
    while(parent != nullptr && node == parent->getLeft()){
        node = parent;
        parent = parent->getParent();
    }
    return parent;
}

template<class T>
taskStatus Tree<T>::inOrder(Node<T>* node, void (*func)(T,int*,void*), int* counter , void* array){
    if(node == nullptr){
        return taskStatus::SUCCESS;
    }
    inOrder(node->getLeft(), func, counter, array);
    func(node->getData(), counter, array);
    *counter = *counter + 1;
    inOrder(node->getRight(), func, counter, array);
    return taskStatus::SUCCESS;
}

template<class T>
taskStatus Tree<T>::inOrderArray(void (*func)(T,int*,void*), void* array){
    int counter = 0;
    inOrder(root_m, func, &counter, array);
    return taskStatus::SUCCESS;
}

template<class T>
Node<T>* Tree<T>::createTreeFromSortedArray(Node<T>** array, int start, int end, Node<T>* parent){
    if(start > end || start < 0 || end < 0){
        return nullptr;
    }
    int mid = (start + end)/2;
    Node<T>* node = array[mid];
    if (parent == nullptr){
        this->setRoot(node);
    }
    else{
        node->setParent(parent);
    }
    node->setLeft(createTreeFromSortedArray(array, start, mid-1, node));
    node->setRight(createTreeFromSortedArray(array, mid+1, end, node));
    updateHeight(node);
    return node;
}

template<class T>
tupleOutput<Node<T>*> Tree<T>::findKthElement(int k){
    if(k < 0 || k >= this->root_m->getNodesInSubtree()){
        return tupleOutput<Node<T>*>(taskStatus::INVALID_INPUT);
    }
    Node<T>* node = this->root_m;
    // we will find the kth smallest element in the tree.
    // if k is 0, we will return the smallest element in the tree.
    // if k is 1, we will return the second smallest element in the tree.
    // and so on.
    // in each node, the field nodesInSubtree_m will hold the number of nodes in the subtree of the node, including the node itself.
    // for example, if the node has 2 leaves, the nodesInSubtree_m will be 3.

    while(node != nullptr){
        if (node->left_m == nullptr){
            if (k == 0){
                return tupleOutput<Node<T>*>( node);
            }
            k--;
            node = node->right_m;
        }
        else{
            if (node->left_m->getNodesInSubtree() == k){
                return tupleOutput<Node<T>*>( node);
            }
            else if (node->left_m->getNodesInSubtree() > k){
                node = node->left_m;
            }
            else{
                k -= node->left_m->getNodesInSubtree() + 1;
                node = node->right_m;
            }
        }
    }
    return tupleOutput<Node<T>*>(taskStatus::FAILURE);
}


//// helper methods ////
/*
    Node<T>* getRoot() const {return root_m;} // we will use this method to get the root of the tree.
    taskStatus setRoot(Node<T>* root) {this->root_m = root; return taskStatus::SUCCESS;} // we will use this method to set the root of the tree.
    taskStatus RRrotation(Node<T>* node); // complexity O(1)
    taskStatus LLrotation(Node<T>* node); // complexity O(1)
    taskStatus RLrotation(Node<T>* node); // complexity O(1)
    taskStatus LRrotation(Node<T>* node); // complexity O(1)
    taskStatus rebalance(Node<T>* node); // complexity O(1)
    taskStatus updateHeight(Node<T>* node); // complexity O(1)
    taskStatus switchNodesLocation(Node<T>* node1, Node<T>* node2); // complexity O(1)
    int getBalanceFactor(Node<T>* node); // complexity O(1)
    int getBalance(Node<T>* node); // complexity O(1)
    void DestroyRecursive(Node<T>* node); // complexity O(n)
    taskStatus updateNodesInSubTree(Node<T> *node); // complexity O(1)
*/


template<class T>
taskStatus Tree<T>::RRrotation(Node<T>* node){
    // we need to remember to update the nodesInSubTree field
    Node<T>* parent = node->getParent();
    Node<T>* right = node->getRight();
    Node<T>* rightLeft = right->getLeft();
    if(parent == nullptr){
        setRoot(right);
    }
    else if(parent->getLeft() == node){
        parent->setLeft(right);
    }
    else{
        parent->setRight(right);
    }
    right->setParent(parent);
    right->setLeft(node);
    node->setParent(right);
    node->setRight(rightLeft);
    if(rightLeft != nullptr){
        rightLeft->setParent(node);

    }
    
    // update the nodesInSubTree field
    updateNodesInSubTree(node);
    updateNodesInSubTree(right);
    


    updateHeight(node);
    updateHeight(right);


    return taskStatus::SUCCESS;
}
// the time complexity of the RRrotation function is O(1).

template<class T>
taskStatus Tree<T>::LLrotation(Node<T>* node){
    Node<T>* parent = node->getParent();
    Node<T>* left = node->getLeft();
    Node<T>* leftRight = left->getRight();
    if(parent == nullptr){
        setRoot(left);
    }
    else if(parent->getLeft() == node){
        parent->setLeft(left);
    }
    else{
        parent->setRight(left);
    }
    left->setParent(parent);
    left->setRight(node);
    node->setParent(left);
    node->setLeft(leftRight);
    if(leftRight != nullptr){
        leftRight->setParent(node);
    }

    // update the nodesInSubTree field
    updateNodesInSubTree(node);
    updateNodesInSubTree(left);

    updateHeight(node);
    updateHeight(left);
    return taskStatus::SUCCESS;
}
// the time complexity of the LLrotation function is O(1).

template<class T>
taskStatus Tree<T>::LRrotation(Node<T>* node){
    Node<T>* left = node->getLeft();
    RRrotation(left);
    LLrotation(node);
    return taskStatus::SUCCESS;
}
// the time complexity of the LRrotation function is O(1).

template<class T>
taskStatus Tree<T>::RLrotation(Node<T>* node){
    Node<T>* right = node->getRight();
    LLrotation(right);
    RRrotation(node);
    return taskStatus::SUCCESS;
}
// the time complexity of the RLrotation function is O(1).

template<class T>
taskStatus Tree<T>::rebalance(Node<T>* node){
    if(node == nullptr){
        return taskStatus::SUCCESS;
    }
    int balance = getBalance(node);
    if(balance > 1){
        if(getBalance(node->getLeft()) >= 0){
            LLrotation(node);
        }
        else{
            LRrotation(node);
        }
    }
    else if(balance < -1){
        if(getBalance(node->getRight()) <= 0){
            RRrotation(node);
        }
        else{
            RLrotation(node);
        }
    }
    else{
        updateHeight(node);
        updateNodesInSubTree(node);
    }

    return rebalance(node->getParent());
}

template<class T>
taskStatus Tree<T>::updateHeight(Node<T>* node){
    if(node == nullptr){
        return taskStatus::SUCCESS;
    }
    int leftHeight = node->getLeft() == nullptr ? 0 : node->getLeft()->getHeight();
    int rightHeight = node->getRight() == nullptr ? 0 : node->getRight()->getHeight();
    int temp = leftHeight > rightHeight ? leftHeight : rightHeight;
    node->setHeight(temp + 1);
    return taskStatus::SUCCESS;
}

template<class T>
taskStatus Tree<T>::switchNodesLocation(Node<T>* node1, Node<T>* node2){
    if(node1 == nullptr || node2 == nullptr){
        return taskStatus::INVALID_INPUT;
    }
    Node<T>* node1Parent= node1->getParent();
    Node<T>* node1Left = node1->getLeft();
    Node<T>* node1Right = node1->getRight();
    Node<T>* node2Parent= node2->getParent();
    Node<T>* node2Left = node2->getLeft();
    Node<T>* node2Right = node2->getRight();
    // lets assume 4 cases:
    // 1. node1 and node2 are seperate nodes, they are not connected to each other.
    // 2. node1 is the parent of node2.
    // 3. node2 is the parent of node1.
    // 4. node1 and node2 are siblings.
    // case 1:
    if (node1Parent != node2 && node1Left != node2 && node1Right != node2 && node1Parent != node2Parent){
        if (node1Parent == nullptr){
            this->setRoot(node2);
        }
        else{
            if (node1Parent->getLeft() == node1){
                node1Parent->setLeft(node2);
            }
            else{
                node1Parent->setRight(node2);
            }
        }
        if (node2Parent == nullptr){
            this->setRoot(node1);
        }
        else{
            if (node2Parent->getLeft() == node2){
                node2Parent->setLeft(node1);
            }
            else{
                node2Parent->setRight(node1);
            }
        }
        node1->setParent(node2Parent);
        node1->setLeft(node2Left); 
        node1->setRight(node2Right);
        node2->setParent(node1Parent);
        node2->setLeft(node1Left);
        node2->setRight(node1Right);
        if (node1Left != nullptr){
            node1Left->setParent(node2);
        }
        if (node1Right != nullptr){
            node1Right->setParent(node2);
        }
        if (node2Left != nullptr){
            node2Left->setParent(node1);
        }
        if (node2Right != nullptr){
            node2Right->setParent(node1);
        }

    }
    //case 2:
    if (node1 == node2Parent){
        if (node1Parent == nullptr){
            this->setRoot(node2);
            node2->setParent(nullptr);
            node1->setParent(node2);
        }
        else{
            if (node1Parent->getLeft() == node1){
                node1Parent->setLeft(node2);
                node2->setParent(node1Parent);
                
            }
            else{
                node1Parent->setRight(node2);
                node2->setParent(node1Parent);
            }
            node1->setParent(node2);

        }
        node1->setLeft(node2Left);
        node1->setRight(node2Right);
        if (node2 == node1Left){
            node2->setLeft(node1);
            node2->setRight(node1Right);
            if (node1Right != nullptr){
                node1Right->setParent(node2);
            }
        } 
        else{
            node2->setLeft(node1Left);
            node2->setRight(node1);
            if (node1Left != nullptr){
                node1Left->setParent(node2);
            }
        }
    }
    //case 3:
    if (node2 == node1Parent){
        switchNodesLocation(node2, node1);
    }
    //case 4:
    if (node1Parent == node2Parent){
        if (node1Parent == nullptr && node2Parent ==nullptr){
            return taskStatus::FAILURE;
        }
        node1->setLeft(node2Left);
        node1->setRight(node2Right);
        node2->setLeft(node1Left);
        node2->setRight(node1Right);
    }
    
    T temp;
    
    temp = node1->data_m;
    node1->data_m=node2->data_m;
    node2->data_m=temp;
    
    int tempHeight = node1->getHeight();
    int tempNodesInSubtree = node1->getNodesInSubtree();

    node1->setHeight(node2->getHeight());
    node2->setHeight(tempHeight);
    
    node1->setNodesInSubtree(node2->getNodesInSubtree());
    node2->setNodesInSubtree(tempNodesInSubtree);


    return taskStatus::SUCCESS;
}

template<class T>
int Tree<T>::getBalance(Node<T>* node){
    if(node == nullptr){
        return 0;
    }
    if (node->getLeft() == nullptr && node->getRight() == nullptr){
        return 0;
    }
    else if(node->getLeft() == nullptr){
        return -(node->getRight()->getHeight());
    }
    else if(node->getRight() == nullptr){
        return node->getLeft()->getHeight();
    }
    else{
        return node->getLeft()->getHeight() - node->getRight()->getHeight();
    }
}

template<class T>
int Tree<T>::getBalanceFactor(Node<T>* node){
    if(node == nullptr){
        return 0;
    }
    int leftHeight = node->getLeft() == nullptr ? 0 : node->getLeft()->getHeight();
    int rightHeight = node->getRight() == nullptr ? 0 : node->getRight()->getHeight();
    return leftHeight - rightHeight;
}

template<class T>
taskStatus Tree<T>::updateNodesInSubTree(Node<T> *node){
    int nodes=1;
    if (node->left_m != nullptr){
        nodes += node->left_m->getNodesInSubtree();
    }
    if (node->right_m != nullptr){
        nodes += node->right_m->getNodesInSubtree();
    }
    node->setNodesInSubtree(nodes);
    return taskStatus::SUCCESS;

}

template<class T>
void Tree<T>::DestroyRecursive(Node<T>* node)
{
    if (node)
    {
        DestroyRecursive(node->getLeft());
        DestroyRecursive(node->getRight());
        delete node;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// helper classes

// The following class is used to support output with status code.
template<typename T>
class tupleOutput {
private:
	
	const taskStatus __status;
	const T __ans;

public:
	tupleOutput() : __status(taskStatus::SUCCESS), __ans(T()) { }
	tupleOutput(taskStatus status) : __status(status), __ans(T()) { }
	tupleOutput(const T &ans) : __status(taskStatus::SUCCESS), __ans(ans) { }
	
	taskStatus status() { return __status; }
	T ans() { return __ans; }
};


template<class T>
class Node{
    public:
    T data_m;
    int height_m;
    int NodesInSubtree_m=0;
    Node<T>* left_m=nullptr;
    Node<T>* right_m=nullptr;
    Node<T>* parent_m=nullptr;

    public:
    Node(T data, Node<T>* parent = nullptr, Node<T>* left = nullptr, Node<T>* right = nullptr);
    ~Node();
    T getData() const {return data_m;}
    int getHeight() const {return height_m;}
    Node<T>* getLeft() const {return left_m;}
    Node<T>* getRight() const {return right_m;}
    taskStatus setLeft(Node<T>* left) {this->left_m = left; return taskStatus::SUCCESS;}
    taskStatus setRight(Node<T>* right) {this->right_m = right; return taskStatus::SUCCESS;}
    taskStatus setHeight(int height) {this->height_m = height; return taskStatus::SUCCESS;}
    taskStatus setData(T data) {this->data_m = data; return taskStatus::SUCCESS;}
    taskStatus setRefData (T & data) {this->data_m = data; return taskStatus::SUCCESS;}
    Node<T>* getParent() const {return parent_m;}
    taskStatus setParent(Node<T>* parent) {this->parent_m = parent; return taskStatus::SUCCESS;}
    int getNodesInSubtree() const {return NodesInSubtree_m;}
    taskStatus setNodesInSubtree(int NodesInSubtree) {this->NodesInSubtree_m = NodesInSubtree; return taskStatus::SUCCESS;}

};



template<class T>
Node<T>::Node(T data, Node<T>* parent, Node<T>* left, Node<T>* right): data_m(data), parent_m(parent), left_m(left), right_m(right), height_m(0){
    this->data_m = data;
    this->parent_m = parent;
    this->left_m = left;
    this->right_m = right;
    this->height_m = 0;
}

template<class T>
Node<T>::~Node(){
    this->left_m = nullptr;
    this->right_m = nullptr;
    this->parent_m = nullptr;

}

 #endif //AVL_H
