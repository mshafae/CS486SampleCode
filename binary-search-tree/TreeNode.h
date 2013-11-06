/*
 * Copyright (c) 2012 Michael Shafae.
 * All rights reserved.
 *
 * $Id: TreeNode.h 4533 2013-11-06 06:27:35Z mshafae $
 *
 * This is a header file for a `TreeNode' class for use with a
 * naïve binary search tree.
 * 
 * Based in part on Introduction to Algorithms, 3rd Ed. by Cormen et al.
 *
 */


#ifndef _TREENODE_H_
#define _TREENODE_H_

#include <cstdlib>
#include <iostream>
#include <cassert>

/**
 * A naïve, templated tree node class.
 * This tree node class is for use with the
 * Binary Search Tree data structure.
 */
template <class T, class U>
class TreeNode{
public:
  /**
   * TreeNode constructor.
   * @param key The key of the node
   */
   explicit TreeNode( const T& key, const U& value ) :
   _key(key), _value(value), _left(NULL), _right(NULL), _parent(NULL) { };
  
 /**
  * TreeNode constructor.
  * @param parent The parent node of the node being constructed.
  * @param key The key of the node.
  */
  explicit TreeNode( TreeNode<T, U>* parent, const T& key, const U& value )  :
  _key(key), _value(value), _left(NULL), _right(NULL), _parent(parent) { };

  /**
   * TreeNode deconstructor.
   * No memory allocated; does nothing.
   */
  ~TreeNode( ){};

  /**
   * Returned the key of the tree node object.
   * @return The value of _key.
   */
  T key( ) const{
    return(_key);
  }

  /**
   * Set the value of the tree node's key.
   * @param key The key of the tree node.
   */
  void setKey( const T& key ){
    _key = key;
  }

 /**
  * Returned the value of the tree node object.
  * @return The value of _value.
  */
 U value( ) const{
   return(_value);
 }

 /**
  * Set the value of the tree node's value.
  * @param value The value of the tree node.
  */
 void setValue( const U& value ){
   _value = value;
 }

  /**
   * Return the tree node pointed to by the left pointer.
   * @return The value of _left.
   */
  TreeNode<T, U>* left( ){
    return(_left);
  }

  /**
   * Return the tree node pointed to by the right pointer.
   * @return The value of _right.
   */
  TreeNode<T, U>* right( ){
    return(_right);
  }

  /**
   * Return the tree node pointed to by the parent pointer.
   * @return The value of _parent.
   */
  TreeNode<T, U>* parent( ){
    return(_parent);
  }

  /**
   * Set the value of the tree node's left pointer.
   * @param left The pointer to the desired left tree node.
   * @return The this pointer is returned.
   */
  TreeNode<T, U>* setLeft( TreeNode<T, U>* left ){
    _left = left;
    return(this);
  }

  /**
   * Set the value of the tree node's right pointer.
   * @param right The pointer to the desired right tree node.
   * @return The this pointer is returned.
   */
  TreeNode<T, U>* setRight( TreeNode<T, U>* right ){
    _right = right;
    return(this);
  }

  /**
   * Set the value of the tree node's parent pointer
   * @param parent The pointer to the desiered parent tree node.
   * @return The this pointer is returned.
   */
  TreeNode<T, U>* setParent( TreeNode<T, U>* parent ){
    _parent = parent;
    return(this);
  }

  /**
   * Write the contents of the tree node to the identified ostream.
   * @param out A reference to a desired ostream - can be cout.
   * @return The reference to the incoming ostream is returned.
   */
   std::ostream& write( std::ostream &out ) const{
        out << _key << " (" << (void*)this << ")" << std::endl;
        out << "\tleft(" << (void*)_left << ") right(" <<
	  (void*)_right << ") parent(" << (void*)_parent << ")" << std::endl;
        return( out );
   }

  /**
   * Determine if this is a leaf node or not.
   * @return True if the node is a leaf node, false otherwise.
   */   
   bool isLeafNode( ) const  {
     bool rv = false;
     if( _left == NULL && _right == NULL ){
       rv = true;
     }
     return( rv );
   }

  /**
   * Determine if this is a node with one child or not.
   * @return True if the node has one and only one child, false otherwise.
   */   
   bool isNodeWithOneChild( ) const{
     bool rv = false;
     if( _left == NULL && _right != NULL ){
       rv = true;
     }else if( _left != NULL && _right == NULL ){
       rv = true;
     }
     return( rv );
   }

  /**
   * Determine if this is a node with two children or not.
   * @return True if the node has two children, false otherwise.
   */   
   bool isNodeWithTwoChildren( ) const{
     bool rv = false;
     if( _left != NULL && _right != NULL ){
       rv = true;
     }
     return( rv );
   }

  /**
   * Determine if this is the left child of this's parent or not.
   * @return True if the node is the left child of it's parent, false otherwise.
   */   
   bool isLeftChildOfParent( ) const{
     bool rv = false;
     assert( _parent );
     if( _parent->left( ) == this ){
       rv = true;
     }
     return( rv );
   }

  /**
   * Determine if this is the left child of this' parent or not.
   * @return True if the node is the right child of it's parent, false otherwise.
   */   
   bool isRightChildOfParent( ) const{
     bool rv = false;
     assert( _parent );
     if( _parent->right( ) == this ){
       rv = true;
     }
     return( rv );
   }

  /**
   * Determine if this is the only child of it's parent.
   * @return True if the node is the only child of it's parent, false otherwise.
   */   
   TreeNode<T, U>* onlyChild( ) const{
     TreeNode<T, U>* rv = NULL;
     if( _left == NULL && _right != NULL ){
       rv = _right;
     }else if( _left != NULL && _right == NULL ){
       rv = _left;
     }
     return( rv );
   }
 
private:
 /**
  * TreeNode's key; private templated data member.
  */
  T _key;

 /**
  * TreeNode's data payload; private templated data member.
  */
  U _value;

 /**
  * TreeNode's pointer to the left TreeNode; private data member.
  */
  TreeNode<T, U>* _left;

 /**
  * TreeNode's pointer to the right TreeNode; private data member.
  */
  TreeNode<T, U>* _right;

 /**
  * TreeNode's parent pointer; private data member.
  */
  TreeNode<T, U>* _parent;
};


template <class T, class U>
std::ostream& operator <<( std::ostream& out, TreeNode<T, U>* n ){
  n->write( out );
  return(out);
}


#endif
