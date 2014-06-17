# define _GNU_SOURCE
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <strings.h>
# include "memory_limit.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return -1;
    }
    long setvalue = 0;
    long setrate = 0;
    long *psetvalue = &setvalue;
    long *psetrate = &setrate;
    char * memint = NULL;
    char * mem_set_value = argv[1];
    
    set_opt(mem_set_value, psetrate, psetvalue);
    
    drop_cache("3");

    while (1)
    {
        if(setvalue < free_mem())
        {
            memint = (char *)malloc(setrate *sizeof(char));
            memset(memint, '0', setrate);
            if (memint == NULL)
            {
                printf("Malloc error!");
                while (1);
            }
        }
        else
        {
            printf("The free memory is %ld bytes now.\n", free_mem());
            while (1) ;
        }
    }
    free(memint);
    return 0;
}

long free_mem()
{
    char * line = NULL;
    ssize_t read;
    size_t len = 0;
    FILE * f = fopen("/proc/meminfo", "r");
    if (f == NULL)
    {
        return -1;
    }
    read = getline(&line, &len, f);
    read = getline(&line, &len, f);
    fclose(f);
    char *newchar = (char*)malloc(read * sizeof(char));
    char *opt = newchar;
    int i = 0;
    for (i = 0; i < read; i++)
    {
        if ((line[i] >= '0') && (line[i] <= '9'))
        {
            *opt = line[i];
            opt++;
        }
    }
    *opt = '\0';
    opt = NULL;
    long ret = atoi(newchar);
    free(newchar);
    return ret * 1024;
}


int set_opt(char * pmem, long * psetrate, long * psetvalue)
{
    char setunit = '0';
    setunit = *(pmem + strlen(pmem) - 1);
    if ('G' == setunit || 'g' == setunit)
    {
        *psetrate = 1024 * 1024;
        *(pmem + strlen(pmem) - 1) = '\0';
        *psetvalue = (*psetrate) * 1024 * atoi(pmem);
        printf("Set free memory to: %ld bytes.\n", *psetvalue);
    }
    else if ('M' == setunit || 'm' == setunit)
    {
        *psetrate = 1024 * 1024;
        *(pmem + strlen(pmem) - 1) = '\0';
        *psetvalue = (*psetrate) * atoi(pmem);
        printf("Set free memory to: %ld bytes.\n", *psetvalue);
    }
    else if ('K' == setunit || 'k' == setunit)
    {
        *psetrate = 1024;
        *(pmem + strlen(pmem) - 1) = '\0';
        *psetvalue = (*psetrate) * atoi(pmem);
        printf("Set free memory to: %ld bytes.\n", *psetvalue);
    }
    else if ('B' == setunit || (setunit >= '0' && setunit <= '9'))
    {
        *psetrate = 1;
        *psetvalue = (*psetrate) * atoi(pmem);
        printf("Set free memory to: %ld bytes.\n", *psetvalue);
    }
    else
    {
        printf("Input error!\n");
        return -1;
    }
    return 0;
}

int drop_cache(char * value)
{
    int ret = 0;
    FILE * fp = fopen("/proc/sys/vm/drop_caches", "w");
    if(!fwrite(value, strlen(value), 1, fp))
    {
        ret = -1;
    }
    fclose(fp);
    fp = NULL;
    return ret;
}
