/*
 * =====================================================================================
 *
 *       Filename:  get_cpu_usage.c
 *
 *    Description:  Prints CPU usage percentage in total and for each core
 *
 *        Version:  1.0
 *        Created:  26-12-2016 15:33:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SITKI BURAK CALIM (https://github.com/sbcalim), sburakcalim@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define PROCSTAT "/proc/stat"
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_number_of_cpu_cores
 *  Description:  How many CPU cores are there?
 * =====================================================================================
 */
	int	
get_number_of_cpu_cores ( )
{
	FILE *fp = fopen(PROCSTAT, "r");

	int n = 0;
	char line[100];
				
	while(!feof(fp))
	{
		fgets(line, 100, fp); 
		if(line[0] == 'c' && line[1] == 'p' && line[2] == 'u') n++;
	}
	fclose(fp);

	return n - 1;
}		/* -----  end of function get_number_of_cpu_cores  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  read_cpu
 *  Description:  Reads /proc/stat file and returns an array[cpu_num][process_type] of process count
 * =====================================================================================
 */
	unsigned long long
**read_cpu ( )
{
	FILE *fp;
	unsigned long long **array;
	char buffer[1024];
	int i, j;
	unsigned long long ignore[6];
	int cpus = get_number_of_cpu_cores();

	array = (unsigned long long **)malloc((cpus + 1) * sizeof(unsigned long long));

	fp = fopen(PROCSTAT, "r");

	for(i = 0; i<cpus + 1; i++)
	{
		array[i] = (unsigned long long *)malloc(4 * sizeof(unsigned long long));
		fscanf(fp, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu ",
				&array[i][0], &array[i][1], &array[i][2], &array[i][3], 
				&ignore[0], &ignore[1], &ignore[2], &ignore[3], &ignore[4], &ignore[5]);
	}
	fclose(fp);
	return array;
}		/* -----  end of function read_cpu  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_cpu_percentage
 *  Description:  Returns a double value of CPU usage percentage from given array[4] = {USER_PROC, NICE_PROC, SYSTEM_PROC, IDLE_PROC}
 * =====================================================================================
 */
	double
get_cpu_percentage ( unsigned long long *a1, unsigned long long *a2 )
{
	return (double)(((double)((a1[0] - a2[0]) + (a1[1] - a2[1]) + (a1[2] - a2[2])) / 
				(double)((a1[0] - a2[0]) + (a1[1] - a2[1]) + (a1[2] - a2[2]) + (a1[3] - a2[3]))) * 100);
}		/* -----  end of function get_cpu_percentage  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Get two results between /proc/stat values and print usage percentage
 * =====================================================================================
 */
	int
main ( int argc, char *argv[] )
{
	int i, num_of_cpus = get_number_of_cpu_cores();	
	double *percentage;	

	printf("CPU");
	for(i = 1; i <= num_of_cpus; i++)
	{
		printf("\tCPU-%d", i);
	}
	printf("\n");

	while(1)
	{
		unsigned long long **p_cpu_array = read_cpu();
		sleep(1);
		unsigned long long **n_cpu_array = read_cpu();

		for(i = 0; i<=num_of_cpus; i++)
		{
			printf("%.2lf\t", get_cpu_percentage(n_cpu_array[i], p_cpu_array[i]));
		}
		printf("\n");
		
		for(i = 0; i < num_of_cpus; i++)
		{
			free(p_cpu_array[i]);
			free(n_cpu_array[i]);
		}
	}
	
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

