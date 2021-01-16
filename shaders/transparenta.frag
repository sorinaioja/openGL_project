#version 410 core


in vec2 fTexCoords;

out vec4 fColor;

//texture
uniform sampler2D diffuseTexture;




void main() 
{
	
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	fColor.a= 0.7f;
	

    
}
