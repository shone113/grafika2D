#version 330 core

layout(location = 0) in vec2 inPos; //Pozicija tjemena
layout(location = 1) in vec4 inCol; //Boja tjemena - ovo saljemo u fragment sejder
out vec4 channelCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera
out vec3 colOffset; 
uniform vec2 uPos;
uniform vec2 uScale; // Faktor skaliranja (aspekt proporcija)
uniform float xScale;


void main()
{
    float xPos = inPos.x * xScale + uPos.x;
    gl_Position = vec4(xPos, inPos.y + uPos.y - 0.4, 0.0, 1.0);


    // Interpolacija boja u zavisnosti od pozicije
    float interp = abs(uPos.x); // Normalizovana vrednost [0, 1] za horizontalnu poziciju
    vec3 baseColor = vec3(1.0, 0.5, 0.0); // ?uto-narand?asta
    vec3 peakColor = vec3(1.0, 1.0, 0.0); // Jarko ?uta

    vec3 finalColor = mix(baseColor, peakColor, 1.0 - interp); // Prelaz izme?u boja
    channelCol = vec4(finalColor, 1.0);
   
}
