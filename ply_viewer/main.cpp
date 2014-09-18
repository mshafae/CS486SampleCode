/*
 * Copyright (c) 2013 Michael Shafae
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
 * $Id: Primitive.h 4472 2013-09-29 07:43:28Z mshafae $
 *
 */

#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <cstdlib>
#include <cstdio>

#include "PlyModel.h"

FaceList *gModel;

static void error_callback( int error, const char* description ){
    fputs( description, stderr );
}

static void key_callback( GLFWwindow* window, int key,
                          int scancode, int action, int mods){
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

bool fileExists(const char *f){
  FILE *fh;
  bool rv = true;
  if( (fh = fopen(f, "r")) == NULL ){
    fprintf( stderr, "Opening file %s encountered an error.\n", f );
    rv = false;
  }else{
    fclose( fh );
  }
  return( rv );
}

int main(int argc, char** argv){
  GLFWwindow* window;

  glfwSetErrorCallback(error_callback);

  if( !glfwInit( ) ){
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_DEPTH_BITS, 16);
  window = glfwCreateWindow( 800, 600, "PLY Viewer", NULL, NULL );
  if( !window ){
    glfwTerminate( );
    exit(1);
  }
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);

  if( fileExists( argv[1] ) ){
    gModel = readPlyModel( argv[1] );
  }else{
    exit(1);
  }
  glShadeModel( GL_SMOOTH );
  GLfloat l_ambient[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat l_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat l_position[] = {0.0, 1.5, -1.5, 0.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, l_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, l_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, l_position);

  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  
  glEnable( GL_DEPTH_TEST );
  glFrontFace( GL_CCW );
  glEnable(GL_NORMALIZE);
  
  while (!glfwWindowShouldClose(window))
  {
      float ratio;
      int width, height;

      glfwGetFramebufferSize(window, &width, &height);
      ratio = float(width) / float(height);

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(50.0, ratio, 0.1, 5.0);
      glMatrixMode(GL_MODELVIEW);

      double scaleFactor = 1.0 / gModel->radius;

      glLoadIdentity();
      gluLookAt (0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
      glRotatef((float) glfwGetTime() * 50.f, 0.577f, 0.577f, 0.577f);

      glScalef(scaleFactor, scaleFactor, scaleFactor);

      GLfloat ambient[] = {0.5, 0.5, 0.5};
      GLfloat diffuse[] = {0.07568, 0.61424, 0.07568};
      GLfloat specular[] = {0.633, 0.727811, 0.633};
      GLfloat shine = 0.6;
      
      glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
      glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
      
      glBegin(GL_TRIANGLES);
      for(int i = 0; i < gModel->fc; i++ ){
        for(int j = 0; j < 3; j++){
          glColor3dv(gModel->colors[gModel->faces[i][j]]);
          glNormal3dv(gModel->v_normals[gModel->faces[i][j]]);
          glVertex3dv(gModel->vertices[gModel->faces[i][j]]);
        }
      }
      glEnd();
      
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate( );
  return(0);
}
