//
//  binary_tree.hpp
//  binary_tree
//
//  Created by Нечаев Антон on 30.09.2023.
//

#pragma once
#include <memory>
#include <assert.h>

template<typename T>
class binary_tree
{
  struct node
  {
      node() = default;
      
      node(const T& data_)
        :node(data_, nullptr)
      {
          
      }
      
      node(const T& data_, std::shared_ptr<node> parent_)
        :data(data_),
        parent(parent_)
      {
            
      }
      
      ~node()
      {
          parent = nullptr;
          left = nullptr;
          right = nullptr;
      }
      
      T data{};
      
      std::shared_ptr<node> parent = nullptr;
      std::shared_ptr<node> left = nullptr;
      std::shared_ptr<node> right = nullptr;
  };
    
public:
    class iterator
    {
        friend class binary_tree;
        
    public:
        iterator operator ++ () //  Префикс
        {
            my_node = findNext();
            return *this;
        }
        
        iterator operator ++ (int)  //  Постфикс
        {
            auto old = *this;
            my_node = findNext();
            return old;
        }
        
        bool operator != (const iterator& other_)
        {
            return my_node.lock() != other_.my_node.lock();
        }
        
        T& operator * ()
        {
            if(my_node.expired())
                throw;
            
            return my_node.lock()->data;
        }
        
    protected:
        iterator(std::weak_ptr<node> node_)
          :my_node(node_)
        {
            
        }
        
    private:
        std::weak_ptr<node> findNext() const
        {
            if (my_node.expired())
                return std::shared_ptr<node>(nullptr);
            
            auto live_node = my_node.lock();
            if (live_node->right)
            {
                auto temp = live_node->right;
                while (temp->left)
                {
                    temp = temp->left;
                }
                
                return temp;
            }
            else
            {
                //  Иначе поднимаемся до родителя до тех пор, пока не найдём родителя больше
                auto parent = live_node->parent;
                while (parent && parent->data < live_node->data)
                {
                    if (parent->parent)
                        parent = parent->parent;
                    else
                        parent = std::shared_ptr<node>(nullptr);
                }
                
                return parent;
            }
        }
        
    private:
        std::weak_ptr<node> my_node;
    };
    
public:
    binary_tree()
    {
        
    }
    
    ~binary_tree()
    {
        root = nullptr;
    }
    
    iterator begin()
    {
        auto temp = root;
        while (temp->left)
        {
            temp = temp->left;
        }
        
        return iterator(temp);
    }
    
    iterator end()
    {
        return iterator(std::shared_ptr<node>(nullptr));
    }
    
    bool insert(const T& key_)
    {
        if (!root)
        {
            root = std::make_shared<node>(key_);
            return true;
        }
        return insert(root, key_);
    }
    
    void erase(const T& key_)
    {
        erase(root, key_);
    }
    
    bool contains(const T& key_) const
    {
        return find(root, key_);
    }
    
private:
    bool insert(std::shared_ptr<node> root_, const T& key_)
    {
        assert(root_);
        
        if (root_->data == key_)
            return false;
        
        else if (root_->data > key_)
        {
            if (root_->left)
                return insert(root_->left, key_);
            else
            {
                root_->left = std::make_shared<node>(key_, root_);
                return true;
            }
        }
        
        else if (root_->data < key_)
        {
            if (root_->right)
                return insert(root_->right, key_);
            else
            {
                root_->right = std::make_shared<node>(key_, root_);
                return true;
            }
        }
        
        return false;
    }
    
    void erase(std::shared_ptr<T> root_, const T& key_)
    {
        auto erased_node = find(root_, key_);
        if (!erased_node)
            return;
        
        auto parent = erased_node->parent;
        // Нет дочерних
        if (!erased_node->left && ! erased_node->right)
        {
            if (parent->left == this)
                parent->left = nullptr;
            else if (erased_node->parent->right == this)
                parent->right = nullptr;
        }
        // Есть оба ребёнка
        else if (erased_node->left && erased_node->right)
        {
            auto replace = erased_node->right;
            while (replace->left)
            {
                replace = replace->left;
            }
            
            //  Оторвём
            if (replace->parent->left == replace)
                replace->parent->left = nullptr;
            else if (replace->parent->right == replace)
                replace->parent->right = nullptr;
            
            replace->parent = parent;
            
            if (parent->left == erased_node)
                parent->left = replace;
            else if (parent->right == erased_node)
                parent->right = replace;
            
            replace->left = erased_node->left;
            replace->right = erased_node->right;
        }
        // Есть только левый или только правый
        else if(erased_node->left)
        {
            if (parent->left == erased_node)
                parent->left = erased_node->left;
            else
                parent->right = erased_node->left;
        }
        else if(erased_node->right)
        {
            if (parent->left == erased_node)
                parent->left = erased_node->right;
            else
                parent->right = erased_node->right;
        }
    }
    
    std::shared_ptr<node> find(std::shared_ptr<node> root_, const T& key_) const
    {
        if(!root_)
            return nullptr;
        
        if (root_->data == key_)
            return root;
        else if(root_->data < key_)
            return find(root_->left, key_);
        else if(root_->data > key_)
            return find(root_->right, key_);
    }
    
private:
    std::shared_ptr<node> root = nullptr;
};
