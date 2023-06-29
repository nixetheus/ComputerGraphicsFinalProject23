#version 450

layout(location = 0) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform GlobalUniformBufferObject {
	vec3 lightDir;
	vec4 lightColor;
	vec3 eyePos;
} gubo;

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);

	vec3 Diffuse = texture(tex, fragUV).rgb * 0.95f * clamp(dot(Norm, lightDir),0.0,1.0);
	vec3 Ambient = texture(tex, fragUV).rgb * 0.05f;
	outColor = vec4(clamp((Diffuse + Specular) * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}