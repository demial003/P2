#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

typedef struct
{
    char **arr;
    int size;
} dictionary;

void listfiles(char *dirname, char *suff, dictionary d);
char *readFiles(char *filename, dictionary d);
dictionary makeDictionary(char *filename);
static int compareWords(const void *word1, const void *word2);
int parseWord(dictionary dict, char *word);
static int checkWords(const void *word1, const void *word2);

static int success = 0;

int main(int argc, char **argv)
{
    char *suff = ".txt";
    struct stat info;
    dictionary d;

    if (argc < 2)
    {
        printf("Please use proper arguments\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
    {
        d = makeDictionary(argv[1]);
        qsort(d.arr, d.size, sizeof(char *), compareWords);
        readFiles("", d);
    }
    else if (strcmp(argv[1], "-s") == 0)
    {
        suff = argv[2];
        d = makeDictionary(argv[3]);
        qsort(d.arr, d.size, sizeof(char *), compareWords);
        for (int i = 4; i < argc; i++)
        {
            stat(argv[i], &info);
            if (S_ISREG(info.st_mode))
            {
                readFiles(argv[i], d);
            }
            else if (S_ISDIR(info.st_mode))
            {
                listfiles(argv[i], suff, d);
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
            qsort(d.arr, d.size, sizeof(char *), compareWords);
            stat(argv[i], &info);
            if (S_ISREG(info.st_mode))
            {
                readFiles(argv[i], d);
            }
            else if (S_ISDIR(info.st_mode))
            {
                listfiles(argv[i], suff, d);
            }
            else
            {
                printf("%s: Invalid argument\n", argv[i]);
            }
        }
    }

    for(int i = 0; i < d.size; i++){
        free(d.arr[i]);
    }
    free(d.arr);

    if (success == 1)
    {
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void listfiles(char *dirname, char *suff, dictionary d)
{
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
            int len = strlen(de->d_name);
            char *s = malloc(8);
            memcpy(s, de->d_name + len - 4, 4);

            char path[100] = {0};
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, de->d_name);
            if (strcmp(s, suff) == 0)
            {
                readFiles(path, d);
                free(s);
            }
            listfiles(path, suff, d);
        }
        de = readdir(dir);
    }
    closedir(dir);
}

char *readFiles(char *filename, dictionary d)
{
    int fd;
    if (filename == "")
    {
        fd = STDIN_FILENO;
    }
    else
    {
        fd = open(filename, O_RDONLY);
    }

    if (fd < 0)
    {
        puts("ERORR");
        exit(1);
    }
    char buf[BUFSIZE];
    char *word = NULL;
    // buf[BUFSIZE] = ' ';
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
                memcpy(word + wordlen, buf + segstart, seglen);
                word[wordlen + seglen] = '\0';
                if (parseWord(d, word) == 1)
                {
                    printf("%s:%d:%d %s\n", filename, line, col, word);
                    success = 1;
                }
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
            word[wordlen + seglen] = '\0';
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
    buf[BUFSIZE - 1] = ' ';
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
                word[wordlen + seglen] = '\0';
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
            word[wordlen + seglen] = '\0';
            wordlen = wordlen + seglen;
        }
    }
    dictionary d;
    d.arr = dict;
    d.size = idx;
    return d;
}

static int compareWords(const void *word1, const void *word2)
{
    return strcasecmp(*(char**)(word1), *(char**)(word2));
}

static int checkWords(const void *word1, const void *word2)
{
    char *ref1 = *(char **)(word1);
    char *ref2 = *(char **)(word2);

    if (islower(ref2[0]) || (isupper(ref2[0]) && isupper(ref1[0])))
    {
        if(strcmp(ref1, "and") == 0){
        }
        return strcasecmp(ref1, ref2);      
    }
    else if(strcasecmp(ref1,ref2) == 0){
        return strcmp(ref1, ref2);
    }
    else{
        return strcasecmp(ref1, ref2);
    }
    
}

int parseWord(dictionary dict, char *word)
{
    int value;
    int len = strlen(word);
    char *s;
    char *ref = malloc(len + 1);
    strncpy(ref, word, len);

    for (int i = 0; i < len; i++)
    {
        if (ref[i] != '(' && ref[i] != '{' && ref[i] != '[' && ref[i] != '\"' && ref[i] != '\'' && isalnum(ref[i]))
        {
            ref = memmove(ref, ref + i, len - i + 1);
            break;
        }
    }
    int flag = 0;
    for (s = ref; *s; ++s)
    {
        if(isdigit(*s)){
            flag = 2;
        }
        else if(isalpha(*s)){
            flag = 0;
            break;
        }
        else{
            flag = 1;
        }
    }

    if(flag != 0) return 2;

 
    int offset = 0;
    for (int i = len - 1; i >= 0; i--)
    {
        if (isalnum(ref[i]))
        {
            ref[i + 1] = '\0';
            offset = i;
            break;
        }
        else
        {
        }
    }

    char* key = ref;

    char **res = (char **)bsearch(&key, dict.arr, dict.size, sizeof(char *), checkWords);

    // free(ref);
    int outp = 1;
    if (res)
    {
        outp =  0;
    }
    return outp;
    
}

