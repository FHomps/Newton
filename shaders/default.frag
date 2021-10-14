#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec4 gl_FragCoord;

uniform ivec2 viewport;
uniform vec2 origin;
uniform float scale;

#define N_ROOTS 3
const vec2 roots[N_ROOTS] = {vec2(0.5, 0.5), vec2(0.5, -0.5), vec2(-0.5, -0.5)};

#define COLOR_A vec3(0.5, 0.5, 0.5)
#define COLOR_B vec3(0.5, 0.5, 0.5)
#define COLOR_C vec3(2.0, 1.0, 0.0)
#define COLOR_D vec3(0.50, 0.20, 0.25)

vec4 get_root_color(int root) {
    return vec4(COLOR_A + COLOR_B * cos(6.28318 * (COLOR_C * float(root) / N_ROOTS + COLOR_D)), 1.0);
}

#define N_ITERS 20

float sq(float a) {
    return a * a;
}

vec2 cprod(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

vec2 cdiv(vec2 top, vec2 bot) {
    float denom = sq(bot.x) + sq(bot.y);
    return vec2(top.x * bot.x + top.y * bot.y, top.y * bot.x - top.x * bot.y) / denom;
}

float dist(vec2 a, vec2 b) {
    return sqrt(sq(a.x - b.x) + sq(a.y - b.y));
}

struct RootDist {
    int root;
    float dist;
};

RootDist closest_root(vec2 x) {
    float min_dist = dist(x, roots[0]);
    int root = 0;
    for (int i = 1; i < N_ROOTS; i++) {
        float dist = dist(x, roots[i]);
        if (dist < min_dist) {
            min_dist = dist;
            root = i;
        }
    }
    return RootDist(root, min_dist);
}

vec2 poly(vec2 x) {
    vec2 product = vec2(1, 0);
    for (int i = 0; i < N_ROOTS; i++) {
        vec2 term = x - roots[i];
        product = cprod(product, term);
    }
    return product;
}

vec2 newton(vec2 x) {
    vec2 terms[N_ROOTS];
    vec2 polys[N_ROOTS];
    vec2 deriv = vec2(1, 0);
    terms[0] = x - roots[0];
    polys[0] = terms[0];
    for (int i = 1; i < N_ROOTS; i++) {
        terms[i] = x - roots[i];
        polys[i] = cprod(terms[i], polys[i-1]);
        deriv = cprod(deriv, terms[i]) + polys[i-1];
    }
    return x - cdiv(polys[N_ROOTS - 1], deriv);
}

void main() {
    FragColor = vec4(vertexColor, 1.0);
    vec2 graphCoord = vec2((2 * gl_FragCoord.x - viewport.x) / viewport.y, 2 * gl_FragCoord.y / viewport.y - 1) * scale + origin;

    for (int i = 0; i < N_ITERS; i++) {
        graphCoord = newton(graphCoord);
    }

    RootDist result = closest_root(graphCoord);
    FragColor = get_root_color(result.root);
}