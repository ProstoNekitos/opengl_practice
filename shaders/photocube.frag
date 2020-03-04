#version 330 core
//out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform samplerCube tex2;

void main()
{
    gl_FragColor = mix(texture(skybox, TexCoords), texture(tex2, TexCoords), 0.5);
}
