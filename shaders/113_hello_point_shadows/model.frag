
#version 330 core
out vec4 FragColor;

struct Material {
    float shininess;
}; 

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

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

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
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    
    
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;  
    
    vec3 specular = light.specular * spec * vec3(0);  
    
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
        FragColor = vec4(light.ambient * texture(texture_diffuse1, TexCoords).rgb, 1.0);
    }
} 