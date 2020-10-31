#version 330 core	     // Minimal GL version support expected from the GPU
// fragmentShader.glsl ...
struct Material {
    // ...
    sampler2D albedoTex;
};
uniform Material material;
uniform vec3 camPos;
in vec3 fPosition;
in vec3 fColor;
in vec3 meshColor;
in vec2 fTexCoord;
//in vec3 fNormal
out vec4 color;	  // Shader output: the color response attached to this fragment

void main() {
    vec3 texColor = texture(material.albedoTex, fTexCoord).rgb; // sample the texture color
    //color = vec4(texColor, 1.0);
    
	vec3 n = normalize(fColor);
	vec3 l = normalize(vec3(1.0, 1.0, 0.0)); // light direction vector (hard-coded just for now)

// TODO: vec3 v = calculate view vector
	vec3 v = normalize(camPos - fPosition);
	
// TODO: vec3 r = calculate reflection vector
	vec3 r = normalize(2 * dot(n, l) * n - l);

// TODO: vec3 ambient = set an ambient color
	vec3 ambient = vec3(0.0, 0.0, 0.0);
//    vec3 ambient = meshColor;
//    vec3 ambient = texColor;
// TODO: vec3 diffuse = calculate the diffuse lighting
	vec3 lightColor = vec3(1.0, 1.0, 1.0);	
	vec3 diffuse = max(dot(n,l),0.0) *lightColor;
	//vec3 diffuse = l * max(dot(n, l),0.0);
// TODO: vec3 specular = calculate the specular lighting
	vec3 specular = 0.5 * pow(max(dot(v,r),0), 64)* lightColor;
	
	color = vec4(ambient + diffuse + specular + texColor, 1.0); // Building RGBA from RGB.
    
	//color = vec4(fColor, 1.0); // build an RGBA from an RGB
	//color = vec4(normalize(fColor), 1.0);
	//color = vec4(normalize(fNormal), 1.0);
	//color = vec4(fNormal, 1.0);

}
