
#version 330 core

out vec4 FragColor;

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;

    int type;
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

uniform Material material;

uniform Light light;
  
uniform vec3 viewPos;

uniform sampler2D ourTexture;

void main()
{   
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    if (light.type == 0)
    {
        lightDir = normalize(-light.direction);
    }

    float diff = max(dot(norm, lightDir), 0.0); 
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // ambient
    vec3 ambient  = light.ambient * material.ambient;
    
    // diffuse 
    vec3 diffuse  = light.diffuse * (diff * material.diffuse);
    vec3 specular = light.specular * (spec * material.specular);  
    
    float theta = dot(lightDir, normalize(-light.direction));
    if (light.type == 3)
    {
        theta     = dot(lightDir, normalize(-light.direction));
        float epsilon   = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  
        diffuse  *= intensity;
        specular *= intensity;
    }
    if (light.type == 1 || light.type == 3)
    {
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));    
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
    }
    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result, 1.0);
    
    if(theta > light.cutOff || light.type != 2)
    {
        FragColor = vec4(result, 1.0);
    } else{
        FragColor = vec4(light.ambient * vec3(material.ambient), 1.0);
    }
}
