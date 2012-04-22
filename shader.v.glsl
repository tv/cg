attribute vec3 v_coord;
attribute vec3 v_normal;

varying vec4 position;  // position of the vertex (and fragment) in world space
varying vec3 vnormalDirection;  // surface normal vector in world space
varying vec3 normal;

uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
uniform mat4 v_inv;

uniform mat4 in_light0_v; // cameras view
uniform mat4 in_light0_v_inv; // cameras view
uniform mat4 in_light0_p; // cameras view

varying vec4 ShadowCoord;

void main()
{

    mat4 bias = mat4(0.5, 0.0, 0.0, 0.0,
                     0.0, 0.5, 0.0, 0.0,
                     0.0, 0.0, 0.5, 0.0,
                     0.5, 0.5, 0.5, 1.0);

    mat4 mvp = p*v*m;

    position = m * vec4(v_coord, 1.0);

    normal = v_normal;
    vnormalDirection = normalize(m_3x3_inv_transp * v_normal);

    ShadowCoord = bias * in_light0_p * in_light0_v * position;

    gl_Position = mvp * vec4(v_coord, 1.0);
}
