#version 330 core
out vec4 color;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;



const int LIGHT_NUM = 4;
uniform mat4 PolygonLight[LIGHT_NUM];





void main(){

	for(int i = 0; i < LIGHT_NUM; i++){}
          


	color = vec4(0.5,0,0.5,0);
}