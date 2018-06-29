#include <math.h>
#include <stdio.h>

#include <task/structs/task.h>
#include <task/structs/taskset.h>
#include <task/structs/periodic_server.h>
#include <task/task_io.h>
#include <task/testing_set.h>
#include <task/utilities.h>
#include <schedulability/sbf.h>
#include <schedulability/dbf.h>
#include <schedulability/h_analysis.h>

#define MAX_SERVER_BANDWITH 0.8

void print_h_schedulability(unsigned int is_schedulable, s_algorithm a, periodic_server * ps, FILE *f) {
	if(is_schedulable)
		fprintf(f, "\nThe taskset is schedulable under %s with the specified periodic server (Qs = %u, Ts = %u).\n", s_algorithm_to_string(a), ps->Qs, ps->Ts);
	else
		fprintf(f, "\nThe taskset is NOT schedulable under %s with the specified periodic server (Qs = %u, Ts = %u).\n", s_algorithm_to_string(a), ps->Qs, ps->Ts);
}

unsigned int h_analysis_fp(taskset *ts, periodic_server *ps, FILE *f) {
	unsigned int testing_set_f[MAX_TESTING_SET_SIZE], testing_set_s[MAX_TESTING_SET_SIZE], testing_set[MAX_TESTING_SET_SIZE], i, is_schedulable = 0;
	int j, n_testing_set, n_testing_set_f, n_testing_set_s;

	for(i = 0; i < ts->size; i++) {
		n_testing_set_f = testing_set_fp(ts, testing_set_f, i);
		n_testing_set_s = testing_set_sbf(ps, testing_set_s, testing_set_f[n_testing_set_f - 1]);

		if ((n_testing_set = merge_testing_sets(testing_set_f, n_testing_set_f, testing_set_s, n_testing_set_s, testing_set)) > 0) {
			print_testing_set_fp(testing_set, n_testing_set, ts, i, f);
			for (j = 0; j < n_testing_set; j++) {
				fprintf(f, "\t dbf(%d) = %u; sbf(%d) = %u\n", testing_set[j], workload(ts, i, testing_set[j]), testing_set[j], sbf(ps, testing_set[j]));

				if (workload(ts, i, testing_set[j]) <= sbf(ps, testing_set[j])) 
					is_schedulable = 1;
			}
			
			fprintf(f, "\n");
		}
	}

	return is_schedulable;
}

unsigned int h_analysis_edf(taskset *ts, periodic_server *ps, FILE *f){
	unsigned int testing_set_e[MAX_TESTING_SET_SIZE], testing_set_s[MAX_TESTING_SET_SIZE], testing_set[MAX_TESTING_SET_SIZE], is_schedulable = 1;
	int i, n_testing_set, n_testing_set_e, n_testing_set_s;
	
	n_testing_set_e = testing_set_edf(ts, testing_set_e);
	n_testing_set_s = testing_set_sbf(ps, testing_set_s, testing_set_e[n_testing_set_e - 1]);

	if ((n_testing_set = merge_testing_sets(testing_set_e, n_testing_set_e, testing_set_s, n_testing_set_s, testing_set)) > 0) {
		print_testing_set_edf(testing_set, n_testing_set, f); 
		for (i = 0; i < n_testing_set; i++) {
			fprintf(f, "\t dbf(%d) = %f; sbf(%d) = %u\n", testing_set[i], dbf(ts, testing_set[i]), testing_set[i], sbf(ps, testing_set[i]));

			if (dbf(ts, testing_set[i]) > sbf(ps, testing_set[i])) 
				is_schedulable = 0;
		}
	}
	
	return is_schedulable;
}

periodic_server * find_periodic_server(taskset *ts, s_algorithm a, int cpu, FILE *f) {
	unsigned int start_Qs, end_Qs, Qs, start_Ts, end_Ts, Ts, is_schedulable;
	double temp_bandwith, best_bandwith = 1.0;
	periodic_server * temp_ps, * ps = create_empty_ps();
	FILE * null = fopen("./null", "r");

	if(cpu > 0)	
		fprintf(f, "Cpu %d : finding a periodic server for the taskset. ", cpu);
	else
		fprintf(f, "Let's try to find a periodic server that can schedule the entire taskset. ");

	start_Ts = min_period(ts);
	end_Ts = 2 * max_period(ts);
	for(Ts = start_Ts; Ts < end_Ts; Ts++) {
		//start_Qs = 0;
		start_Qs = (unsigned int) ceil((double) Ts * utilization_factor(ts));
		end_Qs = (unsigned int) ceil((double) Ts * best_bandwith);
		for(Qs = start_Qs; Qs <= end_Qs; Qs++) {
			temp_ps = load_periodic_server(Qs, Ts);
			temp_bandwith = (double) temp_ps->Qs / temp_ps->Ts;
			
			is_schedulable = (a == EDF) ? h_analysis_edf(ts, temp_ps, null) : h_analysis_fp(ts, temp_ps, null);
			if(is_schedulable && temp_bandwith < best_bandwith) {
				ps = temp_ps;
				best_bandwith = temp_bandwith;
			}
		}
	}

	print_periodic_server(ps, f);
			
	return ps;
}
