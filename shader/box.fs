#version 330 core
out vec4 FragColor;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} fs_in;
uniform sampler2D texture_diffuse;
uniform sampler2D shadowMap;
// uniform vec3 lightDir;
// uniform vec3 lightColor;
uniform vec3 viewPos;


float ShadowCalculation(vec4 fragPosLightSpace, float cosVal){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0||
    projCoords.y < - 1.0 || projCoords.x < -1.0 ) return 0.0;
    projCoords = projCoords * 0.5 + 0.5;
    float depthReference = texture(shadowMap, projCoords.xy).r;
    float depthNow = projCoords.z;
    //为了避免条纹现象（该现象产生的原因是每一像素的深度纹理无法准确表示该像素所在局部的所有深度值）
    float bias = max(0.005 * (1-cosVal), 0.001);

    float shadow = 0.0;
    vec2 texSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++){
        for (int y = -1; y <= 1; y++){
            float pcfDepth = texture(shadowMap, vec2(x, y) * texSize + projCoords.xy).r;
            shadow +=  ((depthNow - bias) > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}


void main()
{
    vec3 lightDir = vec3(1, 1, 1);
    vec3 lightColor = vec3(1, 1, 1);
    vec4 color4 = texture(texture_diffuse, fs_in.TexCoord);
    if(color4.a == 0.0f) discard; //透明则不绘制
    vec3 color = color4.rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 ambientCof = 0.5 * lightColor; //ambient
    float cosVal = dot(normal, lightDir);
    vec3 diffuseCof = max(cosVal, 0.0) * lightColor; //diffuse
    // // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfDir = normalize(viewDir + lightDir);
    vec3 specularCof = pow(max(dot(halfDir, normal), 0.0), 64.0) * lightColor;
    // // // calculate total color
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, cosVal);
    vec3 lighting = (ambientCof + (1.0 - shadow) * (diffuseCof + specularCof)) * color;
    FragColor = vec4(lighting, 1.0);
    //FragColor = vec4(1.0, 0.0 ,0.0 ,1);
}