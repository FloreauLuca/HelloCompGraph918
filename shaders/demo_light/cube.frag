
#version 330 core

out vec4 FragColor;

struct FlashLight {
    vec3 position;  
    vec3 direction;
    float cutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  


in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoord;

uniform FlashLight flashLight[3];

uniform Material material;
  
uniform vec3 viewPos;

uniform sampler2D ourTexture;

vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
  	
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i < 3; i++)
    {
    	result += CalcFlashLight(flashLight[i], norm, FragPos, viewDir);
    }
  
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a spot light.
vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    
    float theta = dot(lightDir, normalize(-light.direction));
    if(theta > light.cutOff)
    {
        return (ambient + diffuse + specular);
    } else{
        return (light.ambient * material.ambient);
    }
}
