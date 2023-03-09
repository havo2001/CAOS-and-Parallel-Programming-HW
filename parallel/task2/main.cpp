#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

using namespace std;

double time_tot = 0.1;        // total time 
double dt =  0.0002;           // time step
double h = 0.02;               // step on the x-coordinate
double l = 1;                 // length of the rod
double u_0 = 1;               // initial value
double pi = 3.14159265358;

int main(int argc, char *argv[])
{	
	int i, m;
    	double time, x, sum, a;
    	int nPoints = 49;
	double u_prev[nPoints + 2];   // first array for the numerical solution
    	double u_next[nPoints + 2];   // second array for the numerical solution
    	double u_exact[nPoints + 2];  // array for the exact solution

    
    	// Initial conditions
    	for (i = 1; i <= nPoints; i++){
		u_prev[i] = u_0;
        	u_next[i] = u_0;    
    	}      
    
    	// Boundary conditions
    	u_prev[0] = 0;
    	u_prev[nPoints + 1] = 0;
    	u_next[0] = 0;
    	u_next[nPoints + 1] = 0;


	MPI_Init(&argc, &argv);
	int size;
	int rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int k = nPoints/size;
	int remainder = nPoints%size;
	int left, right, amount;

	
	//MPI_Barrier(MPI_COMM_WORLD);
	if (rank < remainder) {
		left = rank * k + rank + 1;
		right = left + k;
		amount = k + 1;
	} else {
		left = rank * k + remainder + 1;
		right = left + k - 1;
	        amount  = k;	
	}
	
    	// main loop on time
    	time = 0;
    	while (time < time_tot) {
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank % 2 != 0) {
			if (rank > 0) 
				MPI_Send(&u_prev[left], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
			if (rank > 0) 
				MPI_Recv(&u_prev[left - 1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (rank < size - 1) 
                                MPI_Recv(&u_prev[right + 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (rank < size - 1) 
                                MPI_Send(&u_prev[right], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);		
  		}
		else {
                        if (rank < size - 1) 
                                MPI_Recv(&u_prev[right + 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        if (rank < size - 1) 
                               	MPI_Send(&u_prev[right], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
                        if (rank > 0)
                        	MPI_Send(&u_prev[left], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
                        if (rank > 0) 
                                MPI_Recv(&u_prev[left - 1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

                for (i = left; i <= right; i++){
	                u_next[i] = u_prev[i] + dt /(h * h) * (u_prev[i + 1] - 2 * u_prev[i] + u_prev[i - 1]);
		}
		for (i = left; i <= right; i++){
			u_prev[i] = u_next[i];
                }
               	time = time + dt;
	}
	
	//MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		for (i = 1; i < size; ++i) {
			if (i < remainder) {
				left = i * k + rank + 1;
				amount = k + 1;
			} else {
				left = i * k + remainder + 1;
				amount = k;
			}
			MPI_Recv(&u_next[left], amount, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// printing the numerical solution on the screen
		printf("Numerical solution: \n");
		for (i = 1; i <= nPoints; i++) {
			printf("%f ", u_next[i]);
		}
                printf("\n");
                printf("\n");

	} else {
		MPI_Send(&u_next[left], amount, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
	}
   
	//MPI_Barrier(MPI_COMM_WORLD);	
	if (rank == 0) {
   		// exact solution
    		printf("Exact solution: \n");
    		for (i = 1; i <= nPoints; i++){
       			x = i * h;
       			sum = 0;
    			for (m = 0; m < 5; m++){
       				a =  exp(- pi * pi * (2*m+1) * (2*m+1) * time_tot) * sin( pi * (2*m+1) * x / l) / (2*m+1);
				sum = sum + 4 * u_0 * a/ pi;
   			}
    			u_exact[i] = sum;
    			// printing the exact solution on the screen 
    			printf("%f  ", u_exact[i]);
			}
			printf("\n");
	}

	MPI_Finalize();
    	system("PAUSE");
    	return EXIT_SUCCESS;
}

