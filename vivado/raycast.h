#ifndef SRC_RAYCAST_H_
#define SRC_RAYCAST_H_
#include "xil_types.h"

typedef struct {
    float x;
    float y;
    float z;
} vec_t;

typedef struct {
    vec_t origin;
    vec_t direction;
} ray_t;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} col_t;

typedef struct {
    vec_t center;
    float radius;
    col_t col;
} sphere_t;

#endif /* SRC_RAYCAST_H_ */
