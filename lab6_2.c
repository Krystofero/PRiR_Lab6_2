#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h

double funkcja(double x)
{
	return x*x;
}

int main(int argc, char **argv) {
	int l_procesow, nr_akt_proc, a, b, i, tag;
	double TrapezWynik;
	double ProstokatWynik;
	double h;
	
	tag = 20;
	a = 2;
	b = 8;
	
	MPI_Init(&argc, &argv); //inicjalizacja œrodowiska MPI
	MPI_Comm_size(MPI_COMM_WORLD, &l_procesow); // okresla liczbê uruchomonych procesow tworz¹cych sieæ
	MPI_Comm_rank(MPI_COMM_WORLD, &nr_akt_proc); // okresla numer aktualnego procesu
	MPI_Status status;
	
	if(nr_akt_proc == l_procesow-1) {
		TrapezWynik = 0;
		ProstokatWynik = 0;
		h = (double)(b - a) / l_procesow; //przedzia³ <a,b> podzielony na n równych czêœci (h = odlegloœæ miêdzy xi a xi+1)
		i = 1;
		TrapezWynik += f_x(a + i * h); 
		ProstokatWynik += f_x(a + i * h); 
		i += 1;
		
		MPI_Send(&TrapezWynik, 1, MPI_DOUBLE, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		MPI_Send(&ProstokatWynik, 1, MPI_DOUBLE, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		MPI_Send(&i, 1, MPI_INT, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		MPI_Send(&h, 1, MPI_DOUBLE, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		MPI_Send(&a, 1, MPI_INT, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		MPI_Send(&b, 1, MPI_INT, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		
	}
	if((nr_akt_proc < l_procesow-1) && (nr_akt_proc >= 0)) {// sprawdzam czy aktualny proces znajduje siê w przedziale liczby procesów i czy istniej¹ jakieœ procesy
		MPI_Recv(&i, 1, MPI_INT, nr_akt_proc+1, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&a, 1, MPI_INT, nr_akt_proc+1, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&b, 1, MPI_INT, nr_akt_proc+1, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&h, 1, MPI_DOUBLE, nr_akt_proc+1, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&TrapezWynik, 1, MPI_DOUBLE, nr_akt_proc+1, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&ProstokatWynik, 1, MPI_DOUBLE, nr_akt_proc+1, tag, MPI_COMM_WORLD, &status);
		
		if(nr_akt_proc == 0) {
			TrapezWynik += f_x(a) + f_x(b);
			TrapezWynik *= h / 2;
			printf("Wynik Metody Trapezów = %f", TrapezWynik);
			
			ProstokatWynik *= h;
			printf("Wynik Metody prostok¹tów = %f", ProstokatWynik);
		}
		else {
			TrapezWynik += f_x(a + i * h); 
			ProstokatWynik += f_x(a + i * h); 
			i += 1;
			
			MPI_Send(&TrapezWynik, 1, MPI_DOUBLE, nr_akt_proc-1, tag, MPI_COMM_WORLD);
			MPI_Send(&ProstokatWynik, 1, MPI_DOUBLE, nr_akt_proc-1, tag, MPI_COMM_WORLD);
			MPI_Send(&i, 1, MPI_INT, nr_akt_proc-1, tag, MPI_COMM_WORLD);
			MPI_Send(&h, 1, MPI_DOUBLE, nr_akt_proc-1, tag, MPI_COMM_WORLD);
			MPI_Send(&a, 1, MPI_INT, nr_akt_proc-1, tag, MPI_COMM_WORLD);
			MPI_Send(&b, 1, MPI_INT, nr_akt_proc-1, tag, MPI_COMM_WORLD);
		}
	}
	
	MPI_Finalize();
	return 0;
}
