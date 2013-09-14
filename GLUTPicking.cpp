/*
 * Copyright (c) 2005-2013 Michael Shafae
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id: GLUTPicking.cpp 4429 2013-09-14 04:12:30Z mshafae $
 *
 */

// Demo of the Z-Buffer and how to read from it
// Demo of how to cast a ray for use for object picking.

#ifdef _WIN32
#include <Windows.h>
#include <cstdlib>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>
#define sprintf sprintf_s
#define strdup _strdup
#else
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#define FP_SP_EPSILON 1e-6
#define FP_DP_EPSILON 1e-15

#define FP_EQUAL(a, b, delta) ( ((a) == (b)) || \
((a)-(delta) < (b) && \
  (a)+(delta) > (b)) )

#define FP_NOT_EQUAL(a, b, delta) ( ((a) != (b)) && \
((a)-(delta) > (b) || \
  (a)+(delta) < (b)) )

#define FP_LT(a, b, delta)  (fabs((a) - (b)) < (delta))

#define FP_GT(a, b, delta)  (fabs((a) - (b)) > (delta))

#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif

typedef struct ray3d{
  // origin
  double o[3];
  // direction
  double d[3];
}ray3d;

typedef struct triangle3d{
  double a[3];
  double b[3];
  double c[3];
}triangle3d;

typedef struct hitd{
  double t;
  double point[3];
}hitd;

#define msglError( ) _msglError( stderr, __FILE__, __LINE__ )

int gPerspMode;
char* gPerspModeStrings[2];
int gDepthOn;
char* gDepthModeStrings[2];

void matCopy3d(double *dest, double *src){
  memcpy(dest, src, sizeof(double) * 9);
}

void vecCopy3d(double *dest, double *src){
  int i;
  for(i = 0; i < 3; i++){
    dest[i] = src[i];
  }
}

void rayEval3d(double *p, ray3d *r, double t){
  int i;
  for(i = 0; i < 3; i++){
    p[i] = r->o[i] + (t * r->d[i]); 
  }
}

double vecDot3d(double *a, double *b){
  double accumulate = 0.0;
  int i;
  for(i = 0; i < 3; i++){
    accumulate += a[i] * b[i];
  }
  return( accumulate );
}

void vecCross3d(double *n, double *u, double *v){
  n[0] = u[1] * v[2] - u[2] * v[1];
  n[1] = u[2] * v[0] - u[0] * v[2];
  n[2] = u[0] * v[1] - u[1] * v[0]; 
}

void vecDifference3d(double *c, double *a, double *b){
  int i;
  for(i = 0; i < 3; i++){
    c[i] = a[i] - b[i];
  }
}

void matMultVec3d(double* vout, double* v, double* m){
  double c[3];
  vecCopy3d(c, v);
  vout[0] = m[0] * c[0] + m[3] * c[1] + m[6] * c[2];
  vout[1] = m[1] * c[0] + m[4] * c[1] + m[7] * c[2];  
  vout[2] = m[2] * c[0] + m[5] * c[1] + m[8] * c[2];  
}

