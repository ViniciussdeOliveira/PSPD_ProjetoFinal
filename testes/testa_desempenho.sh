#!/bin/bash
make -C ../jogodavida-mpi-omp-server
echo "Sequencial:"
time ../jogodavida-mpi-omp-server/engines/jogodavida_seq > /dev/null
echo "OpenMP:"
time ../jogodavida-mpi-omp-server/engines/jogodavida_omp > /dev/null
echo "MPI (4 processos):"
time mpirun -np 4 ../jogodavida-mpi-omp-server/engines/jogodavida_mpi > /dev/null 