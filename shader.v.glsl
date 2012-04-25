#version 330


attribute vec4 v_coord;
attribute vec3 v_normal;

varying vec4 vv_position;  // position of the vertex (and fragment) in world space
varying vec3 vv_normalDirection;  // surface normal vector in world space
varying vec3 vv_normal;

uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
uniform mat4 v_inv;

uniform mat4 in_light0_v; // cameras view
uniform mat4 in_light0_v_inv; // cameras view
uniform mat4 in_light0_p; // cameras view

uniform mat4 in_light1_v; // cameras view
uniform mat4 in_light1_v_inv; // cameras view
uniform mat4 in_light1_p; // cameras view

varying vec4[2] ShadowCoord;

void main()
{

    mat4 bias = mat4(0.5, 0.0, 0.0, 0.0,
                     0.0, 0.5, 0.0, 0.0,
                     0.0, 0.0, 0.5, 0.0,
                     0.5, 0.5, 0.5, 1.0);


    mat4 mvp = p * v * m;

    vv_position = m * v_coord;

    vv_normal = v_normal;
    vv_normalDirection = normalize(m_3x3_inv_transp * v_normal);


    ShadowCoord[0] = bias * in_light0_p * in_light0_v * m * v_coord;
    ShadowCoord[0] = ShadowCoord[0] / ShadowCoord[0].w;

    ShadowCoord[1] = bias * in_light1_p * in_light1_v * m * v_coord;
    ShadowCoord[1] = ShadowCoord[1] / ShadowCoord[1].w;

    gl_Position = mvp * v_coord;
}
