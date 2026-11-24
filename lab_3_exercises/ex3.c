#include <stdio.h>
#include <string.h>

#define MAX_NAME  64
#define MAX_PATH  2048
#define MAX_DATA  1024
#define MAX_FILES 255
#define MAX_DIRS  255

typedef struct Directory Directory;

typedef struct File {
    unsigned int id;
    char name[MAX_NAME];
    size_t size;
    char data[MAX_DATA];
    Directory *directory;
} File;

struct Directory {
    char name[MAX_NAME];
    File files[MAX_FILES];
    Directory *sub_dirs[MAX_DIRS];
    unsigned char nf;
    unsigned char nd;
    char path[MAX_PATH];
};


void overwrite_to_file(File *file, const char *str)
{
    strncpy(file->data, str, MAX_DATA - 1);
    file->data[MAX_DATA - 1] = '\0';
    file->size = strlen(file->data) + 1;
}

void append_to_file(File *file, const char *str)
{
    size_t room = MAX_DATA - strlen(file->data) - 1;
    strncat(file->data, str, room);
    file->size = strlen(file->data) + 1;
}

void printp_file(File *file)
{
    Directory *d = file->directory;
    if (strcmp(d->path, "/") == 0)
        printf("/%s\n", file->name);
    else
        printf("%s/%s\n", d->path, file->name);
}


void add_file(File *file, Directory *dir)
{
    if (dir->nf >= MAX_FILES) {
        printf("Directory %s is full\n", dir->path);
        return; }
    file->directory = dir;
    dir->files[dir->nf] = *file;
    dir->nf++;
}


void add_dir(Directory *dir1, Directory *dir2)
{
    if (dir1 && dir2) {
        dir2->sub_dirs[dir2->nd] = dir1;
        dir2->nd++;
        char temp_path[MAX_PATH];
        if (strcmp(dir2->path, "/")) {
            strcpy(temp_path, dir2->path);
            strcat(temp_path, "/");
            strcat(temp_path, dir1->name);
        } else {
            strcpy(temp_path, "/");
            strcat(temp_path, dir1->name);
        }
        strcpy(dir1->path, temp_path);
    }
}


void show_file(File *file) {
    printf("%s ", file->name);
}

void show_dir(Directory *dir)
{
    printf("\nDIRECTORY\n");
    printf(" path: %s\n", dir->path);
    printf(" files:\n");
    printf(" [ ");
    for (int i = 0; i < dir->nf; i++) {
        show_file(&(dir->files[i]));
    }
    printf("]\n");
    printf(" directories:\n");
    printf(" { ");
    for (int i = 0; i < dir->nd; i++) {
        show_dir(dir->sub_dirs[i]);
    }
    printf("}\n");
}


static Directory root, home, bin;

int main(void)
{
    unsigned int next_id = 1;

    strcpy(root.name, "/");
    strcpy(root.path, "/");
    strcpy(home.name, "home");
    strcpy(bin.name, "bin");
    add_dir(&home, &root);
    add_dir(&bin,  &root);

    File f = {0};

    f.id = next_id++; strcpy(f.name, "bash");
    add_file(&f, &bin);
    File *bash = &bin.files[bin.nf - 1];

    memset(&f, 0, sizeof f);
    f.id = next_id++; strcpy(f.name, "ex3_1.c");
    add_file(&f, &home);
    File *ex3_1 = &home.files[home.nf - 1];

    memset(&f, 0, sizeof f);
    f.id = next_id++; strcpy(f.name, "ex3_2.c");
    add_file(&f, &home);
    File *ex3_2 = &home.files[home.nf - 1];

    overwrite_to_file(ex3_1, "int printf(const char * format, ...);");
    overwrite_to_file(ex3_2, "//This is a comment in C language");
    overwrite_to_file(bash, "Bourne Again Shell!!");
    append_to_file(ex3_1, "int main(){printf(\"Hello World!\")}");

    printf("Paths of all files:\n");
    printp_file(bash);
    printp_file(ex3_1);
    printp_file(ex3_2);

    printf("\nContent of %s (%zu bytes): %s\n", ex3_1->name, ex3_1->size, ex3_1->data);
    show_dir(&root);
    return 0;
}
