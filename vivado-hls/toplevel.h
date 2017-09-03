#ifndef __TOPLEVEL_H_
#define __TOPLEVEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;

typedef struct {
    float x;
    float y;
    float z;
} vec_t;

typedef struct {
    short int x;
    short int y;
    short int z;
} vec32_t;

typedef struct {
    vec32_t center;
    u32 col;
    int C;
} sphere_t;

typedef int int32;

u32 toplevel(u32 *ram);

#endif /* SRC_RAYCAST_H_ */
