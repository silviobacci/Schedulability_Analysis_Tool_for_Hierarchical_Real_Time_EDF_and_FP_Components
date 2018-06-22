#ifndef CPU_H
#define CPU_H

#define MAX_USAGE_COMPONENT	1.0
#define MAX_NUMBER_CORES 	16

typedef struct cpu_ {
	unsigned int n_cores;
	core * cores;
} cpu;

#endif
