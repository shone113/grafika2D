#version 330 core

layout(location = 0) in vec2 inPosition; // Pozicija ta?ke (u 2D prostoru)
layout(location = 1) in vec3 inColor;    // Boja ta?ke

uniform vec2 uTranslation; // Uniform promenljiva za translaciju
uniform float doorAnimation; // Uniform za animaciju vrata (može biti 0.0 do 1.0)

uniform float doorHeight; // Vrednost koja kontroliše visinu vrata

void main()
{
    // Dodavanje translacije i animacije (ako vrata treba da se otvore)
    vec2 translatedPos = inPosition + uTranslation;

    //animatedPosition.x *= doorAnimation;  // Ovdje koristimo "doorAnimation" za širenje vrata

    // Postavljamo kona?nu poziciju vrata
    //gl_Position = vec4(animatedPosition, 0.0, 1.0);

    //gl_Position = vec4(inPosition.x, inPosition.y + doorHeight, 0.0, 1.0);
    gl_Position = vec4(translatedPos.x, translatedPos.y + doorHeight, 0.0, 1.0);
}
