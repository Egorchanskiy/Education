#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <dlfcn.h> //Хед для dlopen()
#include <errno.h>
#include <ftw.h> //Хед для ftw обхода
#include <sys/param.h>
#include <malloc.h>

#include "plugin_api.h"



//типы-указатели на функции plugin_api
typedef int (*ppf_func_t)(const char *, struct option *, size_t);
typedef int (*pgi_func_t)(struct plugin_info *);


//Cтруктура, в которой будет хранится все для запуска плагинов
struct plugin
{
    ppf_func_t process_file;
    struct option *in_opts;
    size_t in_opts_len;
};
void **plugin_handles = NULL;
struct plugin *plugin_array = NULL;
struct plugin_info *info_array = NULL;


//гп для хранения директории поиска
char *dir_search;


//гп для хранения пути поиска плагинов
char *plug_path = "./";


//гп для сохранения количества плагинов
int plugin_cnt = 0;


//Флаги для отладки, проверки и запуска плагинов соответственно
int Search_done_FLAG = 0;
int Debug_FLAG = 0;
int Or_FLAG = 0;
int Not_FLAG = 0;



//Запуск plugin_process_file
void CheckFilePlugins(const char *fpath)
{
    //Сколько плагинов сказали что файл подходит, и сколько плагинов успешно запустились
    int result = 0;
    int used = 0;
    for (int i = 0; i < plugin_cnt; i++)
    {
        if (plugin_array[i].in_opts_len > 0)
        {
            used++;
            int t = plugin_array[i].process_file(fpath, plugin_array[i].in_opts, plugin_array[i].in_opts_len);
            if (t < 0)
            {
                used--;
                fprintf(stderr, "Error in plugin №%d! %s", i, strerror(errno));
                if (errno == EINVAL)
                    fprintf(stderr, "errno is EINVAL, skipping this plugin for next files\n");
            }
            else if (t == 0)
                result++;
        }
    }
    if ( //Если файл подходит с учетом флагов Or_FLAG и Not_FLAG выводим
        (Not_FLAG == 0 && ((Or_FLAG == 1 && result > 0) || (Or_FLAG == 0 && result == used))) ||
        (Not_FLAG == 1 && ((Or_FLAG == 1 && result == 0) || (Or_FLAG == 0 && result < used))))
    {
        fprintf(stdout, "%d plugins returned true\n", result);
        fprintf(stdout, "File: %s\n", fpath);
    }
}



/*Принимает инфу о текущем пути файла fpath, структуру stat и флаг typeflag, указывающий тип файла. Если файл является обычным файлом FTW_F, то функция вызывает CheckFileLine() для анализа содержимого файла */
int CheckFile(const char *fpath, const struct stat *sb, int typeflag)
{
    (void)sb;
    if (typeflag == FTW_F)
        CheckFilePlugins(fpath);
    return 0;
}


