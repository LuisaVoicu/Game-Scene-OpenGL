#version 410 core

in vec3 colour;
in vec2 passTexture;

out vec4 fragmentColour;
uniform sampler2D diffuseTexture;

void main() {
    vec4 colorFromTexture = texture(diffuseTexture,passTexture);
	colorFromTexture.a *= 1.0; 
	if(colorFromTexture.a < 0.1)
		discard;
	fragmentColour = colorFromTexture;
}