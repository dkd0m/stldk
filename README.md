# stldk - Simple C library to read and write binary .STL files 
Including a simple tool 'stl_btoa' to convert binary STL files into ascii

### How to build

``` bash
> git clone https://github.com/dkd0m/stldk
> cd stldk
> make
```
## How to use

Copy the stldk.h header and the libstldk.a (or just use the stldk.o) into your corresponding project's folder and build with it.

### Example build

``` bash
> gcc -Wall -Wextra -std=c11 -pedantic test_program.c -L. -lstldk -o test_program.out 
```
### Example usage - test_program.c
``` c
...
...
...
#include "stldk.h"

int main(void){
    StlMesh *model = stldk_read_binary("/home/your_user/yourfile.stl");
    StlMesh *model_copy = stldk_copy(model);

    for(uint32_t i = 0; i < model_copy->n_faces; i++){
        StlFace *face = &model_copy->faces[i];
        /*
        face->normal
        face->v1
        face->v2
        face->v3
        face->attribute
        */
    }

    bool overwriteFile = true;
    FILE *new_file = stldk_write_binary(model_copy, "/home/your_user/yourfile_transformed.stl", overwriteFile);

    free(model);
    free(model_copy);
    fclose(new_file);

    return EXIT_SUCCESS;
}
```

