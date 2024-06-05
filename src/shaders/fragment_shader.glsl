// #version 330 core

// struct Material {
//     vec3 diffuse;
//     vec3 specular;
//     vec3 ambient;
//     float shininess;
//     sampler2D textureMap;
// };

// in vec3 FragPos;
// in vec3 Normal;
// in vec2 TexCoord;

// out vec4 FragColor;

// uniform Material material;

// void main() {
//     vec3 lightPos = vec3(0.0, 0.0, 5.0);
//     vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
//     // Ambient
//     vec3 ambient = material.ambient;
    
//     // Diffuse
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(lightPos - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = diff * material.diffuse;
    
//     // Specular
//     vec3 viewDir = normalize(-FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = spec * lightColor * vec3(texture(material.specularMap, TexCoord));
//     // vec3 specular = spec * material.specular;
    
//     vec3 result = ambient + diffuse + specular;
//     FragColor = vec4(result, 1.0) * texture(material.textureMap, TexCoord);

//     // do nothing
//     // FragColor = texture(material.textureMap, TexCoord);
// }


#version 330 core

// struct Material {
//     sampler2D diffuseMap;
//     sampler2D specularMap;
//     float shininess;
// };

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// uniform Material material;

uniform float shininess;
// uniform vec4 u_diffuse ;
// uniform vec4 u_specular;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient
    vec3 ambient = vec3(0.1);

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec4 diffuse = diff * vec3(texture(material.diffuseMap, TexCoord));

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * vec3(texture(material.specularMap, TexCoord));

    // Result
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}