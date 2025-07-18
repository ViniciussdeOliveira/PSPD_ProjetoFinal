#!/bin/bash
make -C ../jogodavida-mpi-omp-server

echo "Testando com parâmetros específicos..."
../jogodavida-mpi-omp-server/engines/jogodavida_seq 8 10 > saida_seq.txt
../jogodavida-mpi-omp-server/engines/jogodavida_omp 8 10 > saida_omp.txt
mpirun -np 2 ../jogodavida-mpi-omp-server/engines/jogodavida_mpi 8 10 > saida_mpi.txt

echo "Comparando resultados (ignorando timing)..."
grep -v "tempos:" saida_seq.txt > saida_seq_clean.txt
grep -v "tempos:" saida_omp.txt > saida_omp_clean.txt
grep -v "tempos:" saida_mpi.txt > saida_mpi_clean.txt

diff saida_seq_clean.txt saida_omp_clean.txt && echo "OpenMP OK" || echo "OpenMP FALHOU"
diff saida_seq_clean.txt saida_mpi_clean.txt && echo "MPI OK" || echo "MPI FALHOU" 