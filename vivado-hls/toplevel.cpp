#include "toplevel.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

////Input data storage
#define VREZ 600
#define HREZ 800
#define MAXNUMSPHERE 20
//Prototypes
int intersect_sphere(vec_t ray, sphere_t sphere);
u32 toplevel(u32 *ram) {
    #pragma HLS INTERFACE m_axi port=ram offset=slave bundle=MAXI
    #pragma HLS INTERFACE s_axilite port=return bundle=AXILiteS register

	u32 buffer[HREZ];
	sphere_t spheres[MAXNUMSPHERE];
	readsphere:for (int n = 0; n < MAXNUMSPHERE; n+=1) { //Read spheres out of ram
		short int x = ram[5*n] - 1000;
		short int y = ram[5*n+1] - 1000;
		short int z = ram[5*n+2] - 1000;
		vec32_t centre = {x, y ,z};
		u32 col = (u32) ram[5*n + 3]; // Red , Blue, Green
		int C = ram[5*n+4] - 10000; // pre-calculate C used in intersect
		sphere_t sphere = {centre, col , C};
		spheres[n] = sphere;
	}

	//Shoot a ray for every pixel on the screen
    yloop: for (int y = 0; y < VREZ; y += 1) {
    	int yrez = y*(HREZ);
    	float diry = 0.5 - y / (float) VREZ;
    	float dirysqrplsone = (diry * diry + 1); //precalculate for normalisation y*y + z*z

        xloop:for (int x = 0; x < HREZ; x += 1 ) {
#pragma HLS PIPELINE II=7
            //Create our rays facing into the screen (towards positive Z)
            float dirx = -0.5 + x / (float) HREZ;
            //Then normalise
            float mag = sqrtf((dirx*dirx) + dirysqrplsone);
            vec_t dir;
            dir.x = dirx / mag;
            dir.y = diry / mag;
            dir.z = 1 / mag;
            //Determine the colour of the pixel by checking if the ray intersects with anything
            short int min_distance = 32000;
            short int distance;
            u32 color = 0;
            castloop:for (int sp = 0; sp < MAXNUMSPHERE; sp++) {
        #pragma HLS UNROLL
        		if ((distance = intersect_sphere(dir, spheres[sp]))) {
        			if (distance < min_distance) {
        				color = spheres[sp].col;
        				min_distance = distance;
        			}
        		}
            }

            //Write the pixel colour to the buffer
            buffer[x] = color;
        }

        memcpy(&ram[yrez],buffer, 3200); // copy to shared memory in batches.
    }

    return 0;
}

/*
 * Check if the ray intersects with the given sphere. Returns 1 if it does, and 0 if not.
 * dist is set to the distance at which the intersection takes place.
 * The ray must be a normalised vector (meaning its length must be 1).
 *
 * This code is based on the analytical solution from:
 * https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
 */
int intersect_sphere(vec_t ray, sphere_t sphere) {
    float B;
    B = (ray.x * sphere.center.x) +
        (ray.y * sphere.center.y) +
        (ray.z * sphere.center.z);

    float discriminant = (B*B) - sphere.C;
    if (discriminant < 0 || B < 0) {
        return 0;
    }

    return B - sqrtf(discriminant);
}