/*Принимает инфу о текущем пути файла, структуру stat и флаг typeflag. Если файл - обычный FTW_F, и имеет в названии ".so", функция открывает эту библиотеку и проверяет на соответствие plugin_api*/
int CheckPlugin(const char *fpath, const struct stat *sb, int typeflag)
{
    (void)sb;
    if (typeflag == FTW_F && strstr(fpath, ".so") != NULL)
    {
        void *Lib_handle = dlopen(fpath, RTLD_LAZY);
        if (!Lib_handle)
        {
            if (Debug_FLAG)
                fprintf(stderr, "%s: dlopen() failed!: %s", fpath, dlerror());
            return 0;
        }
        else
        {
            pgi_func_t pgi = dlsym(Lib_handle, "plugin_get_info");
            ppf_func_t ppf = dlsym(Lib_handle, "plugin_process_file");
            if (!pgi || !ppf)
            {
                fprintf(stderr, "%s: dlsym for plugin_get_info or plugin_process_file failed: %s\n", fpath, dlerror());
                dlclose(Lib_handle);
                return 0;
            }
            struct plugin_info pi = {0};
            int res = pgi(&pi);
            if (res < 0)
            {
                fprintf(stderr, "%s: plugin_get_info returned %d, skipping it\n", fpath, res);
                dlclose(Lib_handle);
                return 0;
            }
            if (pi.sup_opts_len == 0)
            {
                fprintf(stderr, "%s: plugin supports no options, skipping it\n", fpath);
                dlclose(Lib_handle);
                return 0;
            }
            plugin_cnt++;
            info_array = realloc(info_array, sizeof(struct plugin_info) * plugin_cnt);
            info_array[plugin_cnt - 1] = pi;
            plugin_array = realloc(plugin_array, sizeof(struct plugin) * plugin_cnt);
            plugin_array[plugin_cnt - 1].process_file = ppf;
            plugin_array[plugin_cnt - 1].in_opts_len = 0;
            plugin_array[plugin_cnt - 1].in_opts = NULL;
            plugin_handles = realloc(plugin_handles, sizeof(void *) * plugin_cnt);
            plugin_handles[plugin_cnt - 1] = Lib_handle;
        }
    }
    return 0;
}

/*Начинает обход ФС, если еще не проверяли плагины, запускаем с функцией CheckPlugin в директории plug_path, иначе проходимся по dir_search функцией CheckFile*/
void WalkDirectory()
{
    if (Search_done_FLAG == 0)
    {   
        Search_done_FLAG = 1;
        int result = ftw(plug_path, CheckPlugin, FTW_NS);
        if (result < 0 && Debug_FLAG)
        {
            fprintf(stderr, "ftw() failed: %s\n", strerror(errno));
        }
        return;
    }
    else
    {
        int result = ftw(dir_search, CheckFile, FTW_NS);
        if (result < 0 && Debug_FLAG)
        {
            fprintf(stderr, "ftw() failed: %s\n", strerror(errno));
        }
    }
}

