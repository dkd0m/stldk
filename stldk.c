#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "stldk.h"

#if CHAR_BIT != 8
#error "unsupported char size"
#endif
static_assert(sizeof(float) == STL_FLOAT_SIZE, "unsupported float size");

static long get_file_size(FILE* f){
    fpos_t curr_pos;
    if(fgetpos(f, &curr_pos) != 0){
        perror("get_file_size fgetpos() failed");
        return -1;
    }
    if(fseek(f, 0, SEEK_END) != 0){
        perror("get_file_size fseek() failed");
        return -1;
    }
    long file_size = ftell(f);
    if(file_size == -1L){
        perror("get_file_size ftell() failed");
        return -1;
    }
    if(fsetpos(f, &curr_pos) != 0){
        perror("get_file_size fsetpos() failed");
        return -1;
    }
    return file_size;
}

static int read_stl_vertex(float* v, FILE* f){
    if(fread(v, sizeof(float), 3, f) != 3){
        perror("read_stl_vertex Couldn't read vertex");
        return 1;
    }
    return 0;
}

#define read_vert(ptr,file) if(read_stl_vertex((ptr),(file)) == 1) return 1;
static int read_stl_face(StlFace* face, FILE* f){
    read_vert(face->normal, f);
    read_vert(face->v1, f);
    read_vert(face->v2, f);
    read_vert(face->v3, f);
    if(fread(&(face->attribute), sizeof(face->attribute), 1, f) != 1){
        perror("read_stl_face Couldn't read attribute");
        return 1;
    }
    return 0;
}

extern StlMesh *stldk_read_binary(const char *path){
    FILE *f = fopen(path, "rb");
    if(f == NULL){
        perror("read_stl_face");
        return NULL;
    }

    const long file_size = get_file_size(f);
    if(file_size == -1){
        fputs("Error determining file size\n", stderr);
        return NULL;
    }

    if(file_size < STL_TOP_SIZE + STL_FACE_SIZE){
        fputs("File size is less than possible for the binary stl spec\n", stderr);
        return NULL;
    }

    char header_data[STL_HEADER_SIZE];
    uint32_t n_faces;

    if(fread(header_data, sizeof(header_data), 1, f) != 1){
        perror("Couldn't read header");
        return NULL;
    }
    header_data[STL_HEADER_SIZE - 1] = '\0';

    if(fread(&n_faces, sizeof(n_faces), 1, f) != 1){
        perror("Couldn't read header n_faces");
        return NULL;
    }
    if(n_faces * STL_FACE_SIZE + STL_HEADER_SIZE > file_size){
        fputs("File size does not accomodate for the amount of faces specified in the file\n", stderr);
        return NULL;
    }

    StlMesh* stl_model = stldk_alloc(n_faces);
    memcpy(stl_model->header, header_data, sizeof(header_data));
    stl_model->n_faces = n_faces;

    bool error_occured = false;
    for(uint32_t i = 0; i < stl_model->n_faces; i++){
        if(read_stl_face(&(stl_model->faces[i]), f) == 1){
            break;
        };
    }
    fclose(f);
    if(error_occured){
        free(stl_model);
        return NULL;
    }
    return stl_model;
}

static int stldk_write_top(StlMesh *model, FILE *f){
    size_t n_written = fwrite(model->header, STL_HEADER_SIZE, 1, f);
    if(n_written != 1){
        perror("stldk_write_top fwrite()");
        return 1;
    }
    n_written = fwrite(&model->n_faces, sizeof(model->n_faces), 1, f);
    if(n_written != 1){
        perror("stldk_write_top fwrite()");
        return 1;
    }
    return 0;
}

static int stldk_write_faces(StlFace *verts, uint32_t n_verts, FILE *f){
    for(uint32_t i = 0; i < n_verts; i++){
        StlFace *const curr = &verts[i];
        if(fwrite(curr->normal, sizeof(float), 3, f) != 3){
            return 1;
        }
        if(fwrite(curr->v1, sizeof(float), 3, f) != 3){
            return 1;
        }
        if(fwrite(curr->v2, sizeof(float), 3, f) != 3){
            return 1;
        }
        if(fwrite(curr->v3, sizeof(float), 3, f) != 3){
            return 1;
        }
        if(fwrite(&(curr->attribute), sizeof(curr->attribute), 1, f) != 1){
            return 1;
        }
    }
    return 0;
}


extern FILE *stldk_write_binary(StlMesh *model, const char *path, bool overwrite){
    const char *file_mode;
    if(overwrite){
        file_mode = "w";
    } else {
        file_mode = "wx";
    }
    
    FILE *f = fopen(path, file_mode);
    if(f == NULL){
        perror("stldk_write_binary fopen()");
        return NULL;
    }
    
    if(stldk_write_top(model, f)){
        if(fclose(f)) perror("stldk_write_binary fclose()");
        return NULL;
    };
    if(stldk_write_faces(model->faces, model->n_faces, f)){
        if(fclose(f)) perror("stldk_write_binary fclose()");
        return NULL;
    }
    return f;
}

extern size_t stldk_size(uint32_t model_n_faces){
    return sizeof(StlMesh) + model_n_faces * sizeof(StlFace);
}

extern StlMesh *stldk_alloc(uint32_t n_faces){
    size_t alloc_size = stldk_size(n_faces);
    StlMesh *mesh = malloc(alloc_size);
    if(mesh == NULL){
        perror("stldk_alloc malloc()");
        return NULL;
    }
    memset(mesh, 0, alloc_size);
    mesh->n_faces = n_faces;
    return mesh;
}

extern StlMesh *stldk_copy(const StlMesh *source){
    unsigned long source_size = stldk_size(source->n_faces);
    StlMesh *const stl_copy = stldk_alloc(source->n_faces);
    if(stl_copy == NULL){
        return NULL;
    }
    return memcpy(stl_copy, source, source_size);
}

