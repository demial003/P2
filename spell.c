#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <regex.h>

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

typedef struct
{
    char **arr;
    int size;
} dictionary;

void listfiles(char *dirname);
char *readFiles(char *filename, dictionary d);
dictionary makeDictionary(char *filename);
static int compareWords(const void * word1, const void * word2);
void parseWord(dictionary dict, char* word);

int main(int argc, char **argv)
{
    char *suff = ".txt";
    struct stat info;
    dictionary d;

    if(argc < 2){
        printf("Please use proper arguments\n");
        exit(EXIT_FAILURE);
    }

    if(argc == 2){
        d = makeDictionary(argv[1]);
        qsort(d.arr, d.size, sizeof(char*), compareWords);
    }
    else if (strcmp(argv[1], "-s") == 0)
    {
        puts("suffix tag");
        suff = argv[2];
        d = makeDictionary(argv[3]);
        qsort(d.arr, d.size, sizeof(char*), compareWords);
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
    else
    {
        for (int i = 2; i < argc; i++)
        {
            d = makeDictionary(argv[1]);
            qsort(d.arr, d.size, sizeof(char*), compareWords);
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

    readFiles("test.txt", d);

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

char *readFiles(char *filename, dictionary d)
{

    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        puts("ERORR");
        exit(1);
    }
    char buf[BUFSIZE];
    char *word = NULL;
    buf[BUFSIZE] = ' ';
    int bytes;
    int line = 1;
    int col = 1;
    int wordlen = 0;

    while ((bytes = read(fd, buf, BUFSIZE)) > 0)
    {
        int segstart = 0;
        int pos;
        for (pos = 0; pos < bytes; pos++)
        {
            if (isspace(buf[pos]) || buf[pos] == '\n')
            {
                int seglen = pos - segstart;
                word = realloc(word, wordlen + seglen + 1);
                memcpy(word, buf + segstart, seglen);
                printf("Reading> %s  line: %d  col: %d\n", word, line, col);
                // parseWord(d, word);
                ++col;
                wordlen = 0;
                word = NULL;
                segstart = pos + 1;
            }
            if (buf[pos] == '\n')
            {
                ++line;
                col = 0;
            }
        }
        if (segstart < pos)
        {
            int seglen = pos - segstart;
            word = realloc(word, wordlen + seglen + 1);
            memcpy(word + wordlen, buf + segstart, seglen);
            wordlen = wordlen + seglen;
        }
    }
}

dictionary makeDictionary(char *filename)
{
    int dictSize = 100 * sizeof(char *);
    char **dict = malloc(dictSize);
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        puts("ERORR");
        exit(1);
    }
    char buf[BUFSIZE];
    char *word = NULL;
    buf[BUFSIZE] = ' ';
    int idx = 0;
    int bytes;
    int line = 0;
    int col = 0;
    int wordlen = 0;

    while ((bytes = read(fd, buf, BUFSIZE)) > 0)
    {
        int segstart = 0;
        int pos;
        for (pos = 0; pos < bytes; pos++)
        {
            if (idx * sizeof(char *) >= dictSize - 8)
            {
                dictSize *= 2;
                dict = realloc(dict, dictSize);
            }
            if (buf[pos] == '\n' || isspace(buf[pos]))
            {
                int seglen = pos - segstart;
                word = realloc(word, wordlen + seglen + 1);
                memcpy(word + wordlen, buf + segstart, seglen);
                dict[idx] = word;
                ++idx;
                ++col;
                wordlen = 0;
                word = NULL;
                segstart = pos + 1;
            }
        }

        if (segstart < pos)
        {
            int seglen = pos - segstart;
            word = realloc(word, wordlen + seglen + 1);
            memcpy(word + wordlen, buf + segstart, seglen);
            wordlen = wordlen + seglen;
        }
    }

    dictionary d;
    d.arr = dict;
    d.size = idx;
    return d;
}


static int compareWords(const void * word1, const void * word2){
    return strcmp(*(char**)(word1), *(char**)word2);
}

void parseWord(dictionary dict, char* word){    
    int value;
    int len = strlen(word);
    char * s;
    for(s = word; *s; ++s){
        if(isalpha(*s)){
            break;
        }
        else {
            return;
        }
    }

    for(int i = len - 1; i >= 0; i--){
        if(isalnum(word[i])){
            word[i + 1] = '\0';
            break;
        }
        else{

        }
    }

    char* ref = word;
    char** res = (char**) bsearch(&ref, dict.arr, dict.size, sizeof(char *), compareWords);


    if(res){
        puts("match");
    }
    
}


/*
TODO:
Capitalization
Erros
Testing
*/
