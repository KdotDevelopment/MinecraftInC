#include <world/terrain/noise/noise_perlin.h>

#include <world/terrain/noise/noise.h>

#include <math.h>

noise_t noise_perlin_create(random_t *random) {
    noise_t noise = noise_create();

    noise.noise_type = NOISE_PERLIN;

    noise.x_coord = random_next_uniform(random) * 256.0;
    noise.y_coord = random_next_uniform(random) * 256.0;
    noise.z_coord = random_next_uniform(random) * 256.0;

    for(int i = 0; i < 256; i++) noise.hash[i] = i;

    for(int i = 0; i < 256; i++) {
        int r1 = random_next_int_range(random, 0, 256 - i - 1) + i;
        int r2 = noise.hash[i];
        noise.hash[i] = noise.hash[r1];
        noise.hash[r1] = r2;
        noise.hash[i + 256] = noise.hash[i];
    }

    noise.get = noise_perlin_get;

    return noise;
}

static double F(double x) {
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

static double lerp(double t, double a, double b) {
    return a + t * (b - a);
}

static double grad(int i, double x, double y, double z) {
    i &= 15;
    double a = i < 8 ? x : y;
    double b = i < 4 ? y : (i != 12 && i != 14 ? z : x);
    return ((i & 1) == 0 ? a : -a) + ((i & 2) == 0 ? b : -b);
}

double noise_perlin_get(noise_t *noise, double x, double y, double z) {
    double vx = x + noise->x_coord;
    double vy = y + noise->y_coord;
    double vz = z + noise->z_coord;

    int X = (int)vx;
    if(vx < (double)X) X--;
    int Y = (int)vy;
    if(vy < (double)Y) Y--;
    int Z = (int)vz;
    if(vz < (double)Z) Z--;

    vx -= (double)X;
    vy -= (double)Y;
    vz -= (double)Z;

    int ix = X & 255;
    int iy = Y & 255;
    int iz = Z & 255;

    double xd = F(vx);
    double yd = F(vy);
    double zd = F(vz);

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = noise->hash[noise->hash[noise->hash[ix] + iy] + iz];
    aba = noise->hash[noise->hash[noise->hash[ix] + iy + 1] + iz];
    aab = noise->hash[noise->hash[noise->hash[ix] + iy] + iz + 1];
    abb = noise->hash[noise->hash[noise->hash[ix] + iy + 1] + iz + 1];
    baa = noise->hash[noise->hash[noise->hash[ix + 1] + iy] + iz];
    bba = noise->hash[noise->hash[noise->hash[ix + 1] + iy + 1] + iz];
    bab = noise->hash[noise->hash[noise->hash[ix + 1] + iy] + iz + 1];
    bbb = noise->hash[noise->hash[noise->hash[ix + 1] + iy + 1] + iz + 1];

    double l1 = lerp(xd, grad(aaa, vx, vy, vz), grad(baa, vx - 1.0, vy, vz));
    double l2 = lerp(xd, grad(aba, vx, vy - 1.0, vz), grad(bba, vx - 1.0, vy - 1.0, vz));
    double l3 = lerp(xd, grad(aab, vx, vy, vz - 1.0), grad(bab, vx - 1.0, vy, vz - 1.0));
    double l4 = lerp(xd, grad(abb, vx, vy - 1.0, vz - 1.0), grad(bbb, vx - 1.0, vy - 1.0, vz - 1.0));
    double l = lerp(zd, lerp(yd, l1, l2), lerp(yd, l3, l4));

    return l;
}

void noise_perlin_populate_array(noise_t *noise, double *output, int start_x, int start_y, int start_z, int size_x, int size_y, int size_z, double scale_x, double scale_y, double scale_z, double amplitude) {
    int index = 0;
    double amplitude_inverse = 1.0 / amplitude;
    int last_hash_y = -1;
    double lerp_x1 = 0;
    double lerp_x2 = 0;
    double lerp_x3 = 0;
    double lerp_x4 = 0;

    for(int x = 0; x < size_x; ++x) {
        double noise_x = (start_x + x) * scale_x + noise->x_coord;
        int floor_x = (int)floor(noise_x);
        if(noise_x < floor_x) floor_x--;

        int hash_x = floor_x & 255;
        noise_x -= floor_x;
        double fade_x = noise_x * noise_x * noise_x * (noise_x * (noise_x * 6.0 - 15.0) + 10.0);

        for(int z = 0; z < size_z; ++z) {
            double noise_z = (start_z + z) * scale_z + noise->z_coord;
            int floor_z = (int)floor(noise_z);
            if(noise_z < floor_z) floor_z--;

            int hash_z = floor_z & 255;
            noise_z -= floor_z;
            double fade_z = noise_z * noise_z * noise_z * (noise_z * (noise_z * 6.0 - 15.0) + 10.0);

            for(int y = 0; y < size_y; ++y) {
                double noise_y = (start_y + y) * scale_y + noise->y_coord;
                int floor_y = (int)floor(noise_y);
                if(noise_y < floor_y) floor_y--;

                int hash_y = floor_y & 255;
                noise_y -= floor_y;
                double fade_y = noise_y * noise_y * noise_y * (noise_y * (noise_y * 6.0 - 15.0) + 10.0);

                if(y == 0 || hash_y != last_hash_y) {
                    last_hash_y = hash_y;

                    int hash_xy = (noise->hash[hash_x] + hash_y) & 255;
                    int hash_xy1 = (noise->hash[hash_xy] + hash_z) & 255;
                    int hash_xy2 = (noise->hash[hash_xy + 1] + hash_z) & 255;

                    int hash_x1y = (noise->hash[hash_x + 1] + hash_y) & 255;
                    int hash_x1y1 = (noise->hash[hash_x1y] + hash_z) & 255;
                    int hash_x1y2 = (noise->hash[hash_x1y + 1] + hash_z) & 255;

                    lerp_x1 = lerp(fade_x, grad(noise->hash[hash_xy1], noise_x, noise_y, noise_z), grad(noise->hash[hash_x1y1], noise_x - 1.0, noise_y, noise_z));
                    lerp_x2 = lerp(fade_x, grad(noise->hash[hash_xy2], noise_x, noise_y - 1.0, noise_z), grad(noise->hash[hash_x1y2], noise_x - 1.0, noise_y - 1.0, noise_z));
                    lerp_x3 = lerp(fade_x, grad(noise->hash[hash_xy1 + 1], noise_x, noise_y, noise_z - 1.0), grad(noise->hash[hash_x1y1 + 1], noise_x - 1.0, noise_y, noise_z - 1.0));
                    lerp_x4 = lerp(fade_x, grad(noise->hash[hash_xy2 + 1], noise_x, noise_y - 1.0, noise_z - 1.0), grad(noise->hash[hash_x1y2 + 1], noise_x - 1.0, noise_y - 1.0, noise_z - 1.0));
                }

                double lerp_y1 = lerp(fade_y, lerp_x1, lerp_x2);
                double lerp_y2 = lerp(fade_y, lerp_x3, lerp_x4);
                double lerp_z = lerp(fade_z, lerp_y1, lerp_y2);

                output[index++] += lerp_z * amplitude_inverse;
            }
        }
    }
}