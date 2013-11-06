/*
 * Copyright (c) 2012 Michael Shafae.
 * All rights reserved.
 *
 * $Id: Tree.h 4533 2013-11-06 06:27:35Z mshafae $
 *
 * This is a header file for a naïve Binary Search Tree class.
 *
 * Based in part on Introduction to Algorithms, 3rd Ed. by Cormen et al.
 *
 */

#ifndef _TREE_H_
#define _TREE_H_

#include "TreeNode.h"
#include <cstdlib>
#include <cassert>
#include <iostream>

/**
 * A naïve, templated binary search tree class.
 * Requires the TreeNode class.
 */
template <class T, class U>
class Tree{
public:
 /**
  * Tree constructor.
  * Initializes an empty tree.
  */
  explicit Tree( ) : _root( NULL ) { }

 /**
  * Tree deconstructor.
  * If the tree is not empty, it removes all the tree's nodes.
  */
  ~Tree( ){
    if( _root ){
      TreeNode<T, U>* n = _root->left( );
      if( n ){
        trim( n );
      }
      n = _root->right( );
      if( n ){
        trim( n );
      }
      delete _root;
    }
  }

 /**
  * Check if the tree is empty.
  * @return True if empty, False otherwise.
  */
  bool isEmpty( ) const{
    // The return value
    bool rv = false;
    if( _root == NULL ){
      rv = true;
    }
    return( rv );
  }
 
  /**
   * Insert data into the tree.
   * @param key The key to be inserted into the tree.
   */
   void insert( T& key, U& value ){
     if( find_iterative( _root, key ) ){
       std::cerr << "key already inserted - ignored." << std::endl;
     }else{
       std::cerr << "inserting " << key << std::endl;
       TreeNode<T, U>* x = _root;
       TreeNode<T, U>* y = NULL;
       while( x != NULL ){
         y = x;
         if( x->key( ) > key ){
           x = x->left( );
         }else{
           x = x->right( );
         }
       }
       TreeNode<T, U>* n = new TreeNode<T, U>( y, key, value );
       if( y == NULL ){
         _root = n;
       }else if( y->key( ) > key ){
         y->setLeft( n );
       }else{
         y->setRight( n );
       }
     }
   }

   void insert_recursive( T& key, U& value ){
     if( find_iterative( _root, key ) ){
       std::cerr << "key already inserted - ignored." << std::endl;
     }else{
       if( ! _root ){
	 _root = new TreeNode<T, U>( key );
       }else if( _root->key( ) <= key ){
	 if( _root->right( ) ){
	   insertHelper( _root->right( ), key );
	 }else{
	   _root->setRight( new TreeNode<T, U>( _root, key, value ) );
	 }
       }else{
	 if( _root->left( ) ){
	   insertHelper( _root->left( ), key );
	 }else{
	   _root->setLeft( new TreeNode<T, U>( _root, key, value ) );
	 }
       }
     }
   }

  /**
   * Delete the specified key from the tree.
   * @param key The key to be removed from the tree.
   * @return True if key exists and was removed, false otherwise.
   */
   bool remove( T key ){
     bool ret = false;
     TreeNode<T, U>* n = find_iterative( _root, key );     
     if( !n ){
       ret = false;
     }else{
       ret = true;
       deleteNode( n );
     }
     return( ret );
   }
   
 /**
  * Write the contents of the tree to the identified ostream.
  * @param out A reference to a desired ostream - can be cout.
  */
  std::ostream& write( std::ostream& out ){
    out << "digraph BST{\n";
    out << "\tnode [fontname=\"Helvetica\"];\n";
    if( ! _root ){
      out << "\n";
    }else if( ! _root->right( ) && ! _root->left( ) ){
      out << "\t" << _root->key( ) << ";\n";
    }else{
      writeHelper( _root, out );
    }
    out << "}\n";
    return(out);
  }
  
  std::ostream& writeLinks( std::ostream& out ){
    if( _root ){
      out << _root << std::endl;
      writeLinksHelper( out, _root->left( ) );
      writeLinksHelper( out, _root->right( ) );
    }else{
      out << "Tree empty" << std::endl;
    }
    return( out );
  }

  TreeNode<T, U>* minimum( ){
    return( local_minimum( _root ) );
  }
  
  TreeNode<T, U>* maximum( ){
    return( local_maximum( _root ) );
  }

  bool hasKey( T key ){
    return(find_iterative( _root, key ));
  }

  T inorderSuccessorTest( T key ){
    TreeNode<T, U>* x = find_iterative( _root, key );
    TreeNode<T, U>* y = inorderSuccessor( x );
    return(y->key( ));
  }

private:
 /**
  * Private pointer to the root of the tree.
  */
  TreeNode<T, U>* _root;
  
  TreeNode<T, U>* find_recursive( TreeNode<T, U>* t, T key ){
    TreeNode<T, U>* ret = NULL;
    T k;
    if( t == NULL || (k = t->key( )) == key ){
      ret = t;
    }else if( key < k ){
      ret = find_recursive( t->left( ), key );
    }else{
      // if key > k
      ret = find_recursive( t->right( ), key );
    }
    return( ret );
  }
  
