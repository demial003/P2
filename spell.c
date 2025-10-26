#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

void listfiles(char *dirname);

int main(int argc, char **argv)
{
    char* suff = ".txt";
    char *dict;
    struct stat info;

    if(strcmp(argv[1], "-s") == 0){
        puts("suffix tag");
        suff = argv[2];
        dict = argv[3];
        for (int i = 4; i < argc; i++)
    {
        stat(argv[i], &info);
  if (S_ISREG(info.st_mode))
        {
            printf("%s is a regular file.\n", argv[i]);
        }
        else if (S_ISDIR(info.st_mode))
        {
            listfiles(argv[i]);
        }
        else
        {
            printf("%s is neither a regular file nor a directory (e.g., a symbolic link, device file, etc.).\n", argv[i]);
        }
    }
    }
    else{
        for (int i = 2; i < argc; i++)
    {
        dict = argv[1];
        stat(argv[i], &info);
        if (S_ISREG(info.st_mode))
        {
            printf("%s is a regular file.\n", argv[i]);
        }
        else if (S_ISDIR(info.st_mode))
        {
            listfiles(argv[4]);
        }
        else
        {
            printf("%s is neither a regular file nor a directory (e.g., a symbolic link, device file, etc.).\n", argv[i]);
        }
    }
    }

    printf("\nsuffix: %s\n dict:  %s\n", suff, dict);



    


}

void listfiles(char *dirname)
{
    char *suff = ".txt";

    DIR *dir = opendir(dirname);
    if (dir == NULL)
        return;
    struct dirent *de;
    de = readdir(dir);
    while (de != NULL)
    {
        char *compare;
        compare = dirname + 2;
        
        struct stat info;
        stat(de->d_name, &info);
        if (S_ISDIR(info.st_mode) && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
        {
            printf("dir: %s\n", dirname);
            printf("entry: %s\n", de->d_name);
            char path[100] = {0};
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, de->d_name);
            listfiles(path);
        }
        de = readdir(dir);
    }
    closedir(dir);
}