#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	int screenW;
	int screenH;
	float currentTime;
} ubo;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

float delay = 2.5f;
float startTime = 1.0f;
float endTime = 5.0f;
float duration = endTime - startTime;
void main() {

    vec4 color;
    if (texture(tex, fragUV).a > 0.01 && ubo.currentTime > startTime && ubo.currentTime < endTime + delay + startTime)
        color = vec4(texture(tex, fragUV).rgb, 1.0f - min(1.0f, (ubo.currentTime - delay - startTime) / duration));	// output color
    else
        discard;

    outColor = color;
}