#version 440 core

out vec4 fragColor;

in vec3 normal;
in vec4 color;
in vec3 fragment;
in vec2 textCoord;

uniform bool applyShading;
uniform bool applyTexture;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform sampler2D _texture;

void main()
{	
	if (applyShading) {
		// Phong shading model

		// ambient light
		float ambientStrength = 0.2f;
		vec3 ambient = ambientStrength * lightColor;

		//diffuse light
		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(lightPos - fragment);
		float diffuseValue = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diffuseValue * lightColor;

		// specular light
		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - fragment);
		vec3 reflectedDir = reflect(-lightDir, norm);
		float specValue = pow(max(dot(viewDir, reflectedDir), 0.0), 32);
		vec3 specular = specularStrength * specValue * lightColor;

		fragColor = vec4((diffuse + ambient + specular) * color.rgb, color.a);

	} else { 
		fragColor = color;
	}
	if (applyTexture) { 
		fragColor *= texture(_texture, textCoord);
	}
}
