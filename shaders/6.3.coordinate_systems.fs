#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float vAO;

uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture1, TexCoord);
    //FragColor *= vAO;
   FragColor *= mix(0.7, 1.0, vAO); // Simulate AO darkening
}