# version 120 

// These are variables that we wish to send to our fragment shader
// In later versions of GLSL, these are 'out' variables.
varying vec3 myNormal;
varying vec4 myVertex;

void main() {
  //gl_Position = ftransform( );
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
  myNormal = gl_Normal;
  myVertex = gl_Vertex;
}
