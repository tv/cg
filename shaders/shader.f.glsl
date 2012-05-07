#version 150

varying vec4 vv_position;  // position of the vertex (and fragment) in world space
varying vec3 vv_normalDirection;  // surface normal vector in world space
varying vec3 vv_normal;
varying vec2 vv_tex;

uniform mat4 m, v, p;
uniform mat4 v_inv;

uniform bool noFragment;


const int numberOfLights = 2;
varying vec4[2] ShadowCoord;
uniform sampler2DShadow in_light0_shadow;
uniform float[23] in_light0;
uniform sampler2DShadow in_light1_shadow;
uniform float[23] in_light1;


uniform float[17] in_material;

uniform sampler2D texture;

uniform int debug_mode;

varying float vv_shade;


struct Light
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 direction;
  sampler2DShadow shadowMap;
};

struct Material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 emission;
  vec4 specular;
  float shininess;
};

Light lights[numberOfLights];

vec4 scene_ambient = vec4(0.02, 0.02, 0.02, 1.0);

float lookup(sampler2DShadow shadowMap, vec4 SC, float x, float y)
{
    vec4 shadowcoord = vec4(SC.xy + vec2(x,y), SC.z ,SC.w);
    float depth = shadow2DProj(shadowMap, shadowcoord).x;

    return depth;
    if (depth != 1.0) {
        return 0.2;
    }
    return 1.0;
}

float shadows(Light light, vec4 SC, vec4 lightDirection)
{
    float sum = 0.0;

    float x,y;

    for (y = -1.5; y <= 1.5; y += 1.0) {
      for (x = -1.5; x <= 1.5; x += 1.0) {
        sum += lookup(light.shadowMap, SC, x, y);
      }
    }

    return sum/ 16.0;


    float distance = lightDirection.w / 50;
    vec2 moments =  shadow2DProj(light.shadowMap, SC).r;
    if (distance <= moments.x) {
        return 1.0;
    }

    return 0.0;

    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.002);

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

vec4 lightDirection(Light light) // w = distance to light;
{
    if (light.position.w == 0.0) {
        return vec4(normalize(vec3(light.position)), 1.0);
    } else {
        vec3 pToLight = vec3(light.position - vv_position);

        float distance = length(pToLight);

        pToLight = normalize(pToLight);

        return vec4(pToLight, distance);
    }
}

float attenuation(Light light, vec4 lightDirection)
{
    float attenuation;
    if (light.position.z == 0.0) {
        // No attenuation, or direction. Just simple light.
        attenuation = 1.0;
    } else { // Spot

        float distance = lightDirection.w;

        // attenuation of the light
        attenuation = 1.0 / (light.constantAttenuation
                           + light.linearAttenuation * distance
                           + light.quadraticAttenuation * distance * distance);

        if (light.spotCutoff <= 90.0) { // spotlight?
            float clampedCosine = max(0.0, dot(vec3(lightDirection), light.direction));

            if (clampedCosine < cos(radians(light.spotCutoff))) { // outside of spotlight cone?
                attenuation = 0.0;
            } else {
                attenuation = attenuation * pow(clampedCosine, light.spotExponent);
            }
        }
    }
    return attenuation;
}

vec3 diffuseReflection(Light light, Material mat, float attenuation,
                       vec3 normalDirection, vec3 lightDirection)
{
    return attenuation
        * vec3(light.diffuse) * vec3(mat.diffuse)
        * max(0.0, dot(normalDirection, lightDirection));
}

vec3 specularReflection(Light light, Material mat, float attenuation,
                        vec3 normalDirection, vec3 lightDirection, vec3 viewDirection)
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
        gl_FragColor = vec4(vv_shade, texture2D(texture, vv_tex).r, 1.0, 1.0);
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
        vec4(in_light0[0], in_light0[1], in_light0[2], in_light0[3]),
        vec4(in_light0[4], in_light0[5], in_light0[6], in_light0[7]),
        vec4(in_light0[8], in_light0[9], in_light0[10], in_light0[11]),

        in_light0[12], in_light0[13], in_light0[14],
        in_light0[15], in_light0[16],

        vec3(-in_light0[17], -in_light0[18], -in_light0[19]),

        in_light0_shadow
    );

    lights[1] = Light(
        vec4(in_light1[0], in_light1[1], in_light1[2], in_light1[3]),
        vec4(in_light1[4], in_light1[5], in_light1[6], in_light1[7]),
        vec4(in_light1[8], in_light1[9], in_light1[10], in_light1[11]),

        in_light1[12], in_light1[13], in_light1[14],
        in_light1[15], in_light1[16],

        vec3(-in_light1[17], -in_light1[18], -in_light1[19]),

        in_light1_shadow
    );

    //lights[0].spotCutoff = 10.0f;

    vec3 normalDirection = normalize(vv_normalDirection);
    vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - vv_position));

    vec3 combinedLightning = ambient(frontMaterial);

    for (int i = 0; i < numberOfLights; i++) {

        vec4 lightDirection = lightDirection(lights[i]);

        float shadow = shadows(lights[i], ShadowCoord[i], lightDirection);

        float attenuation = attenuation(lights[i], lightDirection);

        vec3 diffuseReflection = diffuseReflection(
            lights[i],
            frontMaterial,
            attenuation,
            normalDirection,
            vec3(lightDirection)
        );

        vec3 specularReflection = specularReflection(
            lights[i],
            frontMaterial,
            attenuation,
            normalDirection,
            vec3(lightDirection),
            viewDirection
        );


        if (debug_mode == 3) {
            combinedLightning += diffuseReflection + specularReflection;
        } else {
            combinedLightning += shadow * (diffuseReflection + specularReflection);
        }


        if (debug_mode == 1) {
            combinedLightning = vec3(diffuseReflection.r, specularReflection.r, shadow);
        }


    }


        combinedLightning *= texture2D(texture, vv_tex);

        combinedLightning *= vv_shade;

    gl_FragColor = vec4(combinedLightning, 1.0);

}
