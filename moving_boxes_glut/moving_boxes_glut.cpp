// 
// Michael Shafae
// mshafae at fullerton.edu
// 
// A really simple GLUT demo that renders moving
// boxes with no collision detection or resolution.
//
// $Id: moving_boxes_glut.cpp 5397 2014-12-05 04:05:20Z mshafae $
//

#ifdef _WIN32
#include <Windows.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#define _USE_MATH_DEFINES
#include <cmath>
#define sprintf sprintf_s
#define strdup _strdup
#else
#include <cstdio>
#include <cstdlib>
#include <ctime>
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif

#ifndef NDEBUG
#define msglError( ) _msglError( stderr, __FILE__, __LINE__ )
#else
#define msglError( )
#endif

typedef struct rectangle{
  float center[3];
  float half_width;
  float half_height;
  float color[3];
  float translation[3];
  float velocity[3];
}rectangle_t;

#define NUMRECTS 10
rectangle_t* gRects[NUMRECTS];
int gTimer;
bool gPrintRectangleFlag;

void fcopy(float* src, float* dst, size_t n){
  for(size_t i = 0; i < n; i++){
    dst[i] = src[i];
  }
}

int matPrint4d(FILE *f, double *m){
  int i;
  int accum = 0;
  for( i = 0; i < 4; i++ ){
    accum += fprintf( f, "%.16f %.16f %.16f %.16f\n", m[i + 0], m[i + 4], m[i + 8], m[i + 12] );
  }
  return(accum);
}

void vecPerp2f(float* p, float* v){
  p[0] =  v[1];
  p[1] = -v[0];
}

void vecDifference3f(float *c, float *a, float *b){
  int i;
  for(i = 0; i < 3; i++){
    c[i] = a[i] - b[i];
  }
}

float vecDot3f(float *a, float *b){
  float accumulate = 0.0;
  int i;
  for(i = 0; i < 3; i++){
    accumulate += a[i] * b[i];
  }
  return( accumulate );
}

void vecScalarMult3f(float *b,float *a, float s){
  int i;
  for(i = 0; i < 3; i++){
    b[i] = a[i] * s;
  }
}

bool _msglError( FILE *out, const char *filename, int line ){
  bool ret = false;
  GLenum err = glGetError( );
  while( err != GL_NO_ERROR ) {
    ret = true;
    fprintf( out, "GL ERROR:%s:%d: %s\n",
      filename, line, (char*)gluErrorString( err ) );
    err = glGetError( );
  }
  return( ret );
}

void printTopOfBothStacks( char *msg ){
  double m[16];
  msglError( );
  if( msg != NULL ){
    fprintf( stderr, "%s\n", msg );
  }
  fprintf( stderr, "Top of Projection Stack:\n" );
  glGetDoublev( GL_PROJECTION_MATRIX, m );
  puts("proj");
  matPrint4d( stdout, m );
  fprintf( stderr, "\n\n" );
  glGetDoublev( GL_MODELVIEW_MATRIX, m );
  puts("mv");
  matPrint4d( stdout, m );
  msglError( );
}

float frand( ){
  float r = float(rand( )) / float(RAND_MAX);
  if( rand( ) % 2 ){
    r *= -1;
  }
  return r;
}

void printRectangle(int i, rectangle_t* r){
  printf("rectangle %d{\n", i);
  printf("\tcenter = {%g, %g, %g}\n", r->center[0], r->center[1], r->center[2]);
  printf("\thalf width = %g\n", r->half_width);
  printf("\thalf height = %g\n", r->half_height);
  printf("\tcolor = {%g, %g, %g}\n", r->color[0], r->color[1], r->color[2]);
  printf("\ttranslation = {%g, %g, %g}\n", r->translation[0], r->translation[1], r->translation[2]);
  printf("\tvelocity = {%g, %g, %g}\n", r->velocity[0], r->velocity[1], r->velocity[2]);
  printf("}\n");
}

void rectangleVertices(float v[12], rectangle_t* r){
  v[0] = r->center[0] + r->half_width + r->translation[0];
  v[1] = r->center[1] + r->half_height + r->translation[1];
  v[2] = r->center[2] + r->translation[2];
  
  v[3] = r->center[0] - r->half_width + r->translation[0];
  v[4] = r->center[1] + r->half_height + r->translation[1];
  v[5] = r->center[2] + r->translation[2];
  
  v[6] = r->center[0] - r->half_width + r->translation[0];
  v[7] = r->center[1] - r->half_height + r->translation[1];
  v[8] = r->center[2] + r->translation[2];
  
  v[9] = r->center[0] + r->half_width + r->translation[0];
  v[10] = r->center[1] - r->half_height + r->translation[1];
  v[11] = r->center[2] + r->translation[2];
}

