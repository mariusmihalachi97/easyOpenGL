
// Shader-ul de fragment / Fragment shader  
 
 #version 440

in vec3 FragPos;  
in vec3 Normal; 
in vec3 Color;
in vec2 Uv;
in vec3 ringPos;

out vec4 out_Color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos;

uniform sampler2D texture1;

void main(void)
  {
  	// Ambient
    vec3 glow = vec3(0.5, 0.5, 0.5);

    float ambientStrength = 0.01f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
	float diffuse_strength = 0.8f;
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    vec3 lightDirNorm = normalize(lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diffuse_strength * diff * lightColor;
    
    // Specular
    float specularStrength = 0.07f;
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 H = normalize(viewDir + lightDirNorm);
	vec3 specular = vec3(0.0,0.0,0.0);
	//if(diff>0.0)
    {vec3 reflectDir = reflect(-lightDirNorm, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5);
	float spec = pow(max(dot(norm, H), 0.0), 9);
    specular = specularStrength * spec * lightColor;  
        }
    float d = sqrt(dot(lightDir,lightDir));
	float result = dot(normalize(ringPos), vec3(0.0,1.0,0.0));   	
    if(result<0)
	result = 1.0-dot(-normalize(ringPos), vec3(0.0,1.0,0.0));
	else result = 1.0 - result;
	if(result<0.2)discard;
	
    result = result;
	vec2 coord_text = vec2(result, 0.5);
	out_Color = vec4(texture2D(texture1, coord_text).xyz, 1.0);//;
  }
 