bool rayTriangleIntersectiond(hitd *h, ray3d *r, triangle3d *t){
  // Real Time Rendering p.581
  double e1[3], e2[3];
  double p[3], s[3], q[3];
  double a, f, u, v, _t;
  vecDifference3d(e1, t->b, t->a);
  vecDifference3d(e2, t->c, t->a);
  vecCross3d(p, r->d, e2);
  a = vecDot3d(e1, p);
  if(FP_EQUAL(a, 0.0, FP_DP_EPSILON)){
    puts("first");
    return(false);
  }
  f = 1.0/a;
  vecDifference3d(s, r->o, t->a);
  u = vecDot3d(s, p) * f;
  if(u < 0 || u > 1){
    fprintf(stderr, "second, %.10f\n", u);
    return(false);
  }
  vecCross3d(q, s, e1);
  v = vecDot3d(r->d, q) * f;
  if(v < 0 || (u+v) > 1){
    fprintf(stderr, "third %.7f %.7f %.7f\n", u, v, u+v);
    return(false);
  }
  _t = vecDot3d(e2, q) * f;
  h->t = _t;
  rayEval3d(h->point, r, _t);
  return(true);  
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

void msglPrintMatrix16dv( char *varName, double matrix[16] ){
  int i = 0;
  if( varName != NULL ){
    fprintf( stderr, "%s = [\n", varName );
  }
  for( i = 0; i < 4; i++ ){
    fprintf( stderr, "%.5f %.5f %.5f %.5f\n", matrix[i + 0], matrix[i + 4],
    matrix[i + 8], matrix[i + 12] );
  }
  if( varName != NULL ){
    fprintf( stderr, " ]\n" );
  }
}

void printTopOfBothStacks( char *msg ){
  double m[16];
  msglError( );
  if( msg != NULL ){
    fprintf( stderr, "%s\n", msg );
  }
  fprintf( stderr, "Top of Projection Stack:\n" );
  glGetDoublev( GL_PROJECTION_MATRIX, m );
  msglPrintMatrix16dv( "proj", m );
  fprintf( stderr, "\n\n" );
  glGetDoublev( GL_MODELVIEW_MATRIX, m );
  msglPrintMatrix16dv( "mv", m );
  msglError( );
}

bool intersectionDemo(double* frontPlane, double* backPlane){
  ray3d r;
  double t;
  hitd h;
  triangle3d tri;  
  
  tri.a[0] = 0.0;
  tri.a[1] = 0.0;
  tri.a[2] = 0.0;
  tri.b[0] = 0.5;
  tri.b[1] = 0.0;
  tri.b[2] = 0.0;
  tri.c[0] = 0.0;
  tri.c[1] = 0.5;
  tri.c[2] = 0.0;
  
  /*matMultVec3d(tri.a, tri.a, modelMatrix);
  matMultVec3d(tri.b, tri.b, modelMatrix);
  matMultVec3d(tri.c, tri.c, modelMatrix);*/
  
  r.o[0] = frontPlane[0];
  r.o[1] = frontPlane[1];
  r.o[2] = frontPlane[2];
  r.d[0] = backPlane[0] - frontPlane[0];
  r.d[1] = backPlane[1] - frontPlane[1];
  r.d[2] = backPlane[2] - frontPlane[2];
  bool rv = rayTriangleIntersectiond(&h, &r, &tri);
  if(rv){
    fprintf(stderr, "Intersection! %.5f\n", h.t);
  }else{
    fprintf(stderr, "bummer no intersection.");
  }
  return(rv);
}

void pick( int x, int y ){
  GLint viewport[4];
  GLdouble modelMatrix[16];
  GLdouble projMatrix[16];
  GLint wx = x;
  GLint wy;
  GLdouble xx, yy, zz;
  
  printTopOfBothStacks("foo");
  fprintf( stderr, "--Starting Pick--\nmouse reports (%d, %d)\n", x, y );

  glGetDoublev( GL_PROJECTION_MATRIX, projMatrix );
  glGetDoublev( GL_MODELVIEW_MATRIX, modelMatrix );
  glGetIntegerv( GL_VIEWPORT, viewport );

  wy = viewport[3] - y - 1;
  fprintf( stderr, "real y = viewport[3] - y - 1\n%d = %d - %d - 1\n",
    wy, viewport[3], y );
  fprintf( stderr, "Coordinates at cursor are (%d, %d)\n", wx, wy );

  // origin at the lower left corner
  
  // Read the depth value for wx, wy; store in zz
  glReadPixels( wx, wy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zz );
  fprintf( stderr, "zz: %g\n", zz );

  // unproject wx, wy, zz; store in xx, yy, zz
  gluUnProject( double(wx), double(wy), zz, modelMatrix, projMatrix,
    viewport, &xx, &yy, &zz );
  fprintf( stderr, "pick at pixel wz: %g %g %g\n", xx, yy, zz );

  // unproject wx, wy, the near plane (z = 0); store in xx, yy, zz
  gluUnProject( double(wx), double(wy), 0.0, modelMatrix, projMatrix,
    viewport, &xx, &yy, &zz );
  fprintf( stderr, "pick at near plane: %g %g %g\n", xx, yy, zz );

  double near[3];
  near[0] = xx;
  near[1] = yy;
  near[2] = zz;

  // unproject wx, wy, the far plane (z = 1); store in xx, yy, zz
  gluUnProject( double(wx), double(wy), 1.0, modelMatrix, projMatrix,
    viewport, &xx, &yy, &zz );
  fprintf( stderr, "pick at far plane: %g %g %g\n", xx, yy, zz );

  double far[3];
  far[0] = xx;
  far[1] = yy;
  far[2] = zz;

  fprintf(stderr, "--Ending Pick--\n");
  
  if(intersectionDemo(near, far)){
    fprintf(stderr, "There was an intersection!\n");
  }else{
    fprintf(stderr, "You missed!.\n");
  }
}

void drawString( char *string, double px, double py, double color[3] ){
  glColor3dv( color );
  glRasterPos2d( px, py );
  while( *string ){
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, *string );
    string++;
  }
}

void drawTriangle( double translation[3] ){
  glPushMatrix( );
  glTranslated( translation[0], translation[1], translation[2] );
  glBegin( GL_TRIANGLES );
  glColor3d( 1.0, 0.0, 0.0 );
  glVertex3d( 0.0, 0.0, 0.0 );
  glColor3d( 0.0, 1.0, 0.0 );
  glVertex3d( 0.5, 0.0, 0.0 );
  glColor3d( 0.0, 0.0, 1.0 );
  glVertex3d( 0.0, 0.5, 0.0 );
  glEnd( );
  glPopMatrix( );
}

