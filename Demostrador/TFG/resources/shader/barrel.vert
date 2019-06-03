#version 120

varying out vec2 texPos;

void main() {
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texPos = gl_MultiTexCoord0.xy;
}