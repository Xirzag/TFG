#version 120

varying in vec2 texPos;

uniform sampler2D texture;
uniform float BarrelPower;

vec2 Distort(vec2 pos) {
	vec2 p = 2.0 * pos - 1.0;

    float r2 = p.x * p.x + p.y * p.y;
    float newRadius = (1 + BarrelPower*r2);
    p = p * newRadius;

	return (p + 1.0) / 2.f;
}

void main() {
	
	gl_FragColor = texture2D(texture, Distort(texPos));

}
