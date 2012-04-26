layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

uniform mat4 m ,v, p;

varying in vec3 normal[];


void main() {

    mat4 mvp = p*v*m;
    for(int i = 0; i < gl_VerticesIn; ++i)
    {
        gl_Position = mvp * gl_PositionIn[i];
        EmitVertex();

        gl_Position = mvp * (gl_PositionIn[i] + (vec4(normal[i], 0) * 0.6));
        EmitVertex();

        EndPrimitive();
    }
}
