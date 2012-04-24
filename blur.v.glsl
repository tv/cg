
uniform mat4 m, v, p;
attribute vec4 v_coord;
attribute vec2 v_tex;

out vec4   gl_Position;

varying vec2 tex;

void main()
{
    mat4 mvp = p*v*m;
    gl_Position = mvp* v_coord;
    tex = v_tex;
}
