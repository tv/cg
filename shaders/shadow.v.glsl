#version 150
uniform mat4 m, v, p;
attribute vec4 v_coord;

uniform mat4 v_inv;


varying vec4 position;

void main(void)
{
    mat4 mvp = p * v *m;

    gl_Position = mvp * v_coord;
    position = gl_Position;
}