void initOpenGL( ){
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glEnable( GL_DEPTH_TEST );
  glDepthRange(0.0, 1.0);
  glDepthFunc(GL_LEQUAL);
  msglError( );
}

void initData( ){
  gPerspMode = 1;
  gPerspModeStrings[0] = (char*)"Ortho";
  gPerspModeStrings[1] = (char*)"Perspective";
  gDepthOn = 1;
  gDepthModeStrings[0] = (char*)"Z-Buffer Off";
  gDepthModeStrings[1] = (char*)"Z-Buffer On";
  
}

void reshape( int width, int height ){
  if (height == 0){
		height = 1;
	}
	glViewport( 0, 0, (GLsizei)width, (GLsizei)height );
  double ratio = double(width) / double(height);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( gPerspMode ){
	  gluPerspective( 45.0, ratio, 1.0, 2.0 );
	  //glFrustum(-1, 1, -1, 1, 1, 2);
  }else{
    glOrtho( -ratio, ratio, -ratio, ratio, 1.0, 2.0 );
  }
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  gluLookAt( 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
}


void keyboard( unsigned char key, int x, int y ){
  switch( key ){
  case 27: // The 'esc' key
  case 'q':
#ifdef FREEGLUT
    glutLeaveMainLoop( );
#else
    exit( 0 );
#endif
    break;
  case 'p':
    gPerspMode = !gPerspMode;
    reshape( glutGet( GLUT_WINDOW_WIDTH ), glutGet( GLUT_WINDOW_HEIGHT ));
    break;
  case 'z':
    gDepthOn = !gDepthOn;
    gDepthOn ? glEnable( GL_DEPTH_TEST ) : glDisable( GL_DEPTH_TEST );
    break;
  default:
    //fprintf( stderr, "You pushed '%c' (%d).\n", key, key );
    break;
  }
  glutPostRedisplay( );
}

void mouse( int button, int state, int x, int y ){
  switch( state ){
    case GLUT_DOWN:
      switch( button ){
        case GLUT_LEFT_BUTTON:
        case GLUT_MIDDLE_BUTTON:
        case GLUT_RIGHT_BUTTON:
        pick( x, y );
        fprintf( stderr, "\nA mouse button is down.\n" );
        break;
        default:
        fprintf( stderr, "An unknown mouse button is down.\n" );
        break;
      }
      break;
      case GLUT_UP:
      switch( button ){
        case GLUT_LEFT_BUTTON:
        case GLUT_MIDDLE_BUTTON:
        case GLUT_RIGHT_BUTTON:
        fprintf( stderr, "A mouse button is up.\n" );
        break;
        default:
        fprintf( stderr, "An unknown mouse button is down.\n" );
        break;
      }
      break;
    }
}


void display( ){
  msglError( );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
  // A front facing triangle on the xy plane
  glPushMatrix();
  double triangleTranslation1[3] = { 0.0, 0.0, 0.0 };
  drawTriangle( triangleTranslation1 );
  glPopMatrix();
  
  // A front facing triangle -0.5 behind the xy plane
  double triangleTranslation2[3] = { 0.0, 0.0, -0.5 };
  glPushMatrix( );
  glRotatef(180.0, 0, 0, 1);
  drawTriangle( triangleTranslation2 );
  glPopMatrix( );
  
  // A front facing triangle -1.0 behind the xy plane
  double triangleTranslation3[3] = { 0.0, 0.0, -1.0};
  glPushMatrix( );
  glRotatef(90.0, 0, 0, 1);
  glScalef(0.5, 0.5, 0.5);
  drawTriangle( triangleTranslation3 );
  glPopMatrix( );
  

  double textColor[3] = {1.0, 1.0, 1.0};
  drawString( gPerspModeStrings[gPerspMode], -0.4, 0.38, textColor );
  drawString( gDepthModeStrings[gDepthOn], -0.4, 0.3, textColor );

  glutSwapBuffers( );
  msglError( );
}

int main( int argc, char* argv[] ){
  glutInit( &argc, argv );
  // Initialize the context correctly or the depth buffer will not exist!
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowSize( 500, 500);
  glutCreateWindow( "GLUT Depth Buffer Demo" );
#ifdef FREEGLUT
  glutFullScreen( );
#endif
  initOpenGL( );
  initData( );
  glutKeyboardFunc( keyboard );
  glutDisplayFunc( display );
  glutMouseFunc( mouse );
  glutReshapeFunc( reshape );
  glutMainLoop( );
  return(0);
}
