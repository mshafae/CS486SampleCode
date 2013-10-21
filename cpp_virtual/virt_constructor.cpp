// $Id$

#include <cstdio>

class foo{
public:
 foo( ){};
 virtual ~foo( ){printf("hi I'm in a!\n");};
};

class bar:public foo{
public:
  bar( ):foo(){};
  ~bar( ){printf( "hi there, I'm in b!\n" );};
};

int main( void ){
	foo* f = new bar( );
	delete f;
	return(0);
}
