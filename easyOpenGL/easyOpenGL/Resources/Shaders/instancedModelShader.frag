
// Shader-ul de fragment / Fragment shader  
 
 #version 440

in vec3 FragPos;  
in vec3 Normal; 
in vec3 Color;
//in vec2 uv;

out vec4 out_Color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos;

//uniform sampler2D texture1;

void main(void)
  {
  //vec3 col = texture2D(texture1, uv);
  	// Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
	float diffuse_strength = 0.6f;
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    vec3 lightDirNorm = normalize(lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diffuse_strength * diff * lightColor;
    
    // Specular
    float specularStrength = 0.9f;
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 H = normalize(viewDir + lightDirNorm);
	vec3 specular = vec3(0.0,0.0,0.0);
	if(diff>0.00001)
    {vec3 reflectDir = reflect(-lightDirNorm, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5);
	float spec = pow(max(dot(norm, H), 0.0), 8);
    specular = specularStrength * spec * lightColor;  
}
    float d = sqrt(dot(lightDir,lightDir));
    vec3 result = 1/(1+0.01*d+0.0001*d)*(ambient + diffuse + specular) * objectColor;
	out_Color = vec4(result, 1.0f);
	//if(Color.r > 0.95)discard;
  }
 