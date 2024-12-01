#version 330 core

out vec4 outCol;
uniform vec3 uColor;  // Uniform promenljiva za boju

in vec2 chTex; //koordinate teksture
uniform sampler2D uTex; //teksturna jedinica

uniform bool showFish;

void main()
{
	outCol = texture(uTex, chTex);
}