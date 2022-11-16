#version 330 core
in vec3 Normal; 
in vec3 FragPos; 
in vec3 LightPos;

in vec2 TexCoords;
out vec4 FragColor;

struct SpotLight {
    vec3  position;
    vec3  direction;
    float cutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

  
uniform SpotLight spotLight;
uniform vec3 viewPos;

float theta = dot(lightDir, normalize(-light.direction));
    
if(theta > light.cutOff) 
{       
  // do lighting calculations
}
else  // else, use ambient light so scene isn't completely dark outside the spotlight.
  color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);