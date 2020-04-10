#pragma once
#include <stdexcept>

constexpr bool LEFT = false;
constexpr bool RIGHT = true;

template<class N>
class priority_queue
{
private:
	class Node
	{
	public:
		N item;
		uint64_t priority;
		Node* parent;
		Node* left;
		Node* right;

		Node(N item, uint64_t priority, Node* parent, Node* left, Node* right)
		{
			this->item = item;
			this->priority = priority;
			this->parent = parent;
			this->left = left;
			this->right = right;
		}
		
		~Node()
		{
			this->parent = nullptr;
			this->left = nullptr;
			this->right = nullptr;
		}
	};

	Node* root;
	Node* last;
	size_t size;

	//swaps parent and child values depending on which priority is lesser
	void Balance(Node* current)
	{
		//if current is lesser than its parent
		while (current->parent != nullptr && current->priority < current->parent->priority)
		{
			uint64_t temp_priority = current->priority;
			N temp_item = current->item;

			current->priority = current->parent->priority;
			current->item = current->parent->item;
			
			current->parent->item = temp_item;
			current->parent->priority = temp_priority;
			
			current = current->parent;
		}

		//while current has both children and is lesser than one of them
		while ((current->right != nullptr && current->left != nullptr) && (current->priority > current->left->priority || current->priority > current->right->priority))
		{
			//if left child is lesser than right child or equals it
			if (current->left->priority <= current->right->priority)
			{
				uint64_t temp_priority = current->priority;
				N temp_item = current->item;

				current->priority = current->left->priority;
				current->item = current->left->item;
				current->left->item = temp_item;
				current->left->priority = temp_priority;
				current = current->left;
			}
			//if right child is lesser than left child
			else if (current->right->priority < current->left->priority)
			{
				uint64_t temp_priority = current->priority;
				N temp_item = current->item;
				
				current->priority = current->right->priority;
				current->item = current->right->item;
				current->right->item = temp_item;
				current->right->priority = temp_priority;
				current = current->right;
			}
		}
		//if current has only left child
		if (current->left != nullptr && current->priority > current->left->priority)
		{
			uint64_t temp_priority = current->priority;
			N temp_item = current->item;

			current->priority = current->left->priority;
			current->item = current->left->item;
			current->left->priority = temp_priority;
			current->left->item = temp_item;
			current = current->left;
		}

	}

	//deletes last added element
	void delete_last()
	{
		//if last is the right child
		if (last == last->parent->right)
		{
			Node* temp = last;
			last = last->parent->left;

			last->parent->right = nullptr;
			delete temp;

			size--;
		}
		else if (last == last->parent->left) //if last is the left child
		{
			size_t penultimate = size - 2; //number of second-to-last element
			const uint64_t height = log2(size) + 1; //height of the heap
			bool* path = new bool[height - 1]; //path from root to the second-to-last element
			size_t i = height - 2;

			/*making path from root to the second-to-last element
			which will be a new last element*/
			while (penultimate != 0)
			{
				//if element's number is even
				if (penultimate % 2 == 0)
				{
					path[i] = RIGHT;
					penultimate = (penultimate - 2) / 2;
				}
				//if element's number is uneven
				else if (penultimate % 2 != 0)
				{
					path[i] = LEFT;
					penultimate = (penultimate - 1) / 2;
				}
				--i;
			}

			last->parent->left = nullptr;
			delete last;
			size--;

			last = root;

			i = 0;
			//moving pointer to the new last element through the created path
			while (i < height - 1)
			{
				if (path[i] == LEFT)
					last = last->left;
				else if (path[i] == RIGHT)
					last = last->right;

				++i;
			}

			delete[] path;
		}
	}

	//searches for certain element, returns pointer to element if it's found
	Node* contains(N item, Node*& node)
	{
		Node* found = nullptr; //pointer to the node with the search priority

		if (node == nullptr)
			return nullptr;

		if (node->item == item)
			found = node;

		if (found == nullptr)
			if (node->left != nullptr)
				found = contains(item, node->left);

		if (found == nullptr)
			if (node->right != nullptr)
				found = contains(item, node->right);

		return found;
	}

public:

