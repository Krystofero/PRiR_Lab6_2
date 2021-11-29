Projekt przedstawia zadanie 2 z Labolatorium nr 6 PRiR. Program napisano w języku C. Równolegle oblicza i wyświetla na ekranie przybliżenie całki y=x^2 w przedziale <a,b>, używając dwóch metod(trapezów i prostokątów) oraz wykorzystując n procesow. Procesy przekazują między sobą aktualną wartość przybliżenia całki za pomocą obu metod , na koniec wypisuje ostateczne wyniki na ekranie.

Import bibliotek ( w tym biblioteki MPI umożliwiającej pisanie programów wykonywanych równolegle):

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include "mpi.h
   
Definicja funkcji y=x^2 :

    double funkcja(double x)
    {
      return x*x;
    }
 
W mainie najpierw inicjalizujemy potrzebne zmienne: 
    
    int l_procesow, nr_akt_proc, a, b, i, tag;
    double TrapezWynik;
    double ProstokatWynik;
    double h;

    tag = 20;
    a = 2;
    b = 8;

    MPI_Init(&argc, &argv); //inicjalizacja środowiska MPI
    MPI_Comm_size(MPI_COMM_WORLD, &l_procesow); // okresla liczbę uruchomonych procesow tworzących sieć
    MPI_Comm_rank(MPI_COMM_WORLD, &nr_akt_proc); // okresla numer aktualnego procesu
    MPI_Status status;
    
Następnie sprawdzamy czy nasz akualny proces jest ostatni wtedy przypisujemy wartosci zmiennych i je wysyłamy do procesu o jeden mniejszego:    
    
    if(nr_akt_proc == l_procesow-1) {
      TrapezWynik = 0;
      ProstokatWynik = 0;
      h = (double)(b - a) / l_procesow; //przedział <a,b> podzielony na n równych części (h = odleglość między xi a xi+1)
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
   
Następnie sprawdzamy czy aktualny proces znajduje się w przedziale liczby procesów i czy istnieją jakieś procesy. Jeśli tak to odbieramy przypisane zmienne i wykonujemy odpowiednie obliczenia, aż natrafimy na pierwszy proces ( 0 ) - wtedy wypisujemy nasze przybliżenia na ekran:
   
    if((nr_akt_proc < l_procesow-1) && (nr_akt_proc >= 0)) {// sprawdzam czy aktualny proces znajduje się w przedziale liczby procesów i czy istnieją jakieś procesy
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
        printf("Wynik Metody prostokątów = %f", ProstokatWynik);
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
   
   
Na koniec funkcja, która kończy pracę w trybie MPI:

    MPI_Finalize(); //funkcja kończy pracę w trybie MPI
    
