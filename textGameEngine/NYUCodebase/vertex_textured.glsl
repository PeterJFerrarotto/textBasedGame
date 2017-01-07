attribute vec4 position;
attribute vec2 texCoord;
attribute vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec4 vertexColor;
varying vec2 texCoordVar;

void main()
{
	vec4 p = viewMatrix * modelMatrix  * position;
    texCoordVar = texCoord;
	vertexColor = color;
	gl_Position = projectionMatrix * p;
}