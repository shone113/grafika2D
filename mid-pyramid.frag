#version 330 core

out vec4 outCol;
uniform float uMask;
uniform vec4 uColor;  // Uniformna promenljiva za boju
in vec2 fragCoord;  // Koordinate fragmenta koje dolaze iz vertex šadera
uniform int uW;
uniform int uH;

void main()
{

	if(gl_FragCoord.x >= (uW / 2) - uMask)
		outCol = vec4(0.8, 0.4, 0.0, 1.0);
	else
		outCol = vec4(1.0, 0.0, 0.0, 1.0);
	if (gl_FragCoord.y < (uW / 2) - uMask && mod(gl_FragCoord.x, 10) < 5)
		discard;

}