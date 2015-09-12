// 
// Michael Shafae
// mshafae at fullerton.edu
// 
// The Utah teapot floating in space. Camera can be rotated up/down and 
// left right. The camera's transformations are defined/implemented in
// transformations.h/cpp.
//
// This demonstration program requires that you provide your own GFXMath.h.
//
// $Id: camera_control_glfw.cpp 5625 2015-03-26 06:18:52Z mshafae $
//

#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#include "GLFWApp.h"
#include "GFXMath.h"

#include "GLSLShader.h"
#include "transformations.h"
#include "glut_teapot.h"

void msglVersion(void){
  fprintf(stderr, "OpenGL Version Information:\n");
  fprintf(stderr, "\tVendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "\tRenderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "\tOpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "\tGLSL Version: %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));
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

typedef struct ray{
  // origin
  Vec3 o;
  // direction
  Vec3 d;
}ray;

typedef struct triangle{
  Vec3 a;
  Vec3 b;
  Vec3 c;
}triangle3d;

typedef struct hit{
  double t;
  Vec3 point;
}hitd;

void rayEval3(Vec3& p, ray& r, double t){
  int i;
  for(i = 0; i < 3; i++){
    p[i] = r.o[i] + (t * r.d[i]); 
  }
}

bool rayTriangleIntersectiond(hit& h, ray& r, triangle& t){
  // Real Time Rendering p.581
  // There is a bug in here I think.
  Vec3 e1, e2;
  Vec3 p, s, q;
  double a, f, u, v, _t;
  e1 = t.b - t.a;
  e2 = t.c - t.a;
  p = cross(r.d, e2);
  a = dot(e1, p);
  if(fpEqual(a, 0.0, FP_SP_EPSILON)){
    return(false);
  }else{
    printf("XXXX %g\n", a);
  }
  f = 1.0/a;
  s = r.o - t.a;
  u = dot(s, p) * f;
  if(u < 0 || u > 1){
    return(false);
  }else{
    printf("YYYY %g\n", u);
    
  }
  q = cross(s, e1);
  v = dot(r.d, q) * f;
  if(v < 0 || (u+v) > 1){
    return(false);
  }else{
    printf("ZZZZ %g\n", v);
  }
  _t = dot(e2, q) * f;
  h.t = _t;
  rayEval3(h.point, r, _t);
  return(true);  
}


class PickingApp : public GLFWApp{
private:
  triangle tri;
  
  float rotationDelta;

  Vec3 centerPosition;
  Vec3 eyePosition;
  Vec3 upVector;

  Mat4 modelViewMatrix;
  Mat4 projectionMatrix;
  Mat4 normalMatrix;
  
  GLSLProgram shaderProgram;

  Vec4 light0;
  Vec4 light1; 

  // Variables to set uniform params for lighting fragment shader 
  unsigned int uModelViewMatrix; 
  unsigned int uProjectionMatrix; 
  unsigned int uNormalMatrix; 
  unsigned int uLight0_position;
  unsigned int uLight0_color;
  unsigned int uLight1_position;
  unsigned int uLight1_color;
  unsigned int uAmbient;
  unsigned int uDiffuse;
  unsigned int uSpecular;
  unsigned int uShininess;
  
public:
  PickingApp(int argc, char* argv[]) :
    GLFWApp(argc, argv, std::string("Camera Control").c_str( ), 500, 500){ }
  
  void initCenterPosition( ){
    centerPosition = Vec3(0.0, 0.0, 0.0);
  }
  
  void initEyePosition( ){
    eyePosition = Vec3(0.0, 0.0, 6.0);
  }

  void initUpVector( ){
    upVector = Vec3(0.0, 1.0, 0.0);
  }

  void initRotationDelta( ){
    rotationDelta = 5.0;
  }
  
  void initTriangle( ){
    // The triangle that is in the upper right quarter of the window
    tri.a[0] = 0.0;
    tri.a[1] = 0.0;
    tri.a[2] = 0.0;
  
    tri.b[0] = 4.0;
    tri.b[1] = 0.0;
    tri.b[2] = 0.0;
  
    tri.c[0] = 0.0;
    tri.c[1] = 4.0;
    tri.c[2] = 0.0;
  }
  
