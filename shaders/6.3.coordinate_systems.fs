#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float vAO;
in vec3 FragPos;
in float isRenderingWaterFlag;

uniform sampler2D texture1;
uniform vec4 FogColor;
uniform float FogMaxDist;
uniform float FogMinDist;
uniform vec3 CameraPos;
uniform bool isUnderWater;

void main()
{
   FragColor = texture(texture1, TexCoord);
   FragColor.rgb *= mix(0.7, 1.0, vAO); 
   FragColor.a = 1.0;

   float distance = length(FragPos - CameraPos);

   //Linear fog
   float fogFactor = clamp((distance - FogMinDist) / (FogMaxDist - FogMinDist), 0.0, 1.0);

   //apply fog
   FragColor = vec4(mix(FragColor.rgb, FogColor.rgb, fogFactor), 1.0);

   //make water meshes (and future transparent onces) semi-transparent
   if(isRenderingWaterFlag > 0.5) {
	   FragColor.a = 0.65;
   }

   //underwater effect
   if(isUnderWater) {
	   vec3 waterColor = vec3(0.0, 0.2, 0.4);
	   FragColor.rgb = mix(FragColor.rgb, waterColor, 0.5);
	   float underWaterFogFactor = clamp((distance - 0) / (64 - 0), 0.0, 1.0);
	   FragColor = vec4(mix(FragColor.rgb, waterColor, underWaterFogFactor), 1.0);
   }

   
}