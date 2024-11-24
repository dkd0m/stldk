#ifndef DKD0M_STLDK_H
#define DKD0M_STLDK_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define STL_HEADER_SIZE 80
#define STL_UINT32_SIZE 4
#define STL_FLOAT_SIZE 4
#define STL_UINT16_SIZE 2
#define STL_TOP_SIZE STL_HEADER_SIZE + STL_UINT32_SIZE
#define STL_FACE_SIZE STL_FLOAT_SIZE * 3 * 4 + STL_UINT16_SIZE

typedef struct {
    float normal[3];
    float v1[3];
    float v2[3];
    float v3[3];
    uint16_t attribute;
} StlFace;

typedef struct {
    char header[STL_HEADER_SIZE];
    uint32_t n_faces;
    StlFace faces[];
} StlMesh;


StlMesh *stldk_alloc(uint32_t n_faces);
StlMesh *stldk_copy(const StlMesh *source);
size_t stldk_size(uint32_t model_n_faces);
StlMesh *stldk_read_binary(const char *path);
FILE *stldk_write_binary(StlMesh *model, const char *path, bool overwrite);


#endif // DKD0M_STLDK_H
