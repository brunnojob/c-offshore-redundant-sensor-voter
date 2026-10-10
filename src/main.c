#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

static void fail(const char *message) { fprintf(stderr, "%s\n", message); exit(2); }
double number(const char *s) {
    char *end; errno = 0; double value = strtod(s, &end);
    if (errno || end == s || *end || !isfinite(value)) fail("invalid finite number");
    return value;
}
static size_t fields(char *line, char **out, size_t capacity) {
    line[strcspn(line, "\r\n")] = 0;
    size_t count = 0;
    char *p = line;
    while (*p) {
        if (count == capacity) fail("too many fields");
        if (*p == ',') fail("empty field");
        out[count++] = p;
        char *separator = strchr(p, ',');
        if (!separator) break;
        *separator = 0; p = separator + 1;
        if (!*p) fail("empty field");
    }
    return count;
}

typedef struct { unsigned bad, good; int isolated; } Channel;
static double median(double a, double b, double c) { if (a > b) { double t = a; a = b; b = t; } if (b > c) { double t = b; b = c; c = t; } return a > b ? a : b; }
static void vote(Channel *channels, const double *values, const double *ages, double tolerance, double *estimate, int *valid) {
    double center = median(values[0], values[1], values[2]);
    for (size_t i = 0; i < 3; i++) {
        int bad = ages[i] > 2 || fabs(values[i] - center) > tolerance;
        if (bad) { channels[i].good = 0; if (channels[i].bad < 3) channels[i].bad++; }
        else { channels[i].bad = 0; if (channels[i].good < 5) channels[i].good++; }
        if (channels[i].bad == 3) channels[i].isolated = 1;
        if (channels[i].good == 5) channels[i].isolated = 0;
    }
    double sum = 0; unsigned usable = 0;
    for (size_t i = 0; i < 3; i++) if (!channels[i].isolated && ages[i] <= 2 && fabs(values[i] - center) <= tolerance) { sum += values[i]; usable++; }
    *valid = usable >= 2; *estimate = usable ? sum / usable : 0;
}
int main(int argc, char **argv) {
    if (argc > 1 && !(argc == 2 && !strcmp(argv[1], "--self-test"))) fail("unsupported command arguments");
    if (argc == 2 && !strcmp(argv[1], "--self-test")) {
        Channel c[3] = {0}; double v[] = {10, 10.1, 50}, age[] = {0, 0, 0}, out; int valid;
        for (int i = 0; i < 3; i++) vote(c, v, age, 0.5, &out, &valid);
        assert(valid && c[2].isolated && fabs(out - 10.05) < 1e-9);
        v[2] = 10; for (int i = 0; i < 5; i++) vote(c, v, age, 0.5, &out, &valid); assert(!c[2].isolated);
        age[0] = age[1] = 3; vote(c, v, age, 0.5, &out, &valid); assert(!valid); puts("isolation, recovery and quorum tests passed"); return 0;
    }
    Channel channels[3] = {0}; char line[512]; double last = -1; size_t rows = 0;
    while (fgets(line, sizeof line, stdin)) {
        if (strlen(line) == sizeof line - 1 && line[sizeof line - 2] != '\n') fail("input record too large");
        char *f[8]; if (fields(line, f, 8) != 8) fail("expected eight columns");
        double t = number(f[0]), v[3], ages[3], tolerance = number(f[7]);
        if (t <= last || tolerance <= 0) fail("invalid time or tolerance");
        last = t;
        for (size_t i = 0; i < 3; i++) { v[i] = number(f[1 + i]); ages[i] = number(f[4 + i]); if (ages[i] < 0) fail("negative age"); }
        double out; int valid; vote(channels, v, ages, tolerance, &out, &valid);
        printf("time=%.3f valid=%d estimate=%.4f isolated=%d%d%d\n", t, valid, out, channels[0].isolated, channels[1].isolated, channels[2].isolated); rows++;
    }
    if (!rows || ferror(stdin)) fail("missing input");
    return 0;
}
