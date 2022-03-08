#pragma once
#include <initializer_list>
#include <algorithm>
#include <stdexcept>

template<class ValueType> class Set 
{
    private:
        struct Node;

    public:
        class iterator 
        {
            friend Set;

            public:
                iterator();

                iterator operator++();
                iterator operator++(int); 
                iterator operator--();
                iterator operator--(int); 

                ValueType operator*() const;
                ValueType* operator->() const;

                bool operator==(const iterator& o) const;
                bool operator!=(const iterator& o) const;

            private:
                iterator(Node* node, bool is_end = false);

            private:
                Node* _node = nullptr;
                bool _is_end = false;
        };

    public:
        Set() = default;
        Set(const Set& other);
        Set(Set&& other) = delete;
        Set(std::initializer_list<ValueType> init);
        template<class InputIt> Set(InputIt first, InputIt last);
        ~Set();

        Set& operator=(const Set& other); 

        iterator find(const ValueType& value) const;
        iterator lower_bound(const ValueType& value) const;
        size_t size() const;
        bool empty() const;
        iterator begin() const;
        iterator end() const;

        void insert(const ValueType& value);
        void erase(const ValueType& value);
        void clear();

    private:
        struct Node
        {
            public:
                Node(const ValueType& value);

                Node* rotate_left();
                Node* big_rotate_left();
                Node* rotate_right();
                Node* big_rotate_right();

                Node* apply_rotation();

            public:
                ValueType value;

                Node* left = nullptr;
                Node* right = nullptr;
                Node* parent = nullptr;
                int8_t balance = 0; 
        };  

    private:
        void update_edge_nodes();

    private:

       Node* _root = nullptr; 
       Node* _begin_node = nullptr;
       Node* _last_node = nullptr;
       size_t _size = 0;
};



template<class ValueType>
Set<ValueType>::Set(const Set& other) {
    for(const auto& value : other) {
        insert(value);
    } 
}


template<class ValueType>
Set<ValueType>::Set(std::initializer_list<ValueType> init) {
    for(const auto& value : init) {
        insert(value);
    }
}

template<class ValueType>
template<class InputIt> Set<ValueType>::Set(InputIt first, InputIt last) {
    while(first != last) {
        insert(*(first++));
    }
}

template<class ValueType>
Set<ValueType>::~Set() {
    clear();
}

