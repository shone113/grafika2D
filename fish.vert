#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex; //Koordinate texture, propustamo ih u FS kao boje
out vec2 chTex;

uniform float time;
uniform bool movingRight; // Smer kretanja ribe (true = desno, false = levo)
uniform bool showFish;


void main()
{
	float movement = sin(time) * 0.4; // Amplituda 0.5 (prilagodite po želji)

	// Promenite teksturne koordinate na osnovu smera kretanja
	//Ako je cos(time)>0.0, sinus raste, a riba se kre?e udesno
	//Ako je cos(time)<0.0, sinus opada, a riba se kre?e ulevo
    if (cos(time) > 0.0) {
        chTex = inTex; // Normalno
    } else {
        chTex = vec2(1.0 - inTex.x, inTex.y); // Mirror horizontalno
    }

	if(showFish){
		vec3 newPos = vec3(inPos, 0.0) + vec3(movement, 0.0, 0.0); // Pomak levo-desno
		gl_Position = vec4(newPos, 1.0);
	}
}