  bool begin( ){
    msglError( );
    initTriangle( );
    initCenterPosition( );
    initEyePosition( );
    initUpVector( );
    initRotationDelta( );

    // Load the shader program
    const char* vertexShaderSource = "blinn_phong.vert.glsl";
    const char* fragmentShaderSource = "blinn_phong.frag.glsl";
    FragmentShader fragmentShader(fragmentShaderSource);
    VertexShader vertexShader(vertexShaderSource);
    shaderProgram.attach(vertexShader);
    shaderProgram.attach(fragmentShader);
    shaderProgram.link( );
    shaderProgram.activate( );
    
    printf("Shader program built from %s and %s.\n",
           vertexShaderSource, fragmentShaderSource);
    if( shaderProgram.isActive( ) ){
      printf("Shader program is loaded and active with id %d.\n", shaderProgram.id( ) );
    }else{
      printf("Shader program did not load and activate correctly. Exiting.");
      exit(1);
    }

    // Set up uniform variables
    uModelViewMatrix = glGetUniformLocation(shaderProgram.id( ),
                                            "modelViewMatrix");
    uProjectionMatrix = glGetUniformLocation(shaderProgram.id( ),
                                             "projectionMatrix");
    uNormalMatrix = glGetUniformLocation(shaderProgram.id( ),
                                         "normalMatrix");
    uLight0_position = glGetUniformLocation(shaderProgram.id( ),
                                            "light0_position");
    uLight0_color = glGetUniformLocation(shaderProgram.id( ),
                                         "light0_color");
    uLight1_position = glGetUniformLocation(shaderProgram.id( ),
                                            "light1_position");
    uLight1_color = glGetUniformLocation(shaderProgram.id( ),
                                         "light1_color");
    uAmbient = glGetUniformLocation(shaderProgram.id( ), "ambient");
    uDiffuse = glGetUniformLocation(shaderProgram.id( ), "diffuse");
    uSpecular = glGetUniformLocation(shaderProgram.id( ), "specular");
    uShininess = glGetUniformLocation(shaderProgram.id( ), "shininess");
    
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    msglVersion( );
    
    return !msglError( );
  }
  
  bool end( ){
    windowShouldClose( );
    return true;
  }
  
  bool intersectionDemo(Vec3& pointA, Vec3& pointB){
    ray r;
    double t;
    hitd h;
    r.o = pointA;
    r.d = normalize(pointB - pointA);
    bool rv = rayTriangleIntersectiond(h, r, tri);
    if(rv){
      fprintf(stderr, "Intersection! %.5f\n", h.t);
    }else{
      fprintf(stderr, "No intersection.");
    }
    return(rv);
  }
  
  void pick( int x, int y ){
    /*****************************
     * This is not correct. This is just a to illustrate some ideas.
     * Note that the projection matrix and modeling matrix are
     * both identity. You need to implement your own unproject function.
     *****************************/
    GLint viewport[4];
    double modelMatrix[16], projMatrix[16];
    GLint wx = x;
    GLint wy;
    GLdouble xx, yy, zz;
  
    fprintf( stderr, "--Starting Pick--\nmouse reports (%d, %d)\n", x, y );

    
    glGetDoublev( GL_PROJECTION_MATRIX, projMatrix );
    glGetDoublev( GL_MODELVIEW_MATRIX, modelMatrix );
    glGetIntegerv( GL_VIEWPORT, viewport );
    
    msglPrintMatrix16dv("projMatrix", projMatrix);
    msglPrintMatrix16dv("modelMatrix", modelMatrix);
    
    // origin at the lower left corner
    wy = viewport[3] - y - 1;
    fprintf( stderr, "real y = viewport[3] - y - 1\n%d = %d - %d - 1\n",
      wy, viewport[3], y );
    fprintf( stderr, "Coordinates at cursor are (%d, %d)\n", wx, wy );

  
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

    Vec3 near(xx, yy, zz);
    /*double near[3];
    near[0] = xx;
    near[1] = yy;
    near[2] = zz;*/

    // unproject wx, wy, the far plane (z = 1); store in xx, yy, zz
    gluUnProject( double(wx), double(wy), 1.0, modelMatrix, projMatrix,
      viewport, &xx, &yy, &zz );
    fprintf( stderr, "pick at far plane: %g %g %g\n", xx, yy, zz );

    Vec3 far(xx, yy, zz);
    /*double far[3];
    far[0] = xx;
    far[1] = yy;
    far[2] = zz;*/

    fprintf(stderr, "--Ending Pick--\n");
    //eyePosition
    if(intersectionDemo(near, far)){
      fprintf(stderr, "There was an intersection!\n");
    }else{
      fprintf(stderr, "You missed!.\n");
    }
  }
  
