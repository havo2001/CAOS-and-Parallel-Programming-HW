#include <stdlib.h>
#include <stdio.h>
#include <math.h>


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
    
    // main loop on time
    time = 0;
    while (time < time_tot){
          for (i = 1; i <= nPoints; i++){
              // finite difference scheme
              u_next[i] = u_prev[i] + dt / (h * h) * (u_prev[i + 1] - 2 * u_prev[i] + u_prev[i - 1]);
          }
          for (i = 1; i <= nPoints; ++i){		
              u_prev[i] = u_next[i];
          }
          time = time + dt;
    }
    
    // printing the numerical solution on the screen 
    printf("Numerical solution: \n");
    for (i = 1; i <= nPoints; i++){
         printf("%f  ", u_next[i]);
    }
    printf("\n");
    printf("\n");    
    
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

    system("PAUSE");
    return EXIT_SUCCESS;
}

