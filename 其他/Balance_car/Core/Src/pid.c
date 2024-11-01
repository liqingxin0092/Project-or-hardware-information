#include "pid.h"
#include "main.h"

// positional mode PID realise
float  PosPID_Trans(pid_t *ptr, float act)
{
	static float err_p, err_i, err_d, err, err_las = 0;

	err = ptr->tar - act;

	err_p = err;		   // proportional part
	err_i += err;		   // Integral part
	err_d = err - err_las; // Differential part

    if(err_i>600)
        err_i=600;
    else if (err_i<-600)
        err_i=-600;
    
	err_las = err; // Error transfer

	return ptr->p * err_p + ptr->i * err_i + ptr->d * err_d;
}

// incremental mode PID realise
float IncPID_Trans(pid_t *ptr, float act)
{
	static float err_p, err_i, err_d, err, err_las = 0, err_pre = 0;

	err = ptr->tar - act;

	err_p = err - err_las;						   // proportional part
	err_i = err;								   // Integral part
	err_d = (err - err_las) - (err_las - err_pre); // Differential part

	err_pre = err_las; // Error transfer
	err_las = err;

	return ptr->p * err_p + ptr->i * err_i + ptr->d * err_d;
}
