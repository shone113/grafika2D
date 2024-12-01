#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica
uniform bool showGrass; // kontrola prikaza trave


void main()
{
	//outCol = texture(uTex, chTex); //boja na koordinatama chTex teksture vezane na teksturnoj jedinici uTex
	//outCol = vec4(chTex, 0.0, 1.0);

	vec4 texColor = texture(uTex, chTex); // Uzimanje boje teksture
    if (showGrass) {
        outCol = texColor; // Prikazujemo teksturu normalno
    } else {
        outCol = vec4(texColor.rgb, 0.0); // Postavljamo alfa na 0 (potpuna transparentnost)
    }
}