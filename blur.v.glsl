
uniform mat4 m, v, p;
attribute vec3 v_coord;
attribute vec2 v_tex;

out vec4   gl_Position;

varying vec2 tex;

void main()
{
    mat4 mvp = p*v*m;
    gl_Position = mvp* vec4(v_coord, 1.0);
    tex = v_tex;
}
