#define _GNU_SOURCE 	//Для для strdup
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>

#include "plugin_api.h"



static char *g_lib_name = "libessN32491.so";
static char *g_plugin_purpose = "Search for ipv4 addr in binary form";
static char *g_plugin_author = "Samaykin Egor Sergeevich";
static size_t g_opts_len = 1;
static struct plugin_option g_sup_opts[1] ={
    {
        {"ipv4-addr-bin", required_argument, 0, 0},
        "IP adrress in XXX.XXX.XXX.XXX format"}};


int plugin_get_info(struct plugin_info *ppi)
{
    //Если ppi - NULL возвращается ошибка
    if (!ppi)
        return -1;

    //Заполняем ppi и возвращаем успех
    ppi->plugin_author = g_plugin_author;
    ppi->plugin_purpose = g_plugin_purpose;
    ppi->sup_opts_len = g_opts_len;
    ppi->sup_opts = g_sup_opts;
    return 0;
}



int plugin_process_file(const char *fname,
                        struct option in_opts[],
                        size_t in_opts_len)
{
    //Если не получили какой то из аргументов выходим с ошибкой
    if (!fname || !in_opts || !in_opts_len)
        return -1;
    int ret = -1;
    char *debug = getenv("LAB1DEBUG");
    /*Получение IP из входных опций*/
    char *ip = NULL;
    for (size_t i = 0; i < in_opts_len; i++)
    {
        if (strcmp(in_opts[i].name, "ipv4-addr-bin") == 0)
        {
            ip = strdup((char *)in_opts[i].flag);
        }
        else
        {
            errno = EINVAL;
            return -1;
        }
    }
    if (!ip || strlen(ip) > 15 || strlen(ip) < 7 || !strstr(ip, "."))
    {
        /*IP адрес это от 7 символов (0.0.0.0), и до 15 (255.255.255.255) поэтому если длина не входит в этот промежуток, то входные данные неправильные*/
        errno = EINVAL;
        free(ip);
        return -1;
    }
    //Разбиваем входной IP на 4 байта, и отдельно сохраняем в обратном порядке для big-endian и little-endian проверки
    unsigned char byte_array[4];
    unsigned char byte_array_rev[4];
    for (int i = 0; i < 4; i++)
    {
        char *t;
        if (i == 0)
            t = strtok(ip, ".");
        else
            t = strtok(NULL, ".");
        if (t == NULL)
        {
            //Если не нашелся какой-то токен из четырех, т.е. их слишком мало, то неверные входные данные
            if (debug)
                fprintf(stderr, "%s: tokens failed\n", g_lib_name);
            errno = EINVAL;
            free(ip);
            return -1;
        }
        unsigned char value = strtoul(t, NULL, 10);
        if (value == 0 && strcmp(t, "0") != 0)
        {
            //Если ошибка с конвертацией char* в значение, то значит в токене не было числа
            if (debug)
                fprintf(stderr, "%s: strtoul() failed\n", g_lib_name);
            errno = EINVAL;
            free(ip);
            return -1;
        }
        byte_array[i] = (char)value;
        byte_array_rev[3 - i] = (char)value;
    }
    free(ip);
    //Проверка файла
    FILE *fd = fopen(fname, "rb");
    if (!fd)
    {
        //Если не получилось открыть файл fopen() сам выставляет errno
        return -1;
    }
    while (!feof(fd))
    {
        unsigned char buffer[4];
        //Считываем из файла по 4 байта и сверяем
        int t = fread(buffer, sizeof(unsigned char), 4, fd);
        if (t < 4)
        {
            break;
        }
        else if ((memcmp(buffer, byte_array, 4) == 0) || (memcmp(buffer, byte_array_rev, 4) == 0))
        {
            //Если нашли в прямом или обратном порядке
            ret = 0;
            if (fclose(fd) < 0)
            {
                fprintf(stderr, "Failed to fclose() file!!\n");
            }
            return 0;
        }
    }
    ret = 1;
    if (fclose(fd) < 0)
    {
        fprintf(stderr, "Failed to fclose() file!\n");
    }
//Освобождаем то, что выделили
    return ret;
}
