#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float vAO;
in vec3 FragPos;

uniform sampler2D texture1;

void main()
{
   vec4 texColor = texture(texture1, TexCoord);
   FragColor.rgb *= mix(0.7, 1.0, vAO); 
   FragColor = vec4(texColor.rgb, 0.5);

}