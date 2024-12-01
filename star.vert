#version 330 core

layout(location = 0) in vec2 inPosition; // Pozicija ta?ke
layout(location = 1) in vec3 inColor;    // Boja ta?ke

out vec3 fragColor; // Boja koju šaljemo u fragment shader

uniform vec2 uTranslation; // Uniform promenljiva za translaciju
uniform vec2 uPosOrb;
out vec4 channelCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera
uniform bool showStars; // kontrola prikaza trave


void main()
{
    if(!showStars){
        gl_Position = vec4(inPosition + uTranslation, 0.0, 1.0); // Dodajemo translaciju

        float smoothInterp = smoothstep(0.0, 1.0, abs(uPosOrb.x));  // Postepeni prelaz izme?u 0 i 1

        vec3 morningEveningColor = vec3(0.3, 0.5, 0.8); // Svetlija plava za jutro/ve?e
        vec4 starColor = vec4(1.0, 0.8, 0.0, 1.0); 

        vec3 finalColor = mix(morningEveningColor, starColor.rgb, 1.0 - smoothInterp); // Prelaz izme?u boja

        channelCol = vec4(finalColor, 1.0);
    }
}