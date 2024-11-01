#ifndef __pid_H
#define __pid_H

#include "main.h"

typedef struct
{
	float tar;
	float p, i, d;
} pid_t;

// function
float PosPID_Trans(pid_t *ptr, float act);
float IncPID_Trans(pid_t *ptr, float act);

#endif
