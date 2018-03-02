#version 330 core
out vec4 color;

#define LIGHT_NUM 2
in vec4 vertColor;

void main(){

	for(int i = 0; i < LIGHT_NUM; i++){}
          


	color = vertColor;
}