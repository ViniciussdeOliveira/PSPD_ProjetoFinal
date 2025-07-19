#!/bin/bash
cppcheck --enable=all --inconclusive --std=c99 ../jogodavida-mpi-omp-server/jogodavida.c ../jogodavida-mpi-omp-server/jogodavida-omp.c ../jogodavida-mpi-omp-server/jogodavida-mpi.c 2> analise_cppcheck.txt
grep -E 'wall_time|UmaVida|DumpTabul|InitTabul|Correto|main' ../jogodavida-mpi-omp-server/jogodavida.c > funcoes_encontradas.txt
grep -E 'wall_time|UmaVida|DumpTabul|InitTabul|Correto|main' ../jogodavida-mpi-omp-server/jogodavida-omp.c >> funcoes_encontradas.txt
grep -E 'wall_time|UmaVida|DumpTabul|InitTabul|Correto|main' ../jogodavida-mpi-omp-server/jogodavida-mpi.c >> funcoes_encontradas.txt
echo "Análise estática concluída. Veja analise_cppcheck.txt e funcoes_encontradas.txt." 