//#define DIM 0.05
#define DIM 0.125
void initUnitSquare(rectangle_t* r, float* color){
  r->half_width = DIM * 0.5;
  r->half_height = DIM * 0.5;
  fcopy(color, r->color, 3);
  for(int i = 0; i < 2; i++){
    r->center[i] = 0.0;
    r->velocity[i] = frand( ) * 0.01;
    r->translation[i] = frand( );
  }
  r->center[2] = 0.0;
  r->velocity[2] = 0.0;
  r->translation[2] = 0.0;
}

void resetUnitSquare(rectangle_t* r){
  for(int i = 0; i < 2; i++){
    r->center[i] = 0.0;
    r->velocity[i] = frand( ) * 0.01;
    r->translation[i] = frand( );
  }
  r->center[2] = 0.0;
  r->velocity[2] = 0.0;
  r->translation[2] = 0.0;
}

void updateRectangle(rectangle_t* r){
  for(int i = 0; i < 3; i++){
    r->translation[i] += r->velocity[i];
    if(r->translation[i] > 1.0 || r->translation[i] < -1.0){
      r->velocity[i] *= -1;
    }
    if(gPrintRectangleFlag){
      printRectangle(i, r);
    }
  }
}

void drawRectangle(rectangle_t* r){
  float *c = r->center;
  float hw = r->half_width;
  float hh = r->half_height;
  glColor3fv(r->color);
  glPushMatrix( );
  glTranslatef(r->translation[0], r->translation[1], r->translation[2]);

  glBegin(GL_TRIANGLES);
  glVertex3f(c[0] + hw, c[1] + hh, 0.0);
  glVertex3f(c[0] - hw, c[1] - hh, 0.0);
  glVertex3f(c[0] + hw, c[1] - hh, 0.0);
  
  glVertex3f(c[0] + hw, c[1] + hh, 0.0);
  glVertex3f(c[0] - hw, c[1] + hh, 0.0);
  glVertex3f(c[0] - hw, c[1] - hh, 0.0);
  glEnd( );

  glPopMatrix( );
}

void keyboardUp( unsigned char key, int x, int y ){
  switch( key ){
    //default:
    //fprintf( stderr, "The key '%c' (%d) just went up.\n", key, key );
  }
}

void keyboard( unsigned char key, int x, int y ){
  //fprintf( stderr, "You pushed '%c' (%d).\n", key, key );
  switch( key ){
  case 27: // The 'esc' key
  case 'q':
#ifdef FREEGLUT
    glutLeaveMainLoop( );
#else
    exit( 0 );
#endif
    break;
  case '1':
    gTimer = 500;
    break;
  case '2':
    gTimer = 16;
    break;
  case 'd':
    gPrintRectangleFlag = !gPrintRectangleFlag;
    break;
  case 'r':
    for(int i = 0; i < NUMRECTS; i++){
      resetUnitSquare(gRects[i]);
    }
    break;
  default:
    fprintf( stderr, "Unregistered: You pushed '%c' (%d).\n", key, key );
    break;
  }
}

void display( ){
  msglError( );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  for(int i = 0; i < NUMRECTS; i++){
    drawRectangle(gRects[i]);
  }
  glutSwapBuffers( );
  msglError( );
}

void reshape( int width, int height ){
  fprintf( stderr, "reshape\n" );
  if (height == 0){
    height = 1;
  }
  glViewport( 0, 0, (GLsizei)width, (GLsizei)height );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
}

void initOpenGL( ){
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glEnable( GL_DEPTH_TEST );
  glDepthRange(0.0, 1.0);
  glDepthFunc(GL_LEQUAL);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  msglError( );
}

void initData( ){
  float colors[8][3] = {{1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},
  {1.0, 1.0, 1.0},
  {1.0, 0.0, 1.0},
  {0.0, 1.0, 1.0},
  {0.5, 0.5, 0.5},
  {1.0, 1.0, 0.0}};
  srand(time(NULL));
  gTimer = 16;
  gPrintRectangleFlag = false;
  for(int i = 0; i < NUMRECTS; i++){
    gRects[i] = (rectangle_t*)malloc(sizeof(rectangle_t));
    initUnitSquare(gRects[i], colors[(rand( ) % 8)]);
  }
}

void update( int x ){
  for(int i = 0; i < NUMRECTS; i++){
    updateRectangle(gRects[i]);
  }
  glutTimerFunc( gTimer, update, 0 );
  glutPostRedisplay( );
  msglError( );
}

int main(int argc, const char* argv[]){
  glutInit( &argc, (char**)argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowSize( 800, 800);
  glutCreateWindow( "Moving Boxes" );
#ifdef FREEGLUT
  //glutFullScreen( );
#endif
  initOpenGL( );
  initData( );
  glutKeyboardFunc( keyboard );
  glutKeyboardUpFunc( keyboardUp );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutTimerFunc( gTimer, update, 0 );
  glutMainLoop( );
  return(0);
}
