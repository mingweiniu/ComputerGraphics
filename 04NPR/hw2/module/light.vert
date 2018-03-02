#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out vec4 vertColor;

void main(){
	int i = 0;
	gl_Position = position;
	vertColor = color;

}