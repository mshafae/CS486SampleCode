/*
 * Copyright (c) 2011 Michael Shafae.
 * All rights reserved.
 *
 * $Id: Tree_driver.cpp 4533 2013-11-06 06:27:35Z mshafae $
 *
 * Driver/Demonstration program for the Tree class
 *
 */

#include "Tree.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

class BRNG{
  public:
    BRNG( unsigned int seed = 1 ){
      srand( seed );
    }
    ptrdiff_t operator()(ptrdiff_t max) {
        double tmp;
        tmp = static_cast<double>(rand())
                / static_cast<double>(RAND_MAX);
        return static_cast<ptrdiff_t>(tmp * max);
    }
};

int last = 10;
int _rand( ){
  int a = 6;
  int m = 59;
  int next = (last * a) % m;
  last = next;
  return( next );
}

void empty( Tree<unsigned int, unsigned int>& t ){
  if( t.isEmpty( ) ){
    cout << "Tree is empty" << endl;
  }else{
    cout << "Tree is not empty" << endl;
  }
}

void insert( Tree<unsigned int, unsigned int>& t, vector<unsigned int>& l, int numKeys ){
  for( int i = 0; i < numKeys; i++ ){
    //unsigned int x = rand( );
    unsigned int x = l[i];
    cout << "inserting " << x << endl;
    //l.push_back( x );
    t.insert( x, x );
    /*if( i % 10000000 == 0 ){
      printf( "%d\n", i );
    }*/
  }
  cout << "Inserted " << numKeys << " keys" << endl;
}

void find( Tree<unsigned int, unsigned int>& t, vector<unsigned int>& l, int numKeys ){
  for( int i = 0; i < numKeys; i++ ){
    int key = l[i];
    cout << "Looking for " << key << ": ";
    if( t.hasKey(key) ){
      cout << "Found" << endl;
    }else{
      cout << "Not Found" << endl;
    }
  }
}

#define DEL
void del( Tree<unsigned int, unsigned int>& t, vector<unsigned int>& l, int numKeys ){
#ifdef DEL
  int i = 0;
  while( i < numKeys ){
    unsigned int x = l.back( );
    l.pop_back( );
    cout << "trying " << x << endl;
    if( t.remove( x ) ){
      cout << "Removed " << x << endl;
    }else{
      cout << "not found! " << x << endl;
    }
    i++;
  }
  cout << "Deleted " << numKeys << " keys" << endl;
#endif
}

#define REMOVE
void writeGraphViz( Tree<unsigned int, unsigned int>& t, const char* fname ){
  char buffer[255];
  pid_t child;
  sprintf( buffer, "%s.dot", fname );
  ofstream f( buffer );
  t.write( f );
  f.close( );
  sprintf( buffer, "dot -Tpdf %s.dot -o %s.pdf", fname, fname );
  child = fork( );
  if( child == 0 ){
    system( buffer );
#ifdef REMOVE
    sprintf( buffer, "%s.dot", fname);
    remove( buffer );
#endif
    exit(0);
  }
}

void writeLinks( Tree<unsigned int, unsigned int>& t, const char* fname ){
  ofstream f( fname );
  t.writeLinks( f );
  f.close( );
}

/*void insert_find_test( int numKeys ){
  Tree<unsigned int> t;
  vector<unsigned int> l;
  //BRNG rng(time(NULL));
  BRNG rng(1);
  srand(time(NULL));
  empty( t );

  insert( t, l, numKeys );
  find( t, l, numKeys );
  //  insert( t, l, numKeys );
  //find( t, l, numKeys );
}*/

/*void inorder_test( int numKeys ){
  Tree<unsigned int> t;
  vector<unsigned int> l;
  insert( t, l, numKeys );
  sort(l.begin(), l.end());
  copy( l.begin( ), l.end( ), ostream_iterator<unsigned int>(cout, " ") );
  cout << endl;
  random_shuffle( l.begin( ), l.end( ) );
  for( int i = 0; i < numKeys; i++ ){
    cout << "The inorder successor of " << l[i] << " is " << t.inorderSuccessorTest( l[i] );
    if( l[i] < t.inorderSuccessorTest( l[i] ) ){
      cout << endl;
    }else{
      cout << " WARNING!! l[i] >= ios(l[i])" << endl;
    }
  }
  cout << "Let's try again..." << endl;
  random_shuffle( l.begin( ), l.end( ) );
  for( int i = 0; i < numKeys; i++ ){
    cout << "The inorder successor of " << l[i] << " is " << t.inorderSuccessorTest( l[i] ) << endl;
  }
  writeGraphViz( t, "treedump-is" );
  writeLinks( t, "links-is.txt" );
}*/

void insertion_deletion_test( int numKeys ){
  Tree<unsigned int, unsigned int> t;
  vector<unsigned int> l;
  time_t now = time(NULL);
  cout << now << endl;
  BRNG rng(now);
  srand(now);
  // The two cases below break the inorderSuccessor thing
  //BRNG rng(1335157485);
  //srand(1335157485);
  //BRNG rng(1335157607);
  //srand(1335157607);
  empty( t );

  for( int i = 0; i < numKeys; i++){
    l.push_back( i );
  }
  random_shuffle( l.begin( ), l.end(), rng );
  
  insert( t, l, numKeys );

  random_shuffle( l.begin( ), l.end(), rng );
  //copy(l.begin(), l.end(), std::ostream_iterator<unsigned int>(std::cout, "\n") );

  cout << "Writing dump 1\n";
  writeGraphViz( t, "treedump-1" );
  writeLinks( t, "links-1.txt" );

  del( t, l, numKeys / 3 );
  empty( t );
  cout << "Writing dump 2\n";
  writeGraphViz( t, "treedump-2" );
  writeLinks( t, "links-2.txt" );

  del( t, l, numKeys / 3 );
  empty( t );
  cout << "Writing dump 3\n";
  writeGraphViz( t, "treedump-3" );
  writeLinks( t, "links-3.txt" );
  
  /*insert( t, l, numKeys / 3 );
  random_shuffle( l.begin( ), l.end(), rng );
  cout << "Writing dump 4\n";
  writeGraphViz( t, "treedump-4" );
  writeLinks( t, "links-4.txt" );*/

  del( t, l, numKeys / 3 );
  empty( t );
  cout << "Writing dump 5\n";
  writeGraphViz( t, "treedump-5" );
  writeLinks( t, "links-5.txt" );
}

void create_delete_test( int numKeys ){
  Tree<unsigned int, unsigned int> *t = new Tree<unsigned int, unsigned int>( );
  vector<unsigned int> l;
  //BRNG rng(time(NULL));
  BRNG rng(1);
  srand(time(NULL));

  empty( *t );
  insert( *t, l, numKeys );
  delete t;
}

int main( int argc, char** argv ){
  int numKeys;
  if( argc < 2 ){
    cout << "Provide an argument for how many keys to insert.\n";
    exit(1);
  }else{
    numKeys = atoi( argv[1] );
    if( numKeys < 3 ){
      cout << "Come on; input something larger than 7 but less than 59.\n";
      exit(1);
    }
  }

  //rotation_test( );
  //insert_find_test( numKeys );
  //inorder_test( numKeys );
  insertion_deletion_test( numKeys );
  //create_delete_test(numKeys);
  return(0);
}
