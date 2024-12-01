
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;
uniform vec4 vertexColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = textColor * sampled;

    //color = textColor;

     //float alpha = texture(text, TexCoords).r; // Uzima vrednost crvene komponente (koristi se za slova)
    //color = vec4(textColor, alpha);

     //color = texture(text, TexCoord); // Uzmi boje sa teksture

}  