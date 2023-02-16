#version 440 core

out vec4 fragColor;

in vec3 normal;
in vec4 color;
in vec3 fragment;

uniform vec3 lightPos;
uniform bool applyShading;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{	
	if (applyShading) {
		// Phong shading model

		// ambient light
		vec3 ambient = vec3(0.2f, 0.2f, 0.2f);

		//diffuse light
		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(fragment - lightPos);
		float diffuseValue = max(dot(-norm, lightDir), 0.0);
		vec3 diffuse = diffuseValue * lightColor;

		// specular light
		float specularStrength = 0.5f;
		vec3 viewDir = normalize(fragment - viewPos);
		vec3 reflectedDir = reflect(-lightDir, norm);
		float specValue = pow(max(dot(viewDir, reflectedDir), 0.0), 32);
		vec3 specular = specularStrength * specValue * lightColor;

		vec3 rgb = (diffuse + ambient + specular) * color.rgb;
		fragColor = vec4(rgb, 1.0f);
	} else { 
		fragColor = color;
	}
}