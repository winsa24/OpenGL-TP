#version 330 core	     // Minimal GL version support expected from the GPU
uniform vec3 camPos;
in vec3 fPosition;
in vec3 fColor;
//in vec3 fNormal
out vec4 color;	  // Shader output: the color response attached to this fragment

void main() {
	vec3 n = normalize(fColor);
	vec3 l = normalize(vec3(1.0, 1.0, 0.0)); // light direction vector (hard-coded just for now)
// TODO: vec3 v = calculate view vector
	vec3 v = camPos - fPosition;
	
// TODO: vec3 r = calculate reflection vector
	vec3 r = 2 * (n * l) * n - l;

// TODO: vec3 ambient = set an ambient color
	vec3 ambient = vec3(0.3, 0.3, 0.3);
// TODO: vec3 diffuse = calculate the diffuse lighting
	//vec4 n1 = vec4(n)
	vec3 diffuse = vec3(1.0) * dot(n, l);
// TODO: vec3 specular = calculate the specular lighting
	vec3 specular = (r * v);
	
	color = vec4(ambient + diffuse + specular, 1.0); // Building RGBA from RGB.

	//color = vec4(fColor, 1.0); // build an RGBA from an RGB
	//color = vec4(normalize(fColor), 1.0);
	//color = vec4(normalize(fNormal), 1.0);
	//color = vec4(fNormal, 1.0);

}
