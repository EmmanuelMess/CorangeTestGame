
#include "../include/health_manager.h"

health* health_new() {
	return malloc(sizeof(health));
}

void health_delete(health* h) {
	free(h);
}

