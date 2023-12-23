/*
 * parametry
 * -v delsi vypis
 * -s simple - usporny vypis
 * -l[label] vlastni nadpis
 * -p vypis v procentech
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>

#define MAX_LABEL_LENGTH 32
#define MEMORY_INFO_PERCENT 1
#define MEMORY_INFO_VERBOSE 2

static void show_help(void)
{
    puts("parametry:");
    puts("  -p  zobrazi hodnoty v procentech");
    puts("  -v  podrobnejsi vypis");
    puts("  -s  kratky vypis");
    puts("  -l <nadpis>");
}

static int print_output(const char* szlabel, uint_fast32_t params)
{
    FILE* pfile = fopen("/proc/meminfo", "r");
    if (NULL == pfile) {
        printf("ERROR");
        return EXIT_FAILURE;
    }
    char line[256];
    unsigned int mem_total = 0;
    unsigned int mem_avail = 0;
    unsigned int swap_total = 0;
    unsigned int swap_free = 0;
    unsigned int end = 0;
    while (fgets(line, sizeof(line), pfile)) {
        if (sscanf(line, "MemTotal: %u kB", &mem_total) == 1) {
            end |= 1;
            continue;
        }
        if (sscanf(line, "MemAvailable: %u kB", &mem_avail) == 1) {
            end |= 2;
            continue;
        }
        if (sscanf(line, "SwapTotal: %u kB", &swap_total) == 1) {
            end |= 4;
            continue;
        }
        if (sscanf(line, "SwapFree: %u kB", &swap_free) == 1) {
            end |= 8;
            continue;
        }
        if (0xF == end)
            break;
    }
    fclose(pfile);
    if (0 == mem_total)
        return EXIT_FAILURE;
    char sztext[32];
    char outbuff[64 + MAX_LABEL_LENGTH];
    *sztext = 0;
    *outbuff = 0;
    if (szlabel) {
        strncat(outbuff, szlabel,
               sizeof(outbuff) - strlen(outbuff) - 1);
        strncat(outbuff, " ", sizeof(outbuff) - strlen(outbuff) - 1);
    }
    if (MEMORY_INFO_PERCENT & params) {
        sprintf(sztext, "%u", 100*(mem_total - mem_avail) / mem_total);
        strncat(sztext, "%", sizeof(outbuff) - strlen(outbuff) - 1);
    }
    else
        sprintf(sztext, (MEMORY_INFO_VERBOSE & params) ? "%.2f GiB" :  "%.2fG",
            (double)(mem_total - mem_avail) / (1024 * 1024));
    strncat(outbuff, sztext, sizeof(outbuff) - strlen(outbuff) - 1);
    if (0 == swap_total)
        return EXIT_SUCCESS;
    if (MEMORY_INFO_PERCENT & params) {
        sprintf(sztext, " - %u", 100*(swap_total - swap_free) / swap_total);
        strncat(sztext, "%", sizeof(outbuff) - strlen(outbuff) - 1);
    }
    else
        sprintf(sztext, (MEMORY_INFO_VERBOSE & params) ? " - %.2f GiB" : " - %.2fG",
            (double)(swap_total - swap_free) / (1024 * 1024));
    strncat(outbuff, sztext, sizeof(outbuff) - strlen(outbuff) - 1);
    puts(outbuff);
    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    int opt;
    uint_fast32_t params = 0;
    char szlabel[MAX_LABEL_LENGTH];
    *szlabel = 0;
    if (argc > 1) {
        while ((opt = getopt(argc, argv, "vsphl:")) != -1) {
            switch (opt) {
            case 'l':
                if (strlen(optarg) <= sizeof(szlabel) - 1)
                    strncpy(szlabel, optarg,
                        sizeof(szlabel) - strlen(szlabel) - 1);
                break;
            case 'p':
                params |= MEMORY_INFO_PERCENT;
                break;
            case 's':
                params &= ~MEMORY_INFO_VERBOSE;
                break;
            case 'v':
                params |= MEMORY_INFO_VERBOSE;
                break;
            case 'h':
                show_help();
                return EXIT_SUCCESS;
                break;
            }
        }
    }
    return print_output(szlabel, params);
}

