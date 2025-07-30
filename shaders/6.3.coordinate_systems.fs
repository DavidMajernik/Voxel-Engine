#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float vAO;
in vec3 FragPos;
in float isRenderingWaterFlag;
in float isRenderingBillboardFlag;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec4 FogColor;
uniform float FogMaxDist;
uniform float FogMinDist;
uniform vec3 CameraPos;
uniform bool isUnderWater;

uniform vec3 SunDirection;
uniform vec3 SunColor;
uniform float SunIntensity;
uniform vec3 AmbientColor;

void main()
{

   vec4 texColor = texture(texture1, TexCoord);
   if(texColor.a < 0.1) {
		   discard;
	}
   FragColor = texColor; 
   FragColor.rgb *= mix(0.7, 1.0, vAO); 

   float distance = length(FragPos - CameraPos);

	// Simple Lambertian diffuse
	float diffuse = max(dot(Normal, -SunDirection), 0.3);
	// Combine ambient and sunlight
	vec3 lighting = AmbientColor + SunColor * SunIntensity * diffuse;
	FragColor.rgb *= lighting;


   //Linear fog
   float fogFactor = clamp((distance - FogMinDist) / (FogMaxDist - FogMinDist), 0.0, 1.0);

   //apply fog and set final alpha
   float finalAlpha = FragColor.a;
   if (isRenderingWaterFlag > 0.5) {
       finalAlpha = 0.65;
   } else if (isRenderingBillboardFlag > 0.5) {
       finalAlpha = FragColor.a; 
   } else {
       finalAlpha = 1.0;
   }
   FragColor = vec4(mix(FragColor.rgb, (FogColor.rgb)*SunIntensity, fogFactor), finalAlpha);

   //underwater effect
   if(isUnderWater) {
	   vec3 waterColor = vec3(0.0, 0.2, 0.4);
	   FragColor.rgb = mix(FragColor.rgb, waterColor, 0.5);
	   float underWaterFogFactor = clamp((distance - 0) / (64 - 0), 0.0, 1.0);
	   FragColor = vec4(mix(FragColor.rgb, waterColor, underWaterFogFactor), finalAlpha);
   }
   
}