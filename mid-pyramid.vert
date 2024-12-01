#version 330 core

layout(location = 0) in vec2 inPos;
uniform float uMask;
out vec2 fragCoord;  // Koordinate fragmenta koje šaljemo u fragment šader

void main()
{
	gl_Position = vec4(inPos, 0.0, 1.0);
	fragCoord = gl_Position.xy * 0.5 + 0.5;  // Normalizovane ekran koordinate [0, 1]
}