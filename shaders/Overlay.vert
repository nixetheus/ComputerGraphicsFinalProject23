#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	int screenW;
	int screenH;
	float currentTime;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;

void main() {

	// Change position to match with Ar = w / h
	vec2 newPos = inPosition;
	if (ubo.screenW > ubo.screenH)
		newPos.x = sign(inPosition.x) * ubo.screenH / ubo.screenW;
	else
		newPos.y = sign(inPosition.y) * ubo.screenW / ubo.screenH;

	gl_Position = vec4(newPos, 0.5f, 1.0f);
	outUV = inUV;
}