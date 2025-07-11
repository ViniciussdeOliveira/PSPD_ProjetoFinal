#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#define POWMIN 3
#define POWMAX 10
#define ind2d(i,j) ((i)*(tam+2)+(j))

double wall_time(void) {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

typedef struct {
  int *tabulIn, *tabulOut;
  int local_rows, tam;
} TabuleiroLocal;

void UmaVida(int *tabulIn, int *tabulOut, int tam, int local_rows) {
  int i, j, vizviv;
  for (i = 1; i <= local_rows; i++) {
    for (j = 1; j <= tam; j++) {
      vizviv = tabulIn[ind2d(i - 1, j - 1)] + tabulIn[ind2d(i - 1, j)] +
               tabulIn[ind2d(i - 1, j + 1)] + tabulIn[ind2d(i, j - 1)] +
               tabulIn[ind2d(i, j + 1)] + tabulIn[ind2d(i + 1, j - 1)] +
               tabulIn[ind2d(i + 1, j)] + tabulIn[ind2d(i + 1, j + 1)];
      if (tabulIn[ind2d(i, j)] && vizviv < 2)
        tabulOut[ind2d(i, j)] = 0;
      else if (tabulIn[ind2d(i, j)] && vizviv > 3)
        tabulOut[ind2d(i, j)] = 0;
      else if (!tabulIn[ind2d(i, j)] && vizviv == 3)
        tabulOut[ind2d(i, j)] = 1;
      else
        tabulOut[ind2d(i, j)] = tabulIn[ind2d(i, j)];
    }
  }
}

void InitTabul(int *tabulIn, int *tabulOut, int tam) {
  int ij;
  for (ij = 0; ij < (tam + 2) * (tam + 2); ij++) {
    tabulIn[ij] = 0;
    tabulOut[ij] = 0;
  }
  tabulIn[ind2d(1, 2)] = 1;
  tabulIn[ind2d(2, 3)] = 1;
  tabulIn[ind2d(3, 1)] = 1;
  tabulIn[ind2d(3, 2)] = 1;
  tabulIn[ind2d(3, 3)] = 1;
}

int Correto(int *tabul, int tam) {
  int ij, cnt;
  cnt = 0;
  for (ij = 0; ij < (tam + 2) * (tam + 2); ij++)
    cnt = cnt + tabul[ij];
  return (cnt == 5 && tabul[ind2d(tam - 2, tam - 1)] &&
          tabul[ind2d(tam - 1, tam)] && tabul[ind2d(tam, tam - 2)] &&
          tabul[ind2d(tam, tam - 1)] && tabul[ind2d(tam, tam)]);
}

void troca_bordas(int *tabul, int tam, int local_rows, int rank, int nprocs, MPI_Comm comm) {
  MPI_Status status;
  if (rank > 0) {
    MPI_Sendrecv(&tabul[ind2d(1, 0)], tam + 2, MPI_INT, rank - 1, 0,
                 &tabul[ind2d(0, 0)], tam + 2, MPI_INT, rank - 1, 1,
                 comm, &status);
  }
  if (rank < nprocs - 1) {
    MPI_Sendrecv(&tabul[ind2d(local_rows, 0)], tam + 2, MPI_INT, rank + 1, 1,
                 &tabul[ind2d(local_rows + 1, 0)], tam + 2, MPI_INT, rank + 1, 0,
                 comm, &status);
  }
}

int main(int argc, char **argv) {
  int pow, i, tam, *tabulIn = NULL, *tabulOut = NULL;
  int rank, nprocs, local_rows, resto, start_row, end_row;
  double t0, t1, t2, t3;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (argc == 3) {
    tam = atoi(argv[1]);
    int generations = atoi(argv[2]);
    
    local_rows = tam / nprocs;
    resto = tam % nprocs;
    if (rank < resto) {
      local_rows++;
      start_row = rank * local_rows + 1;
    } else {
      start_row = rank * local_rows + resto + 1;
    }
    end_row = start_row + local_rows - 1;

    int *localIn = (int *)malloc((local_rows + 2) * (tam + 2) * sizeof(int));
    int *localOut = (int *)malloc((local_rows + 2) * (tam + 2) * sizeof(int));
    for (i = 0; i < (local_rows + 2) * (tam + 2); i++) {
      localIn[i] = 0;
      localOut[i] = 0;
    }

    if (rank == 0) {
      t0 = wall_time();
      tabulIn = (int *)malloc((tam + 2) * (tam + 2) * sizeof(int));
      tabulOut = (int *)malloc((tam + 2) * (tam + 2) * sizeof(int));
      InitTabul(tabulIn, tabulOut, tam);
    }
    for (int p = 0; p < nprocs; p++) {
      int p_rows = tam / nprocs + (p < resto ? 1 : 0);
      int p_start = (p < resto) ? p * p_rows + 1 : p * p_rows + resto + 1;
      if (rank == 0) {
        if (p == 0) {
          for (i = 0; i < p_rows + 2; i++)
            for (int j = 0; j < tam + 2; j++)
              localIn[ind2d(i, j)] = tabulIn[ind2d(i + p_start - 1, j)];
        } else {
          for (i = 0; i < p_rows + 2; i++)
            MPI_Send(&tabulIn[ind2d(i + p_start - 1, 0)], tam + 2, MPI_INT, p, 10, MPI_COMM_WORLD);
        }
      } else if (p == rank) {
        for (i = 0; i < p_rows + 2; i++)
          MPI_Recv(&localIn[ind2d(i, 0)], tam + 2, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    if (rank == 0) t1 = wall_time();
    MPI_Barrier(MPI_COMM_WORLD);
    for (i = 0; i < generations; i++) {
      troca_bordas(localIn, tam, local_rows, rank, nprocs, MPI_COMM_WORLD);
      UmaVida(localIn, localOut, tam, local_rows);
      troca_bordas(localOut, tam, local_rows, rank, nprocs, MPI_COMM_WORLD);
      UmaVida(localOut, localIn, tam, local_rows);
    }
    if (rank == 0) t2 = wall_time();
    if (rank == 0) {
      for (i = 0; i < local_rows; i++)
        for (int j = 0; j < tam + 2; j++)
          tabulIn[ind2d(i + 1, j)] = localIn[ind2d(i + 1, j)];
      for (int p = 1; p < nprocs; p++) {
        int p_rows = tam / nprocs + (p < resto ? 1 : 0);
        int p_start = (p < resto) ? p * p_rows + 1 : p * p_rows + resto + 1;
        for (i = 0; i < p_rows; i++)
          MPI_Recv(&tabulIn[ind2d(p_start + i, 0)], tam + 2, MPI_INT, p, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    } else {
      for (i = 1; i <= local_rows; i++)
        MPI_Send(&localIn[ind2d(i, 0)], tam + 2, MPI_INT, 0, 20, MPI_COMM_WORLD);
    }
    if (rank == 0) {
      if (Correto(tabulIn, tam))
        printf("**RESULTADO CORRETO**\n");
      else
        printf("**RESULTADO ERRADO**\n");
      t3 = wall_time();
      printf("tam=%d; tempos: init=%7.7f, comp=%7.7f, fim=%7.7f, tot=%7.7f \n",
             tam, t1 - t0, t2 - t1, t3 - t2, t3 - t0);
      free(tabulIn);
      free(tabulOut);
    }
    free(localIn);
    free(localOut);
    MPI_Finalize();
    return 0;
  }

  for (pow = POWMIN; pow <= POWMAX; pow++) {
    tam = 1 << pow;
    local_rows = tam / nprocs;
    resto = tam % nprocs;
    if (rank < resto) {
      local_rows++;
      start_row = rank * local_rows + 1;
    } else {
      start_row = rank * local_rows + resto + 1;
    }
    end_row = start_row + local_rows - 1;

    int *localIn = (int *)malloc((local_rows + 2) * (tam + 2) * sizeof(int));
    int *localOut = (int *)malloc((local_rows + 2) * (tam + 2) * sizeof(int));
    for (i = 0; i < (local_rows + 2) * (tam + 2); i++) {
      localIn[i] = 0;
      localOut[i] = 0;
    }

    if (rank == 0) {
      t0 = wall_time();
      tabulIn = (int *)malloc((tam + 2) * (tam + 2) * sizeof(int));
      tabulOut = (int *)malloc((tam + 2) * (tam + 2) * sizeof(int));
      InitTabul(tabulIn, tabulOut, tam);
    }
    for (int p = 0; p < nprocs; p++) {
      int p_rows = tam / nprocs + (p < resto ? 1 : 0);
      int p_start = (p < resto) ? p * p_rows + 1 : p * p_rows + resto + 1;
      if (rank == 0) {
        if (p == 0) {
          for (i = 0; i < p_rows + 2; i++)
            for (int j = 0; j < tam + 2; j++)
              localIn[ind2d(i, j)] = tabulIn[ind2d(i + p_start - 1, j)];
        } else {
          for (i = 0; i < p_rows + 2; i++)
            MPI_Send(&tabulIn[ind2d(i + p_start - 1, 0)], tam + 2, MPI_INT, p, 10, MPI_COMM_WORLD);
        }
      } else if (p == rank) {
        for (i = 0; i < p_rows + 2; i++)
          MPI_Recv(&localIn[ind2d(i, 0)], tam + 2, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    if (rank == 0) t1 = wall_time();
    MPI_Barrier(MPI_COMM_WORLD);
    for (i = 0; i < 2 * (tam - 3); i++) {
      troca_bordas(localIn, tam, local_rows, rank, nprocs, MPI_COMM_WORLD);
      UmaVida(localIn, localOut, tam, local_rows);
      troca_bordas(localOut, tam, local_rows, rank, nprocs, MPI_COMM_WORLD);
      UmaVida(localOut, localIn, tam, local_rows);
    }
    if (rank == 0) t2 = wall_time();
    if (rank == 0) {
      for (i = 0; i < local_rows; i++)
        for (int j = 0; j < tam + 2; j++)
          tabulIn[ind2d(i + 1, j)] = localIn[ind2d(i + 1, j)];
      for (int p = 1; p < nprocs; p++) {
        int p_rows = tam / nprocs + (p < resto ? 1 : 0);
        int p_start = (p < resto) ? p * p_rows + 1 : p * p_rows + resto + 1;
        for (i = 0; i < p_rows; i++)
          MPI_Recv(&tabulIn[ind2d(p_start + i, 0)], tam + 2, MPI_INT, p, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    } else {
      for (i = 1; i <= local_rows; i++)
        MPI_Send(&localIn[ind2d(i, 0)], tam + 2, MPI_INT, 0, 20, MPI_COMM_WORLD);
    }
    if (rank == 0) {
      if (Correto(tabulIn, tam))
        printf("**RESULTADO CORRETO**\n");
      else
        printf("**RESULTADO ERRADO**\n");
      t3 = wall_time();
      printf("tam=%d; tempos: init=%7.7f, comp=%7.7f, fim=%7.7f, tot=%7.7f \n",
             tam, t1 - t0, t2 - t1, t3 - t2, t3 - t0);
      free(tabulIn);
      free(tabulOut);
    }
    free(localIn);
    free(localOut);
  }
  MPI_Finalize();
  return 0;
} 