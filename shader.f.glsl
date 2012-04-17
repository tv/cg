#version 330

varying vec4 position;  // position of the vertex (and fragment) in world space
varying vec3 varyingNormalDirection;  // surface normal vector in world space
uniform mat4 m, v, p;
uniform mat4 v_inv;

uniform float[20] in_light0;
uniform float[17] in_material;

struct Light
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 spotDirection;
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


void main(void)
{
    lights[0] = Light(
        vec4(in_light0[0], in_light0[1], in_light0[2], in_light0[3]),
        vec4(in_light0[4], in_light0[5], in_light0[6], in_light0[7]),
        vec4(in_light0[8], in_light0[9], in_light0[10], in_light0[11]),
        in_light0[12], in_light0[13], in_light0[14],
        in_light0[15], in_light0[16],
        vec3(in_light0[17], in_light0[18], in_light0[19])
    );

    Material frontMaterial = Material(
        vec4(in_material[0], in_material[1], in_material[2], in_material[3]),
        vec4(in_material[4], in_material[5], in_material[6], in_material[7]),
        vec4(in_material[8], in_material[9], in_material[10], in_material[11]),
        vec4(in_material[12], in_material[13], in_material[14], in_material[15]),
        in_material[16]
    );

    vec3 normalDirection = normalize(varyingNormalDirection);
    vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
    vec3 lightDirection;
    float attenuation;

    vec3 combinedLightning = vec3(scene_ambient) * vec3(frontMaterial.ambient);

    for (int i = 0; i < numberOfLights; i++) {

        if (lights[i].position.w == 0.0) {
            // No attenuation, or direction. Just simple light.
            attenuation = 1.0;
            lightDirection = normalize(vec3(lights[i].position));
        } else { // Spot
            vec3 positionToLightSource = vec3(lights[i].position - position);

            float distance = length(positionToLightSource);

            lightDirection = normalize(positionToLightSource);

            // attenuation of the light
            attenuation = 1.0 / (lights[i].constantAttenuation
                               + lights[i].linearAttenuation * distance
                               + lights[i].quadraticAttenuation * distance * distance);

            if (lights[i].spotCutoff <= 90.0) { // spotlight?
                float clampedCosine = max(0.0, dot(-lightDirection, lights[i].spotDirection));
                if (clampedCosine < cos(radians(lights[i].spotCutoff))) { // outside of spotlight cone?
                    attenuation = 0.0;
                } else {
                    attenuation = attenuation * pow(clampedCosine, lights[i].spotExponent);
                }
            }
        }

        vec3 diffuseReflection = attenuation
                                * vec3(lights[i].diffuse) * vec3(frontMaterial.diffuse)
                                * max(0.0, dot(normalDirection, lightDirection));

        vec3 specularReflection;
        if (dot(normalDirection, lightDirection) < 0.0) { // light source on the wrong side?
          specularReflection = vec3(0.0, 0.0, 0.0);
        } else {
          specularReflection = attenuation * vec3(lights[i].specular) * vec3(frontMaterial.specular)
            * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), frontMaterial.shininess);
        }

        combinedLightning = combinedLightning + diffuseReflection + specularReflection;
    }



    gl_FragColor = vec4(combinedLightning, 1.0);
}
