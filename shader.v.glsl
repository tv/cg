attribute vec3 v_coord;
attribute vec3 v_normal;

varying vec4 position;  // position of the vertex (and fragment) in world space
varying vec3 vnormalDirection;  // surface normal vector in world space

uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
uniform mat4 v_inv;
uniform mat4 bias;


varying vec4 ShadowCoord;

void main()
{
    mat4 mvp = p*v*m;

    position = m * vec4(v_coord, 1.0);
    vnormalDirection = normalize(m_3x3_inv_transp * v_normal);

    ShadowCoord = bias * m*(vec4(v_coord, 1.0));

    gl_Position = mvp * vec4(v_coord, 1.0);
}
