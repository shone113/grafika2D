#version 330 core

out vec4 outCol;
uniform vec3 uColor;  // Uniform promenljiva za boju
in vec4 channelCol; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu

void main()
{
	outCol = channelCol;
}