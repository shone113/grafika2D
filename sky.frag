#version 330 core

out vec4 outCol;
uniform float uMask;
uniform vec4 uColor;  // Uniformna promenljiva za boju
in vec2 fragCoord;  // Koordinate fragmenta koje dolaze iz vertex šadera
uniform int uH;
uniform bool uDaySky;  // Uniformna bool promenljiva
uniform float uDayFactor; // Faktor izme?u dana (1.0) i no?i (0.0)
in vec4 channelCol; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu
in vec3 colOffset; //Promena boje u odnosu na udaljenost od x ose


void main()
{
    outCol = channelCol;
}