  bool render( ){
    Vec4 light0_position(0.0, 5.0, 10.0, 1.0);
    Vec4 light1_position(0,5,-10,1);
    const Vec4 light0_specular(0.6,0.3,0,1);
    const Vec4 light1_specular(0,0.3,0.6,1);
    // specular on teapot
    const Vec4 one(1,1,1,1);
    // diffuse on teapot
    const Vec4 medium(0.5,0.5,0.5,1);
    // ambient on teapot
    const Vec4 small(0.2,0.2,0.2,1);
    // shininess of teapot
    const Vec1 high(100);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Vec2i w = windowSize( );
    double ratio = double(w[0]) / double(w[1]);

    projectionMatrix = perspective(90.0, ratio, 1.0, 25.0);

    modelViewMatrix = lookat(eyePosition, centerPosition, upVector);

    normalMatrix = modelViewMatrix.inverse().transpose( );

    // Set light & material properties for the teapot;
    // lights are transformed by current modelview matrix
    // such that they are positioned correctly in the scene.
    light0 = modelViewMatrix * light0_position;
    light1 = modelViewMatrix * light1_position;
    
    glUniformMatrix4fv(uModelViewMatrix, 1, false, modelViewMatrix);
    glUniformMatrix4fv(uProjectionMatrix, 1, false, projectionMatrix);
    glUniformMatrix4fv(uNormalMatrix, 1, false, normalMatrix);
    glUniform4fv(uLight0_position, 1, light0); 
    glUniform4fv(uLight0_color, 1, light0_specular); 
    glUniform4fv(uLight1_position, 1, light1); 
    glUniform4fv(uLight1_color, 1, light1_specular); 

    glUniform4fv(uAmbient, 1, small); 
    glUniform4fv(uDiffuse, 1, medium); 
    glUniform4fv(uSpecular, 1, one); 
    glUniform1fv(uShininess, 1, high); 

    //_glutSolidTeapot(2);
    glBegin( GL_TRIANGLES );
    glColor3d( 1.0, 0.0, 0.0 );
    glVertex3fv( (float*)(tri.a) );
    glColor3d( 0.0, 1.0, 0.0 );
    glVertex3fv( (float*)(tri.b) );
    glColor3d( 0.0, 0.0, 1.0 );
    glVertex3fv( (float*)(tri.c) );
    glEnd( );
    
    
    if(isKeyPressed('Q')){
      end( );      
    }else if(isKeyPressed(GLFW_KEY_EQUAL)){
      rotationDelta += 1.0;
      printf( "Rotation delta set to %g\n", rotationDelta );
    }else if(isKeyPressed(GLFW_KEY_MINUS)){
      rotationDelta -= 1.0;
      printf( "Rotation delta set to %g\n", rotationDelta );
    }else if(isKeyPressed('R')){
      initEyePosition( );
      initUpVector( );
      initRotationDelta( );
      printf("Eye position, up vector and rotation delta reset.\n");
    }else if(isKeyPressed(GLFW_KEY_LEFT)){
      rotateCameraLeft(-rotationDelta, eyePosition,
                       centerPosition, upVector);
    }else if(isKeyPressed(GLFW_KEY_RIGHT)){
      rotateCameraLeft(rotationDelta, eyePosition,
                       centerPosition, upVector);
    }else if(isKeyPressed(GLFW_KEY_UP)){
      rotateCameraUp(-rotationDelta, eyePosition,
                     centerPosition, upVector);
    }else if(isKeyPressed(GLFW_KEY_DOWN)){
      rotateCameraUp(rotationDelta, eyePosition,
                     centerPosition, upVector);
    }
    if(mouseButtonFlags( ) == GLFWApp::MOUSE_BUTTON_LEFT){
      printf("mouse left button\n");
      Vec2 mousePosition = mouseCurrentPosition( );
      std::cout << mousePosition << std::endl;
      pick(mousePosition[0], mousePosition[1]);
    }
    return !msglError( );
  }
   
};


int main(int argc, char* argv[]){
  PickingApp app(argc, argv);
  return app();
}

