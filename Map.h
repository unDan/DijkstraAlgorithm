#pragma once
#include <string>
#include <iostream>
#include <list>
#include <stdexcept>
#include "List.h"

typedef bool Color;

template <class Key, class Val>
class Map
{
	enum _colors { BLACK = 0, RED };
	
	class Node
	{
	public:
		Key key;
		Val value;
		Color color;
		Node* parent;
		Node* left;
		Node* right;

		Node(Key key, Val value, Color color, Node* parent, Node* left, Node* right)
		{
			this->key = key;
			this->value = value;
			this->color = color;
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
	size_t size;

	//returns node's parent
	Node* father(Node* current)
	{
		if(current == nullptr)
			return nullptr;
		
		return current->parent;
	}

	//returns node's parent's parent
	Node* grandfather(Node* current)
	{
		if (current == nullptr || father(current) == nullptr)
			return nullptr;

		return current->parent->parent;
	}

	//returns node's parent's parent's other child
	Node* uncle(Node* current)
	{
		Node* g = grandfather(current);
		
		if (g == nullptr)
			return nullptr;
		
		if (father(current) == g->left)
			return g->right;

		if (father(current) == g->right)
			return g->left;
	}

	//returns node's parent's other child
	Node* brother(Node* current)
	{
		if (current == father(current)->left)
			return father(current)->right;
		else
			return father(current)->left;
	}

	void rotation_left(Node* rotatable)
	{
		Node* pivot = rotatable->right;

		pivot->parent = rotatable->parent;

		//if rotatable is not root
		if (rotatable->parent != nullptr)
		{
			if (rotatable->parent->left == rotatable)
				rotatable->parent->left = pivot;
			else
				rotatable->parent->right = pivot;
		}
		else
			root = pivot;

		rotatable->right = pivot->left;
		if (pivot->left != nullptr)
			pivot->left->parent = rotatable;

		rotatable->parent = pivot;
		pivot->left = rotatable;

	}

	void rotation_right(Node* rotatable)
	{
		Node* pivot = rotatable->left;

		pivot->parent = rotatable->parent;

		//if rotatable is not root
		if (rotatable->parent != nullptr)
		{
			if (rotatable->parent->left == rotatable)
				rotatable->parent->left = pivot;
			else
				rotatable->parent->right = pivot;
		}
		else
			root = pivot;

		rotatable->left = pivot->right;
		if (pivot->right != nullptr)
			pivot->right->parent = rotatable;

		rotatable->parent = pivot;
		pivot->right = rotatable;
	}

	void insert_balance(Node* current)
	{
		//--- case 1: current has no father (often shit nowadays)---//
		if (father(current) == nullptr)
			current->color = BLACK;
		else
		{
			//--- case 2: current has BLACK father (impossible) ---//
			if (father(current)->color == BLACK)
				return;
			else
			{
				//--- case 3: uncle is red (communists even are in my code) ---//
				if (uncle(current) != nullptr && uncle(current)->color == RED)
				{
					father(current)->color = BLACK;
					uncle(current)->color = BLACK;
					grandfather(current)->color = RED;
					insert_balance(grandfather(current));
				}
				else
				{
					//--- case 4.1: current is right (libertarian) and his father is left (communist) (typical post-soviet russian family) ---//
					if (current == father(current)->right && father(current) == grandfather(current)->left)
					{
						rotation_left(father(current));
						current = current->left;
					}
					//--- case 4.2: current is left (communist) and his father is right (libertarian) (lol wtf) ---//
					else if (current == father(current)->left && father(current) == grandfather(current)->right)
					{
						rotation_right(father(current));
						current = current->right;
					}
					
					//--- case 5: both current and father are right/left (both have the same political opinions) (why not?) ---//		
					father(current)->color = BLACK;
					grandfather(current)->color = RED;

					if (current == father(current)->left && father(current) == grandfather(current)->left)
						rotation_right(grandfather(current));
					else 
						rotation_left(grandfather(current));
				}
			}
		}
	}

	void remove_balance(Node* current)
	{
		//--- case 1: current has father (rare shit nowadays)---//
		if(father(current) != nullptr)
		{
			//--- case 2: brother is red ---//
			if (brother(current)->color == RED)
			{
				father(current)->color = RED;
				brother(current)->color = BLACK;

				if (current == father(current)->left)
					rotation_left(father(current));
				else
					rotation_right(father(current));
			}

			//--- case3: the whole family is black ---//
			if (brother(current)->color == BLACK && father(current)->color == BLACK &&
				brother(current)->right->color == BLACK && brother(current)->left->color == BLACK)
			{
				brother(current)->color = RED;
				remove_balance(father(current));
			}
			else 
			{
				//--- case 4: the whole family except father is black (seems like mom was cheating) ---//
				if (brother(current)->color == BLACK && father(current)->color == RED &&
					brother(current)->right->color == BLACK && brother(current)->left->color == BLACK)
				{
					brother(current)->color = RED;
					father(current)->color = BLACK;
				}
				else
				{
					//--- case 5: one of brother's children is red (sounds like communism propaganda but ok)---//
					if (current == father(current)->left && brother(current)->right->color == BLACK && brother(current)->left->color == RED)
					{
						brother(current)->color = RED;
						brother(current)->left->color = BLACK;
						rotation_right(brother(current));
					}
					else if (current == father(current)->right && brother(current)->right->color == RED && brother(current)->left->color == BLACK)
					{
						brother(current)->color = RED;
						brother(current)->right->color = BLACK;
						rotation_left(brother(current));
					}

					//--- case 6: the whole family except one of brother's children is black ---//
					brother(current)->color = father(current)->color;
					father(current)->color = BLACK;

					if (current == father(current)->left)
					{
						brother(current)->right->color = BLACK;
						rotation_left(father(current));
					}
					else
					{
						brother(current)->left->color = BLACK;
						rotation_right(father(current));
					}
				}
			}
		}
	}
	
	void insert(Key key, Val value, Node* current, Node* parent)
	{
		if (current == nullptr)
		{
			Node* node = new Node(key, value, RED, parent, nullptr, nullptr);

			if (node->key < parent->key)
				parent->left = node;
			else if(node->key >= parent->key)
				parent->right = node;

			insert_balance(node);
		}
		else
		{
			if (key < current->key)
				insert(key, value, current->left, current);
			else if (key >= current->key)
				insert(key, value, current->right, current);
		}
	}

	void clear(Node* node)
	{
		if (node != nullptr)
		{
			clear(node->left);
			clear(node->right);
			delete node;
		}
	}

	void remove(Node* removable)
	{
		//--- case 1: removable has no children ---//
		if (removable->right == nullptr && removable->left == nullptr)
		{
			if (father(removable) != nullptr)
			{
				if (removable == father(removable)->left) //removable is left child
					father(removable)->left = nullptr;
				else if (father(removable)->right) //removable is right child
					father(removable)->right = nullptr;
			}
			else
				root = nullptr;
			
			delete removable;
		}
		//--- case 2: removable has one child only ---//
		else if (removable->left == nullptr || removable->right == nullptr)
		{
			Node* child = nullptr;
			
			if (removable->left == nullptr) //removable has no left child
				child = removable->right;
			else if (removable->right == nullptr) //removable has no right child
				child = removable->left;
			
			if (father(removable) != nullptr)
			{
				if (removable == father(removable)->left) //removable is left child
				{
					father(removable)->left = child;
					child->parent = father(removable);
				}
				else if (removable == removable->parent->right) //removable is right child
				{
					father(removable)->right = child;
					child->parent = father(removable);
				}

				if (removable->color == BLACK)
				{					
					if (child->color == RED)
						child->color = BLACK;
					else
						remove_balance(child);
				}
				
				delete removable;
			}
			else
				root = child;
		
		}
		//--- case 3: removable has both children ---//
		else
		{
			Node* temp = removable;
			removable = removable->right;

			//finding node with the maximal key in removable right subtree
			while (removable->left != nullptr)
				removable = removable->left;

			std::swap(removable->key, temp->key);
			std::swap(removable->value, temp->value);

			remove(removable);
		}
	}

	//returns true if key is in the map
	bool contains(Key& key, Node* node)
	{
		if (node != nullptr)
		{
			if (key < node->key)
				return contains(key, node->left);
			else if (key > node->key)
				return contains(key, node->right);
			else if (key == node->key)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	Val find(Key& key, Node* node)
	{
		if (node != nullptr)
		{
			if (key < node->key)
				return find(key, node->left);
			else if (key > node->key)
				return find(key, node->right);
			else if (key == node->key)
				return node->value;
			else
				throw std::out_of_range("This key is not in the map!");

		}
	}
	

	//returns pointer to the node with the key
	Node* find_node(Key& key, Node* node)
	{
		if (node != nullptr)
		{
			if (key < node->key)
				return find_node(key, node->left);
			else if (key > node->key)
				return find_node(key, node->right);
			else if (key == node->key)
				return node;
			else
				throw std::out_of_range("This key is not in the map!");

		}
	}

	//symmetrically traverses the tree and works depending on the selector
	void in_order_traverse_keys(Node* node, List<Key>& returnable)
	{
		if (node != nullptr)
		{
			in_order_traverse_keys(node->left, returnable);
			returnable.push_back(node->key);
			in_order_traverse_keys(node->right, returnable);
		}
	}

	//symmetrically traverses the tree and works depending on the selector
	void in_order_traverse_values(Node* node, List<Val>& returnable)
	{
		if (node != nullptr)
		{
			in_order_traverse_values(node->left, returnable);
			returnable.push_back(node->value);
			in_order_traverse_values(node->right, returnable);
		}
	}

	//symmetrically traverses the tree and works depending on the selector
	void in_order_traverse_print(Node* node)
	{
		if (node != nullptr)
		{
			in_order_traverse_print(node->left);		
			std::cout << "[" << node->key << ":" << node->value << "] ";
			in_order_traverse_print(node->right);
		}
	}
	
public:

	//default constructor
	Map()
	{
		root = nullptr;
		size = 0;
	}

	//constructor that initializes map with 1 element
	Map(Key key, Val value)
	{
		size = 0;
		root = nullptr;
		insert(key, value);
	}

	~Map()
	{
		clear();
	}

	//add a key:value pair in the map
	void insert(Key key, Val value)
	{
		if (root == nullptr)
		{
			root = new Node(key, value, BLACK, nullptr, nullptr, nullptr);
		}
		else
			insert(key, value, root, nullptr);

		size += 1;
	}

	//delete the key:value pair from the map
	void remove(Key key)
	{
		Node* removable = find_node(key, root);
		
		remove(removable);

		size -= 1;
	}

	//delete all key::value pairs from the map
	void clear()
	{
		clear(root);
		
		root = nullptr;
		size = 0;
	}

	//print the map into the console in keys alphabetical order
	void print()
	{
		in_order_traverse_print(root);
	}

	void update(Key key, Val new_value)
	{
		Node* rewritable = find_node(key, root);
		rewritable->value = new_value;
	}

	//return all map's keys as an alphabetically sorted list
	List<Key> get_keys()
	{
		List<Key> lst;
		if(root != nullptr)
			in_order_traverse_keys(root, lst);

		return lst;
	}

	//return all map's values as list
	List<Val> get_values()
	{
		List<Val> lst;
		
		if (root != nullptr)
			in_order_traverse_values(root, lst);

		return lst;
	}

	Val find(Key key)
	{
		return find(key, root);
	}

	bool contains(Key key)
	{
		if (root != nullptr)
			return contains(key, root);
		else
			return false;
	}

	size_t getSize()
	{
		return size;
	}
};

