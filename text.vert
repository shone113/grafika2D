#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout(location = 1) in vec4 aColor; // Boja vertex-a (ako koristiš boje)
out vec2 TexCoords;

uniform mat4 projection;

out vec4 vertexColor; // Boja koja ide u fragment shader

uniform mat4 view;
uniform mat4 model;

void main()
{
    //gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;

      gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    vertexColor = vec4(aColor);  // Postavi vrednost za vertexColor
}  