	priority_queue()
	{
		this->root = nullptr;
		this->last = nullptr;
		size = 0;
	}
	
	~priority_queue()
	{
		if (size > 0)
		{
			while (last->parent != nullptr)
				delete_last();

			delete root;
			size = 0;
		}
	}

	//inserts new element in the heap
	void insert(N item, uint64_t priority)
	{
		//if heap is empty creates root
		if (size == 0)
		{
			root = new Node(item, priority, nullptr, nullptr, nullptr);
			last = root;
			size++;
			return;
		}

		//as long as current is a right child head up
		Node* current = last;
		while (current != root && current == current->parent->right)
			current = current->parent;

		//at this point current is not right child any more
		//if current is not root
		if (current != root)
		{
			//if current's parent has right child 
			if (current->parent->right != nullptr)
			{
				current = current->parent->right;

				//current is now a right child; head down in the left direction
				while (current->left != nullptr)
					current = current->left;
			}
			else
				current = current->parent; //if current's parent has no right child
		}
		else
		{
			//if current is a root then head down in the left direction
			while (current->left != nullptr)
				current = current->left;
		}

		//insert new element as left if current has no left child
		//insert new element as right if current has left child
		if (current->left == nullptr)
		{
			last = new Node(item, priority, current, nullptr, nullptr);
			current->left = last;
		}
		else if (current->left != nullptr)
		{
			last = new Node(item, priority, current, nullptr, nullptr);
			current->right = last;
		}

		size++;
		Balance(last); //balance heap
	}


	//deletes element from the heap
	void remove(N item)
	{
		Node* current = contains(item, root);

		if (current != nullptr)
		{
			//if heap consists of root only 
			if (current == root && size == 1)
			{
				delete root;
				size = 0;
				current = nullptr;
				root = nullptr;
				return;
			}

			//if deleting any element except the last
			if (current != last)
			{
				uint64_t temp_priority = last->priority;
				N temp_item = last->item;
				last->priority = current->priority;
				last->item = current->item;
				current->priority = temp_priority;
				current->item = temp_item;

				delete_last();
				Balance(current);
			}
			else
				delete_last();

		}
		else
			throw std::out_of_range("Can not remove non-existent element");
	}

	//deletes the root
	N extract_min()
	{
		if (root != nullptr)
		{
			uint64_t temp_priority = root->priority;
			N temp_item = root->item;

			//if heap consists of root only 
			if (size == 1)
			{
				delete root;
				size = 0;
				root = nullptr;
			}
			else
			{
				root->priority = last->priority;
				root->item = last->item;
				last->priority = temp_priority;
				last->item = temp_item;

				delete_last();
				Balance(root);
			}

			return temp_item;
		}
		else
			throw std::out_of_range("Can not remove non-existent element");
	}

	uint64_t get_priority(N item) 
	{
		if(root == nullptr)
			throw std::out_of_range("Can not get priority of non-existent element");

		Node* required = contains(item, root);

		if(required == nullptr)
			throw std::out_of_range("Can not get priority of non-existent element");

		return required->priority;
	}

	uint64_t get_priority() const
	{
		if (root == nullptr)
			throw std::out_of_range("Can not get priority of non-existent element");

		return root->priority;
	}

	N get_min()
	{
		if(root == nullptr)
			throw std::out_of_range("Can not get non-existent element");

		return root->item;
	}
	
	//searches for certain element, returns true if element is found
	bool contains(N item)
	{
		//if there is a node with the search priority
		if (contains(item, root) != nullptr)
			return true;
		else
			return false;
	}

	void update(N item, uint64_t new_priority)
	{
		Node* updateable = contains(item, root);

		if (updateable == nullptr)
			return;
		
		updateable->priority = new_priority;
		Balance(updateable);
	}

	//returns size of the heap
	size_t getSize() const
	{
		return size;
	}
};


