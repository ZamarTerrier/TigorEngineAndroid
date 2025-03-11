//
// Created by prg6 on 11.03.2025.
//

#include "e_direct.h"

#include "e_memory.h"

char* DirectGetCurrectFilePath(){

    char *currPath = (char *)AllocateMemory(256, sizeof(char));

#ifndef __ANDROID__
    _getcwd(currPath, 256);
#endif

    return currPath;
}

char *DirectGetFileExt(const char *name_file){
    int len = strlen(name_file);
    int i = 0;
    for(i=0;i < len; i++)
        if(name_file[i] == '.')
            break;

    int diff = len - i;

    char *ext = (char *)AllocateMemory(diff + 1, sizeof(char));

    memcpy(ext, name_file + i, diff);
    ext[diff] = '\0';

    return ext;
}

char *DirectGetFileWithoutExt(const char *name_file){
    int len = strlen(name_file);
    int i = 0;
    for(i=0;i < len; i++)
        if(name_file[i] == '.')
            break;

    char *ext = (char *)AllocateMemory(i + 1, sizeof(char));

    memcpy(ext, name_file, i - 1);
    ext[i] = '\0';

    return ext;
}

char *DirectGetFileName(const char *name_file){
    int len = strlen(name_file);
    int i = 0;
    for(i = len - 1;i > 0; i--)
        if(name_file[i] == '\\' || name_file[i] == '/')
            break;

    int diff = len - i;

    char *name = (char *)AllocateMemory(diff, sizeof(char));

    memcpy(name, name_file + i + 1, diff - 1);
    name[diff - 1] = '\0';

    return name;
}

int DirectIsFileExist(const char *path){
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    } else {
#ifndef NDEBUG
        printf("Error! File does not exist : %s\n", path);
#endif
        return false;
    }
}
