#version 330 core

out vec4 outCol;
uniform vec3 uColor;  // Uniform promenljiva za boju

uniform bool lakeTransparent; // kontrola prikaza trave
uniform bool lakeShader;

void main()
{
	//outCol = vec4(0.0, 0.4, 0.8, 1.0); // Svetloplava boja
	if(lakeTransparent){
		outCol = vec4(uColor, 1.0);  // Boja sa alfa kanal vrednoš?u 1.0 (neprozirno)
	}else{
		outCol = vec4(uColor, 0.5);  // Boja sa alfa kanal vrednoš?u 1.0 (neprozirno)
	}

}