layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

uniform mat4 m ,v, p;

in vec3 vnormalDirection[];

out vec3 weight;

void main() {

    mat4 mvp = p*v*m;
    weight = vec3(1.0, 0.0, 0.0);
    gl_Position = mvp * gl_in[0].gl_Position;
    EmitVertex();

    weight = vec3(0.0, 1.0, 0.0);
    gl_Position = mvp * gl_in[1].gl_Position;
    EmitVertex();

    weight = vec3(0.0, 0.0, 1.0);
    gl_Position = mvp * gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
