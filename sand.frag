#version 330 core

out vec4 outCol;
uniform vec3 uColor;  // Uniform promenljiva za boju

void main()
{
	//outCol = vec4(0.0, 0.4, 0.8, 1.0); // Svetloplava boja

	outCol = vec4(uColor, 1.0);  // Boja sa alfa kanal vrednoš?u 1.0 (neprozirno)
	//outCol = vec4(1.0, 1.0, 1.0, 1.0);
}