//------------------------------------------------------------------------------
// TASK IO:	Contains functions to load, print and add tasks to tasksets.
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "task/types.h"
#include "task/utilities.h"
#include "task/task_io.h"

//------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PRINT TASKSET: prints all the parameters of a taskset
//------------------------------------------------------------------------------

void print_taskset(taskset *ts, FILE *f) {
	unsigned int i;

	fprintf(f, "The taskset is composed by the following tasks expressed with the (C,D,T) model:\n");

	for (i = 0; i < ts->size; i++)
		fprintf(f, "\t Task %u : (%u, %u, %u)\n", ts->tasks[i].id, ts->tasks[i].C, ts->tasks[i].D, ts->tasks[i].T);

	fprintf(f, "\n");
	fprintf(f, "The utilization of the taskset is %f\n\n",  utilization_factor(ts));
}

//------------------------------------------------------------------------------
// PRINT PERIODIC SERVER: prints all the parameters of a periodic server
//------------------------------------------------------------------------------

void print_periodic_server(periodic_server *ps, FILE *f) {
	fprintf(f, "The periodic server has the following paramters: \n\t Qs = %d \n\t Ts = %d \n\n", ps->Qs, ps->Ts);
}

//------------------------------------------------------------------------------
// CREATE EMPTY TS: allocate the memory space for an empty taskset
//------------------------------------------------------------------------------

taskset * create_empty_ts() {
	taskset *ts = malloc(sizeof(taskset));

	if (ts == NULL)
		return NULL;

	ts->tasks = malloc(sizeof(task) * MAX_NUMBER_TASKS);
	
	if (ts->tasks == NULL) {
		free(ts);
		return NULL;
	}
	
	ts->size = 0;

	return ts;
}

//------------------------------------------------------------------------------
// CREATE EMPTY PS: allocate the memory space for an empty periodic server
//------------------------------------------------------------------------------

periodic_server * create_empty_ps() {
	periodic_server * ps = malloc(sizeof(periodic_server));
	
	if (ps == NULL)
		return NULL;

	return ps;
}

//------------------------------------------------------------------------------
// ADD TO TEMP TASKSET: returns a new taskset merging the taskset and the task
//------------------------------------------------------------------------------

taskset *add_to_temp_taskset(taskset *previous_ts, task to_add) {
	unsigned int i = 0;

	taskset * new_ts = create_empty_ts();
	
	for(i = 0; i < previous_ts->size; i++)
		new_ts->tasks[i] = previous_ts->tasks[i];
	new_ts->tasks[previous_ts->size] = to_add;
	
	new_ts->size = previous_ts->size + 1;
	
	return new_ts;
}

//------------------------------------------------------------------------------
// ADD TO TASKSET: as the previous function but it deallocates previous_ts
//------------------------------------------------------------------------------

taskset *add_to_taskset(taskset *previous_ts, task to_add) {
	taskset *new_ts = add_to_temp_taskset(previous_ts, to_add);
	free(previous_ts);
	return new_ts;
}

//------------------------------------------------------------------------------
// LOAD TASKSET: load the taskset specified in the file f
//------------------------------------------------------------------------------

taskset * load_taskset(FILE *f) {
	int res;
	unsigned int i = 0;
	taskset * ts = create_empty_ts();

	while(!feof(f) && (i < MAX_NUMBER_TASKS)) {
		res = fscanf(f, "%u %u %u\n", &ts->tasks[i].C, &ts->tasks[i].D, &ts->tasks[i].T);
		ts->tasks[i].id = i + 1;
		if (res == 3)
			i++;
	}

	ts->size = i;

	return ts;
}

//------------------------------------------------------------------------------
// LOAD PERIODIC SERVER: creates a periodic server with the specified Qs and Ts
//------------------------------------------------------------------------------

periodic_server * load_periodic_server(unsigned int Qs, unsigned int Ts) {
	periodic_server * ps = create_empty_ps();
	
	ps->Qs = Qs;
	ps->Ts = Ts;

	return ps;
}
