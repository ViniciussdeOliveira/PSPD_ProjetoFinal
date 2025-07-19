#!/bin/bash
make -C ../jogodavida-mpi-omp-server
echo "OpenMP:"
for threads in 1 2 4 8; do
    export OMP_NUM_THREADS=$threads
    echo "Threads: $threads"
    time ../jogodavida-mpi-omp-server/engines/jogodavida_omp > /dev/null
done
echo "MPI:"
for procs in 1 2 4 8; do
    echo "Processos: $procs"
    time mpirun -np $procs ../jogodavida-mpi-omp-server/engines/jogodavida_mpi > /dev/null
done 