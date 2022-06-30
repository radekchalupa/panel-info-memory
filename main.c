/* 
 * parametry
 * -v delsi vypis
 * -s simple - usporny vypis
 * -l[label] vlastni nadpis
 * -p vypois v procentech
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#define MAX_LABEL_LENGTH 32
#define OUTPUT_PCT 'p'
#define OUTPUT_VALUE 'v'

static int print_output(char* szlabel, char argoutput, int verbose)
{
	FILE* pfile = fopen("/proc/meminfo", "r");
	if (NULL == pfile)
	{
		printf("ERROR");
		return EXIT_FAILURE;
	}
	char line[256];
	unsigned int mem_total;
	unsigned int mem_avail;
	unsigned int swap_total;
	unsigned int swap_free;
	unsigned int end = 0;
	while (fgets(line, sizeof(line), pfile))
	{
		if (sscanf(line, "MemTotal: %u kB", &mem_total) == 1)
		{
			end |= 1;
			continue;
		}
		if (sscanf(line, "MemAvailable: %u kB", &mem_avail) == 1)
		{
			end |= 2;
			continue;
		}
		if (sscanf(line, "SwapTotal: %u kB", &swap_total) == 1)
		{
			end |= 4;
			continue;
		}
		if (sscanf(line, "SwapFree: %u kB", &swap_free) == 1)
		{
			end |= 8;
			continue;
		}
		if (0xF == end)
			break;
	}
	fclose(pfile);
	char sztext[32];
	char szoutput[64 + MAX_LABEL_LENGTH];
	*sztext = 0;
	*szoutput = 0;
	if (szlabel)
		strcat(szoutput, szlabel);
	if (OUTPUT_PCT == argoutput)
		sprintf(sztext, "%u", 100*(mem_total - mem_avail) / mem_total);
	else
		sprintf(sztext, verbose ? "%.2f GiB" :  "%.2fG",
			(double)(mem_total - mem_avail) / (1024 * 1024));
	strcat(szoutput, sztext);
	if (OUTPUT_PCT == argoutput)
		sprintf(sztext, " - %u", 100*(swap_total - swap_free) / swap_total);
	else
		sprintf(sztext, verbose ? " - %.2f GiB" : "-%.2fG",
			(double)(swap_total - swap_free) / (1024 * 1024));
	strcat(szoutput, sztext);
	printf(szoutput);
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	int opt;
	char szlabel[MAX_LABEL_LENGTH + 1];
	*szlabel = 0;
	char argoutput = OUTPUT_VALUE;
	int verbose = 1;
	if (argc > 1)
	{
		while ((opt = getopt(argc, argv, "vspl:")) != -1)
		{
			switch (opt)
			{
			case 'l':
				if (strlen(optarg) <= MAX_LABEL_LENGTH)
					strcpy(szlabel, optarg);
				break;
			case 'p':
				argoutput = OUTPUT_PCT;
				break;
			case 's':
				verbose = 0;
				break;
			case 'v':
				verbose = 1;
				break;
			}
		}
	}
	return print_output(szlabel, argoutput, verbose);
}
