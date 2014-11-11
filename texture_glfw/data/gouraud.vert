#version 120
/*
 * Michael Shafae
 * mshafae at fullerton.edu
 * 
 * A simple Gouraud shader with one light source.
 * This file is the vertex shader which calculates
 * the the vector to the light, the vector to the viewer,
 * and the reflection vector. The color is calculated
 * using the Phong illumination equation.
 *
 * For more information see:
 *     <http://en.wikipedia.org/wiki/Gouraud_shading>
 *
 * $Id: blinn_phong.frag.glsl 4891 2014-04-05 08:36:23Z mshafae $
 *
 * Be aware that for this course, we are limiting ourselves to
 * GLSL v.1.2. This is not at all the contemporary shading
 * programming environment, but it offers the greatest degree
 * of compatability.
 *
 * Please do not use syntax from GLSL > 1.2 for any homework
 * submission.
 *
 */

uniform vec4 light2_position;
uniform vec4 light3_position;

// out
varying vec4 myColor;

void main( ){
  vec4 ambientMaterial = vec4(0.1, 0.1, 0.1, 1.0);
  vec4 diffuseMaterial = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 specularMaterial = vec4(0.0, 0.0, 1.0, 1.0);
  float specularPower = 4.0;
  
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  
	// Transforming The Vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	// Transforming The Normal To ModelView-Space
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 viewer = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 eye = normalize(-viewer);

  vec3 _light2_position = light2_position.xyz / light2_position.w;
  vec3 light2 = normalize(_light2_position - viewer);
  vec3 reflection2 = normalize(reflect(-light2, normal));
	
	vec3 _light3_position = light3_position.xyz / light3_position.w;
	vec3 light3 = normalize(_light3_position - viewer);
	vec3 reflection3 = normalize(reflect(-light3, normal));
	
	// Transforming The Vertex Position To ModelView-Space
	vec4 vertex_in_modelview_space = gl_ModelViewMatrix * gl_Vertex;

  vec4 ambient = vec4(0.2, 0.0, 0.0, 1.0);

  vec4 diffuse2 = clamp(diffuseMaterial * max(dot(normal, light2), 0.0), 0.0, 1.0);
  vec4 specular2 = clamp(specularMaterial * pow(max(dot(reflection2, eye), 0.0), specularPower), 0.0, 1.0);
	
	vec4 diffuse3 = clamp(diffuseMaterial * max(dot(normal, light3), 0.0), 0.0, 1.0);
  vec4 specular3 = clamp(specularMaterial * pow(max(dot(reflection3, eye), 0.0), specularPower), 0.0, 1.0);
	
  myColor = ambient + diffuse2 + specular2 + diffuse3 + specular3;
}
