#version 330

varying vec4 position;
varying vec3 vnormalDirection;

varying vec4 ShadowCoord;

uniform sampler2D ShadowMap;

uniform mat4 m, v, p;
uniform mat4 v_inv;

uniform bool noFragment;


uniform float[23] in_light0;
uniform float[17] in_material;

uniform int debug_mode;

struct Light
{
  vec3 position;
  float simpleLight;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 direction;

};

struct Material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 emission;
  vec4 specular;
  float shininess;
};

const int numberOfLights = 1;
Light lights[numberOfLights];

vec4 scene_ambient = vec4(0.02, 0.02, 0.02, 1.0);


float chebyshevUpperBound(sampler2D map, vec4 SC)
{

    vec4 SCpP = SC / SC.w;

    float distance = ShadowCoord.z;
    vec2 moments = texture2D(map, SCpP.xy).rg;
    return moments.x;
    if (distance <= moments.x) {
        return 1.0;
    }

    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);

    float d = distance - moments.x;

    return variance / (variance + d*d);
}

vec4 white()
{
    return vec4(1.0, 1.0, 1.0, 1.0);
}

vec3 ambient(Material mat)
{
    return vec3(scene_ambient) * vec3(mat.ambient);
}

float attenuation(Light light)
{
    float attenuation;
    if (light.simpleLight == 0.0) {
        // No attenuation, or direction. Just simple light.
        attenuation = 1.0;
    } else { // Spot
        vec3 positionToLightSource = light.position - vec3(position);

        float distance = length(positionToLightSource);

        vec3 lightDirection = normalize(positionToLightSource);

        // attenuation of the light
        attenuation = 1.0 / (light.constantAttenuation
                           + light.linearAttenuation * distance
                           + light.quadraticAttenuation * distance * distance);

        if (light.spotCutoff <= 90.0) { // spotlight?
            float clampedCosine = max(0.0, dot(lightDirection, light.direction));

            if (clampedCosine < cos(radians(light.spotCutoff))) { // outside of spotlight cone?
                attenuation = 0.0;
            } else {
                attenuation = attenuation * pow(clampedCosine, light.spotExponent);
            }
        }
    }
    return attenuation;
}

vec3 diffuseReflection(Light light, Material mat, float attenuation, vec3 normalDirection, vec3 lightDirection)
{
    return attenuation
        * vec3(light.diffuse) * vec3(mat.diffuse)
        * max(0.0, dot(normalDirection, lightDirection));
}

vec3 specularReflection(Light light, Material mat, float attenuation, vec3 normalDirection, vec3 lightDirection, vec3 viewDirection)
{
    vec3 specularReflection;
    if (dot(normalDirection, lightDirection) < 0.0) { // light source on the wrong side?
      specularReflection = vec3(0.0, 0.0, 0.0);
    } else {
      specularReflection = attenuation * vec3(light.specular) * vec3(mat.specular)
        * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), mat.shininess);
    }

    return specularReflection;
}

void main(void)
{


    if (debug_mode == 2) {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    Material frontMaterial = Material(
        vec4(in_material[0], in_material[1], in_material[2], in_material[3]),
        vec4(in_material[4], in_material[5], in_material[6], in_material[7]),
        vec4(in_material[8], in_material[9], in_material[10], in_material[11]),
        vec4(in_material[12], in_material[13], in_material[14], in_material[15]),
        in_material[16]
    );

    lights[0] = Light(
        vec3(in_light0[0], in_light0[1], in_light0[2]),
        in_light0[3],
        vec4(in_light0[4], in_light0[5], in_light0[6], in_light0[7]),
        vec4(in_light0[8], in_light0[9], in_light0[10], in_light0[11]),
        in_light0[12], in_light0[13], in_light0[14],
        in_light0[15], in_light0[16],
        vec3(in_light0[17], in_light0[18], in_light0[19])
    );

    vec3 normalDirection = normalize(vnormalDirection);
    vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));


    vec3 combinedLightning = ambient(frontMaterial);

    for (int i = 0; i < numberOfLights; i++) {
        float shadow = 1.0-chebyshevUpperBound(ShadowMap, ShadowCoord);

        float attenuation = attenuation(lights[i]);

        vec3 positionToLightSource = lights[i].position - vec3(position);
        vec3 lightDirection = normalize(lights[i].direction);

        vec3 diffuseReflection = diffuseReflection(
                    lights[i], frontMaterial, attenuation, normalDirection, lightDirection);

        vec3 specularReflection = specularReflection(
                    lights[i], frontMaterial, attenuation, normalDirection, lightDirection, viewDirection);



        if (debug_mode == 3) {
            combinedLightning += diffuseReflection + specularReflection;
        } else {
            combinedLightning += shadow * (diffuseReflection + specularReflection);
        }


        if (debug_mode == 1) {
            combinedLightning = vec3(diffuseReflection.x, specularReflection.x, shadow);
        }


    }



    gl_FragColor = vec4(combinedLightning, 1.0);



}
