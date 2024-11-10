// Shader-ul de varfuri  
 
 #version 440


layout (location = 0)in vec3 a_position;
layout (location = 1)in vec3 a_color;
layout (location = 2)in vec3 a_norm;
layout (location = 3)in vec2 a_uv;

out vec4 Position; 
out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
out vec2 Uv;
 
uniform mat4 transfMatrix;
uniform mat4 modelMatrix;
 


void main(void)
  {
    Position = transfMatrix*vec4(a_position, 1.0);
	Color = a_color;
    Uv = a_uv;
    Normal=mat3(transpose(inverse(modelMatrix))) * a_norm; 
	FragPos = vec3(modelMatrix* vec4(a_position, 1.0f));
   } 
 