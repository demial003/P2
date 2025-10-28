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


typedef struct{
    char** arr;
    int size;
} dictionary;

void listfiles(char *dirname);
char *readFiles(char *filename);
dictionary makeDictionary(char *filename);
void use_word(char *word);

int main(int argc, char **argv)
{

dictionary dict = makeDictionary("dict.txt");
for(int i = 0; i < dict.size; i++){

        printf("%s\n", dict.arr[i]);
}
// free(dict.arr);

    

    // readFiles("dict.txt");

    // makeDictionary("dict.txt", puts);

    // char* suff = ".txt";
    // char *dict;
    // struct stat info;

    // if(strcmp(argv[1], "-s") == 0){
    //     puts("suffix tag");
    //     suff = argv[2];
    //     dict = argv[3];
    //     for (int i = 4; i < argc; i++){
    //     stat(argv[i], &info);
    //     if (S_ISREG(info.st_mode))
    //     {
    //         printf("%s is a regular file.\n", argv[i]);
    //     }
    //     else if (S_ISDIR(info.st_mode))
    //     {
    //         listfiles(argv[i]);
    //     }
    //     else
    //     {
    //         printf("%s is neither a regular file nor a directory (e.g., a symbolic link, device file, etc.).\n", argv[i]);
    //     }
    //     }
    // }
    // else{
    //     for (int i = 2; i < argc; i++)
    // {
    //     dict = argv[1];
    //     stat(argv[i], &info);
    //     if (S_ISREG(info.st_mode))
    //     {
    //         printf("%s is a regular file.\n", argv[i]);
    //     }
    //     else if (S_ISDIR(info.st_mode))
    //     {
    //         listfiles(argv[4]);
    //     }
    //     else
    //     {
    //         printf("%s is neither a regular file nor a directory (e.g., a symbolic link, device file, etc.).\n", argv[i]);
    //     }
    // }
    // }

    // printf("\nsuffix: %s\n dict:  %s\n", suff, dict);
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

char *readFiles(char *filename)
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
    int line = 0;
    int col = 0;
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
                printf("%s\n", word);
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
            memcpy(word, buf + segstart, wordlen);
            wordlen = wordlen + seglen;
        }
    }
    if (word)
    {
        // use_word(word);
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

     while ((bytes = read(fd, buf, BUFSIZE)) > 0) {
	// if (DEBUG) printf("[got %d bytes]\n", bytes);
	int segstart = 0;
	int pos;
	for (pos = 0; pos < bytes; pos++) {
		            if (idx * sizeof(char *) >= dictSize - 8)
            {
                dictSize *= 2;
                dict = realloc(dict, dictSize);
            }
	    if (isspace(buf[pos]) || buf[pos] == '\n') {
		int seglen = pos - segstart;
		// if (DEBUG) printf("[%d/%d/%d found newword; %d+%d]\n", segstart, pos, bytes, wordlen, seglen);
		word = realloc(word, wordlen + seglen + 1);
		memcpy(word + wordlen, buf + segstart, seglen);
		word[wordlen + seglen] = '\0';
		// use_word(word, arg);
		dict[idx] = word;
		++idx;
		word = NULL;
		wordlen = 0;
		segstart = pos + 1;
	    }
	}

	if (segstart < pos) {
	    int seglen = pos - segstart;
	    word = realloc(word, wordlen + seglen + 1);
	    memcpy(word + wordlen, buf + segstart, seglen);
	    wordlen = wordlen + seglen;
	    word[wordlen] = '\0';
	}
    }


    dictionary d;
    d.arr = dict;
    d.size = idx;
    return d;
}