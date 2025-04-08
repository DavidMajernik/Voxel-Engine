#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint AO;

out vec2 TexCoord;
out float vAO;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vAO = AO/3.0;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}