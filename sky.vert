#version 330 core

layout(location = 0) in vec2 inPos;
uniform float uMask;
out vec2 fragCoord;  // Koordinate fragmenta koje šaljemo u fragment šader
uniform bool uDaySky;  // Uniformna bool promenljiva
out vec4 channelCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera
out vec3 colOffset; 
uniform vec2 uPosSky;


void main()
{
	gl_Position = vec4(inPos, 0.0, 1.0);
	fragCoord = gl_Position.xy * 0.5 + 0.5;  // Normalizovane ekran koordinate [0, 1]


    //float interp = abs(uPosSky.x); // Normalizovana vrednost [0, 1] za horizontalnu poziciju
    float smoothInterp = smoothstep(0.0, 1.0, abs(uPosSky.x));  // Postepeni prelaz izme?u 0 i 1

    // Definiši boje
    vec3 nightColor = vec3(0.05, 0.05, 0.2); // Najtamnija plava za no?
    vec3 morningEveningColor = vec3(0.3, 0.5, 0.8); // Svetlija plava za jutro/ve?e
    vec3 dayColor = vec3(0.53, 0.81, 0.92); // Svetlo plava za dan

    vec3 finalColor;
    if(uDaySky){
         finalColor = mix(morningEveningColor, dayColor, 1.0 - smoothInterp); // Prelaz izme?u boja
    }else{
        finalColor = mix(morningEveningColor, nightColor, 1.0 - smoothInterp); // Prelaz izme?u boja
    }

    channelCol = vec4(finalColor, 1.0);

}