template<class ValueType>
Set<ValueType>& Set<ValueType>::operator=(const Set& other)  {
    if(this == &other) {
        return *this;
    }

    clear();

    for(const auto& value : other) {
        insert(value);
    }

    return *this;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::find(const ValueType& value) const {
    Node* current = _root;

    while(current != nullptr) {
        if(current->value < value) {
            current = current->right;
        } else if(value < current->value) {
            current = current->left;
        } else {
            return iterator(current);
        }
    }

    return end();
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::lower_bound(const ValueType& value) const {
    Node* current = _root;
    Node* last_greater = nullptr;

    while(current != nullptr) {
        if(current->value < value) {
            current = current->right;
        } else if(value < current->value) {
            last_greater = current;
            current = current->left;
        } else {
            return iterator(current);
        }
    }

    if(last_greater == nullptr) {
        return end();
    }

    return iterator(last_greater);
}

template<class ValueType>
size_t Set<ValueType>::size() const {
    return _size;
}

template<class ValueType>
bool Set<ValueType>::empty() const  {
    return _size == 0;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::begin() const {
    return iterator(_begin_node, _begin_node == nullptr);
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::end() const {
    return iterator(_last_node, true);
}

template<class ValueType>
void Set<ValueType>::insert(const ValueType& value) {
    if(_root == nullptr) {
        _root = _begin_node = _last_node = new Node(value);
        ++_size;
        return;
    }

    Node* current = _root;

    while(current != nullptr) {
        if(current->value < value) {
            if(current->right == nullptr) {
                current->right = new Node(value);
                current->right->parent = current;
                --(current->balance);
                break;
            } 

            current = current->right;
        } else if(value < current->value) {
            if(current->left == nullptr) {
                current->left = new Node(value);
                current->left->parent = current;
                ++(current->balance);
                break;
            }

            current = current->left;
        } else {
            return;
        }
    }

    
    while(current != nullptr) {
        current = current->apply_rotation();

        if(current->parent == nullptr) {
            _root = current;
        }

        if(current->balance == 0) {
            break;
        } 

        if(current->parent != nullptr) {
            if(current == current->parent->left) {
                ++(current->parent->balance);
            } else {
                --(current->parent->balance);
            }
        }

        current = current->parent;
    }

    ++_size;
    update_edge_nodes();
}

template<class ValueType>
void Set<ValueType>::erase(const ValueType& value) {
    Node* current = _root;

    while(current != nullptr) {
        if(current->value < value) {
            current = current->right;
        } else if(value < current->value) {
            current = current->left;
        } else {
            break;
        }
    }

    if(current == nullptr) {
        return;
    }

    --_size;

    while(true) {
        if(current->left == nullptr && current->right == nullptr) {
            if(current->parent == nullptr) {
                delete _root;
                _root = nullptr;
                current = nullptr;
            } else {
                if(current->parent->right == current) {
                    current = current->parent;
                    delete current->right;
                    current->right = nullptr;
                    ++(current->balance);
                } else {
                    current = current->parent;
                    delete current->left;
                    current->left = nullptr;
                    --(current->balance);
                }
            }

            break;
        } else if(current->left != nullptr) {
            Node* nearest = current->left;

            while(nearest->right != nullptr) {
                nearest = nearest->right;
            }

            std::swap(current->value, nearest->value);
            current = nearest;
        } else if(current->right != nullptr) {
            Node* nearest = current->right;

            while(nearest->left != nullptr) {
                nearest = nearest->left;
            }

            std::swap(current->value, nearest->value);
            current = nearest;
        } 
         
    }

    while(current != nullptr) {
        current = current->apply_rotation();

        if(current->parent == nullptr) {
            _root = current;
        }

        if(abs(current->balance) == 1) {
            break;
        }

        if(current->parent != nullptr) {
            if(current == current->parent->left) {
                --(current->parent->balance);
            } else {
                ++(current->parent->balance);
            }
        }

        current = current->parent;
    }

    update_edge_nodes();
}

template<class ValueType>
void Set<ValueType>::clear() {
    Node* current = _root;  

    while(current != nullptr) {
        if(current->left != nullptr) {
            current = current->left;
        } else if(current->right != nullptr) {
            current = current->right;
        } else {
            Node* parent = current->parent;

            if(parent == nullptr) {
                delete current;
                break;
            }

            if(parent->right == current) {
                parent->right = nullptr;
            } else {
                parent->left = nullptr;
            }

            delete current;

            current = parent; 
        }
    } 

    _size = 0;
    _root = _begin_node = _last_node = nullptr;
}

template<class ValueType>
void Set<ValueType>::update_edge_nodes() {
    _begin_node = _last_node = _root;

    if(_root == nullptr) {
        return;
    }

    while(_begin_node->left != nullptr) {
        _begin_node = _begin_node->left;
    }

    while(_last_node->right != nullptr) {
        _last_node = _last_node->right;
    }
}

template<class ValueType>
Set<ValueType>::iterator::iterator() : _node(nullptr), _is_end(false) { }

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::iterator::operator++() {
    if(_node == nullptr || _is_end) {
        return *this;
    }

    Node* node_copy = _node;

    if(_node->right == nullptr) {
        while(true) {
            if(_node->parent == nullptr) {
                _is_end = true;
                _node = node_copy;
                break;
            } else if(_node->parent->right == _node) {
                _node = _node->parent;
            } else {
                _node = _node->parent;
                break;
            }
        } 
    } else {
        _node = _node->right;

        while(_node->left != nullptr) { 
            _node = _node->left;
        }
    }

    return *this; 
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::iterator::operator++(int)  {
    Node* node_copy = _node;

    ++(*this);

    return iterator(node_copy);
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::iterator::operator--() {
    if(_node == nullptr) {
        return *this;
    }

    if(_is_end) {
        _is_end = false;
        return (*this);
    }

    if(_node->left == nullptr) {
        while(true) {
            if(_node->parent == nullptr) {
                _node = nullptr;
                break;
            } else if(_node->parent->left == _node) {
                _node = _node->parent;
            } else {
                _node = _node->parent;
                break;
            }
        } 
    } else {
        _node = _node->left;

        while(_node->right != nullptr) { 
            _node = _node->right;
        }
    }

    return *this;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::iterator::operator--(int)  {
    Node* node_copy = _node;

    --(*this);

    return iterator(node_copy);
}

template<class ValueType>
ValueType Set<ValueType>::iterator::operator*() const {
    if(_is_end) {
        throw std::domain_error("Attempt to dereference end iterator");
    }

    return (_node->value);
}

template<class ValueType>
ValueType* Set<ValueType>::iterator::operator->() const {
    if(_is_end) {
        throw std::domain_error("Attempt to dereference end iterator");
    }

    return &(_node->value);
}

template<class ValueType>
bool Set<ValueType>::iterator::operator==(const iterator& o) const {
    return _node == o._node && _is_end == o._is_end;
}

template<class ValueType>
bool Set<ValueType>::iterator::operator!=(const iterator& o) const {
    return _node != o._node || _is_end != o._is_end;
}

template<class ValueType>
Set<ValueType>::iterator::iterator(Node* node, bool is_end) : _node(node), _is_end(is_end) { }

template<class ValueType>
Set<ValueType>::Node::Node(const ValueType& value) : value(value) {
    parent = left = right = nullptr;
    balance = 0;
} 

template<class ValueType>
typename Set<ValueType>::Node* Set<ValueType>::Node::rotate_left() {
    if(right->balance == 0) {
        balance = -1;
        right->balance = 1;
    } else {
        balance = right->balance = 0;
    }

    Node* new_root = right; 

    if(parent != nullptr) {
        if(parent->left == this) {
            parent->left = new_root;
        } else {
            parent->right = new_root;
        }
    }
    
    right = new_root->left;
    new_root->left = this;
    new_root->parent = parent;
    parent = new_root;

    if(right != nullptr) {
        right->parent = this;
    }

    return new_root;
}

template<class ValueType>
typename Set<ValueType>::Node* Set<ValueType>::Node::big_rotate_left() {
    int8_t was_new_root_balance = right->left->balance;

    right = right->rotate_right();

    Node* new_root = rotate_left();

    new_root->balance = 0;

    if(was_new_root_balance == 1) {
        new_root->left->balance = 0;
        new_root->right->balance = -1; 
    } else if(was_new_root_balance == -1) {
        new_root->left->balance = 1;
        new_root->right->balance = 0; 
    } else {
        new_root->left->balance = 0;
        new_root->right->balance = 0; 
    } 

    return new_root;
}

template<class ValueType>
typename Set<ValueType>::Node* Set<ValueType>::Node::rotate_right() {
    if(left->balance == 0) {
        balance = 1;
        left->balance = -1;
    } else {
        balance = left->balance = 0;
    }

    Node* new_root = left; 

    if(parent != nullptr) {
        if(parent->left == this) {
            parent->left = new_root;
        } else {
            parent->right = new_root;
        }
    }
    
    left = new_root->right;
    new_root->right = this;
    new_root->parent = parent;
    parent = new_root;

    if(left != nullptr) {
        left->parent = this;
    }
    
    return new_root;
}

template<class ValueType>
typename Set<ValueType>::Node* Set<ValueType>::Node::big_rotate_right() {
    int8_t was_new_root_balance = left->right->balance;

    left = left->rotate_left();

    Node* new_root = rotate_right();

    new_root->balance = 0;

    if(was_new_root_balance == 1) {
        new_root->right->balance = -1;
        new_root->left->balance = 0; 
    } else if(was_new_root_balance == -1) {
        new_root->right->balance = 0;
        new_root->left->balance = 1; 
    } else {
        new_root->right->balance = 0;
        new_root->left->balance = 0; 
    } 

    return new_root;
}

template<class ValueType>
typename Set<ValueType>::Node* Set<ValueType>::Node::apply_rotation() {
    if(balance == -2) {
        if(right->balance <= 0) {
            return rotate_left();
        } else {
            return big_rotate_left();
        } 
    } else if(balance == 2) {
        if(left->balance >= 0) {
            return rotate_right();
        } else {
            return big_rotate_right();
        }
    }

    return this;
}

