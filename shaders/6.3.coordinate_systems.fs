#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float vAO;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec4 FogColor;
uniform float FogMaxDist;
uniform float FogMinDist;
uniform vec3 CameraPos;

void main()
{

   FragColor = texture(texture1, TexCoord);
   FragColor *= mix(0.7, 1.0, vAO); 
   FragColor.a = 1.0;

   float distance = length(FragPos - CameraPos);
   float fogFactor = 1- clamp((distance - FogMinDist) / (FogMaxDist - FogMinDist), 0.0, 1.0);

   FragColor = vec4(mix(FogColor.rgb, FragColor.rgb, fogFactor), 1.0);
}