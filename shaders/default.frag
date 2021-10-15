#version 440 core
out vec4 FragColor;

in vec3 vertexColor;
in vec4 gl_FragCoord;

uniform ivec2 viewport;
uniform dvec2 origin;
uniform double scale;

#define N_ROOTS 3
const dvec2 roots[N_ROOTS] = {dvec2(0.5, 0.5), dvec2(0.5, -0.5), dvec2(-0.5, -0.5)};

#define COLOR_A vec3(0.5, 0.5, 0.5)
#define COLOR_B vec3(0.5, 0.5, 0.5)
#define COLOR_C vec3(2.0, 1.0, 0.0)
#define COLOR_D vec3(0.50, 0.20, 0.25)

vec4 get_root_color(int root) {
    return vec4(COLOR_A + COLOR_B * cos(6.28318 * (COLOR_C * float(root) / N_ROOTS + COLOR_D)), 1.0);
}

#define N_ITERS 50

double sq(double a) {
    return a * a;
}

dvec2 cprod(dvec2 a, dvec2 b) {
    return dvec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

dvec2 cdiv(dvec2 top, dvec2 bot) {
    double denom = sq(bot.x) + sq(bot.y);
    return dvec2(top.x * bot.x + top.y * bot.y, top.y * bot.x - top.x * bot.y) / denom;
}

struct RootDist {
    int root;
    double dist;
};

RootDist closest_root(dvec2 x) {
    double min_dist = distance(x, roots[0]);
    int root = 0;
    for (int i = 1; i < N_ROOTS; i++) {
        double dist = distance(x, roots[i]);
        if (dist < min_dist) {
            min_dist = dist;
            root = i;
        }
    }
    return RootDist(root, min_dist);
}

dvec2 poly(dvec2 x) {
    dvec2 product = dvec2(1, 0);
    for (int i = 0; i < N_ROOTS; i++) {
        dvec2 term = x - roots[i];
        product = cprod(product, term);
    }
    return product;
}

dvec2 newton(dvec2 x) {
    dvec2 deriv = dvec2(1, 0);
    dvec2 term = x - roots[0];
    dvec2 poly = term;
    for (int i = 1; i < N_ROOTS; i++) {
        term = x - roots[i];
        deriv = cprod(deriv, term) + poly;
        poly = cprod(term, poly);
    }
    return x - cdiv(poly, deriv);
}

void main() {
    FragColor = vec4(vertexColor, 1.0);
    dvec2 graphCoord = dvec2((2 * gl_FragCoord.x - viewport.x) / viewport.y, 2 * gl_FragCoord.y / viewport.y - 1) * scale + origin;

    for (int i = 0; i < N_ITERS; i++) {
        graphCoord = newton(graphCoord);
    }

    RootDist result = closest_root(graphCoord);
    FragColor = get_root_color(result.root);
}