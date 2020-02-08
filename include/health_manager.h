#ifndef TESTGAME_HEALTH_MANAGER_H
#define TESTGAME_HEALTH_MANAGER_H

#include <corange.h>

typedef struct {
	int current;
} health;

health* health_new();

void health_delete(health* h);

#endif //TESTGAME_HEALTH_MANAGER_H
