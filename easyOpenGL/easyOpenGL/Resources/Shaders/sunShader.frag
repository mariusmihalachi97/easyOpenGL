
// Shader-ul de fragment / Fragment shader  
 
 #version 440

in vec3 FragPos;  
in vec3 Normal; 
in vec3 Color;
in vec2 Uv;

out vec4 out_Color;

uniform sampler2D texture1;

void main(void)
  {
	vec3 result = 1.1 * (texture2D(texture1, Uv).xyz);   		    
	out_Color = vec4(result, 1.0f);
  }
 