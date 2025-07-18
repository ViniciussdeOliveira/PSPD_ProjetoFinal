#!/bin/bash
make -C ../jogodavida-mpi-omp-server
../jogodavida-mpi-omp-server/engines/jogodavida_seq > saida_seq.txt
../jogodavida-mpi-omp-server/engines/jogodavida_omp > saida_omp.txt
mpirun -np 4 ../jogodavida-mpi-omp-server/engines/jogodavida_mpi > saida_mpi.txt
diff saida_seq.txt saida_omp.txt && echo "OpenMP OK" || echo "OpenMP FALHOU"
diff saida_seq.txt saida_mpi.txt && echo "MPI OK" || echo "MPI FALHOU" 