# version 120

uniform sampler2D texture;

void main(){
  vec4 color = texture2D(texture, gl_TexCoord[0].st);
  gl_FragColor = color;
}

