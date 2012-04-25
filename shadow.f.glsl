varying vec4 position;

uniform int debug_mode;

void main(void)
{
    float d = position.z / 50;

    gl_FragColor = vec4(d, d, d, 1.0);
    return;

    float depth = position.z;// / position.w;
    depth = (depth * 0.5 + 0.5) / 50;

    float moment1 = depth;
    float moment2 = depth * depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    moment2 += 0.25 * ((dx * dx) + (dy * dy));

    if (debug_mode == 4) {
        gl_FragColor = vec4(moment1, moment2, 0.0, 1.0);
    } else {
        gl_FragColor = vec4(moment1, moment2, 0.0, 0.0);
    }
}
