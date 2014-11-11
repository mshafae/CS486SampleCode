# version 120

// These are passed from the vertex shader to here, the fragment shader
// In later versions of GLSL these are 'in' variables.
varying vec3 myNormal;
varying vec4 myVertex;

// These are passed in from the CPU program, camera_control_*.cpp
uniform vec4 light2_position;
uniform vec4 light3_position;

void main (void){
  vec3 W = vec3(1, 1, 1);
  vec3 P = vec3(1, 2, 20);
  // They eye is always at (0,0,0) looking down -z axis 
  // Also compute current fragment position and direction to eye 
  const vec3 eyepos = vec3(0,0,0);
  vec4 _mypos = gl_ModelViewMatrix * myVertex;
  vec3 mypos = vec3(_mypos.xyz / _mypos.w);
  vec3 eyedirn = normalize(eyepos - mypos);

  // Compute normal, needed for shading. 
  vec3 normal = normalize(gl_NormalMatrix * myNormal);

  // Light 0, point
  vec3 position0 = light2_position.xyz / light2_position.w;
  vec3 direction0 = normalize (position0 - mypos);
  float cos_theta0 = max(dot(eyedirn, reflect(normal, direction0)), 0);
  vec4 color0 = vec4(1, 1, 1, 1);
  color0.r = W.r * pow((cos_theta0 + 1)/2, P.r);
  color0.g = W.g * pow((cos_theta0 + 1)/2, P.g);
  color0.b = W.b * pow((cos_theta0 + 1)/2, P.b);
  color0.a = 1.0;

  // Light 1, point 
  vec3 position1 = light3_position.xyz / light3_position.w;
  vec3 direction1 = normalize(position1 - mypos);
  float cos_theta1 = max(dot(eyedirn, reflect(normal, direction1)), 0);
  vec4 color1 = vec4(1, 1, 1, 1);
  color1.r = W.r * pow((cos_theta1 + 1)/2, P.r);
  color1.g = W.g * pow((cos_theta1 + 1)/2, P.g);
  color1.b = W.b * pow((cos_theta1 + 1)/2, P.b);
  color1.a = 1.0;
      
  gl_FragColor = color0 + color1;
}