  TreeNode<T, U>* find_iterative( TreeNode<T, U>* t, T key ){
    while( t != NULL && t->key( ) != key ){
      if( key > t->key( ) ){
        t = t->right( );
      }else{
        t = t->left( );
      }
    }
    return( t );
  }
  
  TreeNode<T, U>* inorderSuccessor( TreeNode<T, U>* n ){
    TreeNode<T, U>* ios = NULL;
    TreeNode<T, U>* tmp = NULL;
    if( n->right( ) != NULL ){
      ios = local_minimum( n->right( ) );
      assert( ios );
    }else{
      ios = n->parent( );
      tmp = n;
      while( ios != NULL && tmp == ios->right( ) ){
        tmp = ios;
        ios = ios->parent( );
      }
      if( ios == NULL ){
	// if ios is null then ios is the tree's max.
	// return (the original) n as the ios.
        ios = n;
      }
    }
    return( ios );
  }
  
  void transplant( TreeNode<T, U>* u, TreeNode<T, U>* v ){
    if( u->parent( ) == NULL ){
      _root = v;
    }else if( u == u->parent()->left() ){
      u->parent()->setLeft( v );
    }else{
      u->parent()->setRight(v);
    }
    if( v != NULL ){
      v->setParent( u->parent( ) );
    }
  }

  void deleteNode( TreeNode<T, U>* n ){
    if( n->left( ) == NULL ){
      transplant( n, n->right() );
    }else if( n->right( ) == NULL ){
      transplant( n, n->left( ) );
    }else{
      TreeNode<T, U>* y = local_minimum( n->right( ) );
      if( y->parent( ) != n ){
        transplant( y, y->right( ) );
        y->setRight( n->right( ) );
        y->right( )->setParent( y );
      }
      transplant( n, y );
      y->setLeft( n->left( ) );
      y->left( )->setParent( y );
    }
    delete n;
  }

 // BROKEN!!! try to remove the inorder predessecor
 // of the root node and it breaks
 // and it doesn't free the memory!!
  void deleteNodeX( TreeNode<T, U>* n ){
    TreeNode<T, U>* x;
    TreeNode<T, U>* y;
    if( n->isLeafNode( ) ){
      y = n;
    }else{
      y = inorderSuccessor( n );
    }
    if( y->left( ) != NULL ){
      x = y->left( );
    }else{
      x = y->right( );
    }
    if( x != NULL ){
      x->setParent( y->parent( ) );
    }
    if( y->parent( ) == NULL ){
      _root = x;
    }else if( y->isLeftChildOfParent( ) ){
      y->parent( )->setLeft( x );
    }else{
      y->parent( )->setRight( x );
    }
    if( y != n ){
      n->setKey( y->key( ) );
    }
  }
  
  void writeLinksHelper( std::ostream& out, TreeNode<T, U>* n ){
    if( n ){
      out << n << std::endl;
      writeLinksHelper( out, n->left( ) );
      writeLinksHelper( out, n->right( ) );
    }
  }

  void trim( TreeNode<T, U>* n ){
    TreeNode<T, U>* l = n->left( );
    TreeNode<T, U>* r = n->right( );
    if( l ){
      trim( l );
      delete l;
    }
    if( r ){
      trim( r );
      delete r;
    }
  }

  void insertHelper( TreeNode<T, U>* n, T key ){
    if( n->key( ) <= key ){
      if( n->right( ) ){
        insertHelper( n->right( ), key );
      }else{
        n->setRight( new TreeNode<T, U>( n, key ) );
      }
    }else{
      if( n->left( ) ){
        insertHelper( n->left( ), key );
      }else{
        n->setLeft( new TreeNode<T, U>( n, key ) );
      }
    }
  }

  void writeNull(TreeNode<T, U>* n, int nullcount, std::ostream& out ){
    out << "\tnull" << nullcount << " [shape=point];\n";
    out << "\t" << n->key( ) << " -> null" << nullcount << ";\n";
  }

  void writeHelper( TreeNode<T, U>* n, std::ostream& out ){
    static int nullcount = 0;
    if( n->left( ) ){
      out << "\t" << n->key( ) << " -> " << n->left( )->key( ) << ";\n";
      writeHelper(n->left( ), out );
    }else{
      writeNull(n, nullcount++, out );
    }
    if( n->right( ) ){
      out << "\t" << n->key( ) << " -> " << n->right( )->key( ) << ";\n";
      writeHelper(n->right( ), out );
    }else{
      writeNull( n, nullcount++, out );
    }
  }

  TreeNode<T, U>* local_minimum( TreeNode<T, U>* r ){
    TreeNode<T, U>* n = r;
    while( n->left( ) != NULL ){
      n = n->left( );
    }
    return( n );
  }
  
  TreeNode<T, U>* local_maximum( TreeNode<T, U>* r ){
    TreeNode<T, U>* n = r;
    while( n->right( ) != NULL ){
      n = n->right( );
    }
    return( n );
  }

};

template <class T, class U>
std::ostream& operator <<( std::ostream& out, Tree<T, U>& t ){
  t.writeLinks( out );
  return(out);
}

#endif
