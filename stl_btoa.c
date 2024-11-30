#include <stdlib.h>
#include <stdio.h>
#include "stldk.h"

static void print_help(void){
    puts("Usage:");
    puts("./stl_btoa file_path");
    return;
}

static void print_stl_face(const StlFace *face) {
    printf("  facet normal %.6f %.6f %.6f\n", face->normal[0], face->normal[1], face->normal[2]);
    printf("    outer loop\n");
    printf("      vertex %.6f %.6f %.6f\n", face->v1[0], face->v1[1], face->v1[2]);
    printf("      vertex %.6f %.6f %.6f\n", face->v2[0], face->v2[1], face->v2[2]);
    printf("      vertex %.6f %.6f %.6f\n", face->v3[0], face->v3[1], face->v3[2]);
    printf("    endloop\n");
    printf("  endfacet\n");
}

static void print_stl_file(const StlMesh *stl) {
    const char solid_name[] = "ascii_made_with_stldk";
    printf("solid %s\n", solid_name);

    for (uint32_t i = 0; i < stl->n_faces; i++) {
        print_stl_face(&stl->faces[i]);
    }

    printf("endsolid %s\n", solid_name);
}

int main(int argc, char *argv[]){
    if(argc != 2){
        print_help();
        return EXIT_SUCCESS;
    }
    StlMesh *stl_file = stldk_read_binary(argv[1]);
    if(stl_file == NULL){
        puts("An error occured reading the stl file.");
        return EXIT_FAILURE;
    }
    stldk_calculate_normals(stl_file);
    stldk_flip_normals(stl_file);
    stldk_calculate_normals(stl_file);

    print_stl_file(stl_file);
    free(stl_file);

    return EXIT_SUCCESS;
}
