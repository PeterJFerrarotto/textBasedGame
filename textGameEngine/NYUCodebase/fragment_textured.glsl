
uniform sampler2D diffuse;
varying vec2 texCoordVar;
varying vec4 vertexColor;

void main() {
	gl_FragColor = texture2D(diffuse, texCoordVar) * vertexColor;
}