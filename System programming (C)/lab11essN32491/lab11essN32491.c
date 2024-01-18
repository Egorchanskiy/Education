#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <ftw.h>    //Хед для ftw обхода
#include <sys/param.h>
#include <malloc.h>

#define _GNU_SOURCE //Макрос для поддержки ф-ции getline

//гп для хранения директории поиска и строки соответственно
char* dir_search;
char* targ_search;

//Флаг отладки
int Debug_FLAG = 0;

//Поиск целевой строки
void CheckFileLine(const char* fpath)
{
    FILE* fp = fopen(fpath, "r");
    if (!fp && Debug_FLAG) {
        fprintf(stderr, "Unable to open! %s\n", fpath);
    }

    char* buffer = NULL;
    size_t buffer_length = 0;
    ssize_t nread;
    size_t Nline = 0;
    while ((nread = getline(&buffer, &buffer_length, fp)) != -1) {
        if (strstr(buffer, targ_search) != NULL) {
            if (Debug_FLAG)
                fprintf(stdout, "Line = %ld, file = %s, string = %s", Nline, fpath, buffer);
            else
                fprintf(stdout, "File: %s\n", fpath);
        }
        Nline++;
    }
    free(buffer);
    fclose(fp);
}

/*Принимает инфу о текущем пути файла fpath, структуру stat и флаг typeflag, указывающий тип файла. Если файл является обычным файлом FTW_F, то функция вызывает CheckFileLine() для анализа содержимого файла */
int CheckFile(const char* fpath, const struct stat* sb, int typeflag)
{
    (void)sb;
    if (typeflag == FTW_F)
        CheckFileLine(fpath);
    return 0;
}

/*Начинает обход ФС, начиная с данной директории dir_search. Используется ftw() для рекурсивного обхода дерева каталогов и вызывает CheckFile() для каждого файла*/
void WalkDirectory()
{
    int result = ftw(dir_search, CheckFile, FTW_NS);
    if (result < 0 && Debug_FLAG) {
        fprintf(stderr, "ftw() failed: %s\n", strerror(errno));
    }
}

/*Функция для разбора аргументов ком строки, getopt нужен для обработки опций*/
void OptionDo(int argc, char* const* argv)
{
    int value = 0;
    opterr = 0;
    static struct option long_options[] = {
        { "help", no_argument, 0, 'h' },
        { "version", no_argument, 0, 'v' }
    };

    while (1) {
        int optindex = 0;
        value = getopt_long(argc, argv, "hv", long_options, &optindex);
        if (value == -1)
            break;
        //Выводимая инфа
        switch (value) {
        case 'h':
            fprintf(stdout, "-h, --help: Display information about options\
                \n-v, --version: Display information about programms\n");
            exit(EXIT_SUCCESS);

        case 'v':
            fprintf(stdout, "Version 1.1\
                \nSamaykin Egor Sergeevich\
                \nN32491\
                \n3\n");
            exit(EXIT_SUCCESS);

        case '?':
            if (Debug_FLAG)
                fprintf(stderr, "Inputed unknown option: %c\
                    \nUsage: lab11essN32491 [options] directory target\n",
                    optopt);
            exit(EXIT_FAILURE);

        default:
            break;
        }
    }

    if (argc - optind <= 1) {
        if (Debug_FLAG)
            fprintf(stderr, "Usage: lab11essN32491 [options] directory target\n");
        exit(EXIT_FAILURE);
    }
    dir_search = argv[optind];
    targ_search = argv[optind + 1];
}

int main(int argc, char* const* argv)
{ //(int argc, char **argv)
    //Проверка флага переменной окружения
    if (getenv("LAB11DEBUG")) {
        Debug_FLAG = 1;
    }

    OptionDo(argc, argv);
    WalkDirectory();

    return 0;
}