/*Функция для разбора аргументов ком строки, getopt нужен для обработки опций*/
void OptionDo(int argc, char *const *argv)
{
    int value = 0;
    opterr = 0;
    //Считаем сколько опций надо выделить (+2 для version и help)
    size_t opt_cnt = 2;
    for (int i = 0; i < plugin_cnt; i++)
        opt_cnt += info_array[i].sup_opts_len;
    struct option *long_options = calloc(opt_cnt + 1, sizeof(struct option));
    long_options[0] = (struct option){"help", 0, 0, 'h'};
    long_options[1] = (struct option){"version", 0, 0, 'v'};
    size_t opt_copy_idx = 2;
    //Копируем опции в long_options
    for (int i = 0; i < plugin_cnt; i++)
    {
        for (size_t j = 0; j < info_array[i].sup_opts_len; j++)
        {
            long_options[opt_copy_idx] = info_array[i].sup_opts[j].opt;
            opt_copy_idx++;
        }
    }

    int found_plug_opts = 0;
    while (1)
    {
        int found_cnt = 0;
        int optindex = 0;
        value = getopt_long(argc, argv, "hvP:OAN", long_options, &optindex);
        if (value == -1)
            break;
        //Выводимая инфа
        switch (value)
        {
        case 'h':
            fprintf(stdout, "-h, --help: Display information about options\
                \n-v, --version: Display information about programms\
                \n-P: Change plugin directory\
                \n-O: Use Or for plugin results\
                \n-A: Use And for plugin results (used by default)\
                \n-N: Use Not for plugin results\n");
            fprintf(stdout, "Available plugin options:\n");
            for (size_t i = 2; i < opt_cnt; i++)
            {
                fprintf(stdout, "--%s\n", long_options[i].name);
            }

            //Освобождаем память перед выходом
            for (int i = 0; i < plugin_cnt; i++)
            {
                if (dlclose(plugin_handles[i]) != 0)
                    fprintf(stderr, "dlclose error: %s\n", dlerror());
                if (plugin_array[i].in_opts)
                    free(plugin_array[i].in_opts);
            }
            if (plugin_handles)
                free(plugin_handles);
            if (plugin_array)
                free(plugin_array);
            if (info_array)
                free(info_array);
            exit(EXIT_SUCCESS);

        case 'v':
            fprintf(stdout, "Version 1.2\
                \nSamaykin Egor Sergeevich\
                \nN32491\
                \n19\n");
            //Освобождаем память перед выходом
            for (int i = 0; i < plugin_cnt; i++)
            {
                if (dlclose(plugin_handles[i]) != 0)
                    fprintf(stderr, "dlclose error: %s\n", dlerror());
                if (plugin_array[i].in_opts)
                    free(plugin_array[i].in_opts);
            }
            if (plugin_handles)
                free(plugin_handles);
            if (plugin_array)
                free(plugin_array);
            if (info_array)
                free(info_array);
            exit(EXIT_SUCCESS);

        case 'P':
            //Уже рассмотрел отдельно, чтобы не приходилось освобождать память и искать плагины дважды
            break;

        case 'O':
            Or_FLAG = 1;
            break;

        case 'A':
            Or_FLAG = 0;
            break;

        case 'N':
            Not_FLAG = 1;
            break;

        case '?':
            if (Debug_FLAG)
                fprintf(stderr, "Inputed unknown option: %c\
                    \nUsage: %s [options] directory target\n",
                        optopt, argv[0]);
            exit(EXIT_FAILURE);
        case 0:
            //Длинная опция для плагина
            for (int i = 0; i < plugin_cnt; i++)
            {
                for (size_t j = 0; j < info_array[i].sup_opts_len; j++)
                {
                    if(strcmp(long_options[optindex].name, info_array[i].sup_opts[j].opt.name) == 0){
                        found_plug_opts++;
                        found_cnt++;
                        plugin_array[i].in_opts_len++;
                        plugin_array[i].in_opts = realloc(plugin_array[i].in_opts, sizeof(struct option) * plugin_array[i].in_opts_len);
                        plugin_array[i].in_opts[plugin_array[i].in_opts_len-1] = long_options[optindex];
                        if(long_options[optindex].has_arg) plugin_array[i].in_opts[plugin_array[i].in_opts_len-1].flag = (int*)optarg;
                    }
                }
            }
            if(found_cnt > 1){
                fprintf(stderr, "Error: 2 options with the same name in plugins\nOpt name:%s\n",long_options[optindex].name);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            break;
        }
    }
    free(long_options);
    if (argc - optind < 1 || found_plug_opts == 0)
    {
        if (Debug_FLAG)
            fprintf(stderr, "Usage: %s [options] directory\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    dir_search = argv[optind];
}

int main(int argc, char *const *argv)
{ //(int argc, char **argv)
    //Проверка флага переменной окружения
    if (getenv("LAB1DEBUG"))
    {
        Debug_FLAG = 1;
    }
    //Проверка опции -P перед запуском поиска плагинов
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "-P") == 0)
        {
            if (Debug_FLAG)
                fprintf(stderr, "-P found\n");
            plug_path = argv[i + 1];
            break;
        }
    }
    WalkDirectory();
    OptionDo(argc, argv);

    //Информация была нужна для получения опций, теперь нужно освободить
    if (info_array)
        free(info_array);
    WalkDirectory();

    for (int i = 0; i < plugin_cnt; i++)
    {
        if (dlclose(plugin_handles[i]) != 0)
            fprintf(stderr, "dlclose error: %s\n", dlerror());
        if (plugin_array[i].in_opts)
            free(plugin_array[i].in_opts);
    }
    if (plugin_handles)
        free(plugin_handles);
    if (plugin_array)
        free(plugin_array);
    return 0;
}
