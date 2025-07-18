#!/bin/bash
ENGINE_HYB=../jogodavida-mpi-omp-server/engines/jogodavida_mpi_omp
SIZE=100
GEN=10
for procs in 1 2 4; do
  for threads in 1 2 4; do
    export OMP_NUM_THREADS=$threads
    echo "Processos: $procs, Threads: $threads"
    time mpirun -np $procs $ENGINE_HYB $SIZE $GEN > /dev/null
  done
done 