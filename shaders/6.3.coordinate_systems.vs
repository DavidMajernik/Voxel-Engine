#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint AO;

out vec2 TexCoord;
out float vAO;
out vec3 FragPos;
out float isRenderingWaterFlag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform bool renderingWater;

void main()
{
    vAO = AO/3.0;
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0f));
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);

    // Apply a simple wave effect based on time

    if(renderingWater) {
        float wave = sin(time + aPos.x * 0.5) * 0.05; 
        float wave2 = sin(time + aPos.z * 0.6) * 0.05;
        worldPos.y += wave + wave2; // Modify the y-coordinate to create a wave effect
        worldPos.y -= 0.2;

    }

    if(renderingWater) {
        isRenderingWaterFlag = 1.0;
    } else {
        isRenderingWaterFlag = 0.0;
    }

    gl_Position = projection * view * model * worldPos;


}