#!/bin/bash

echo "Compilando..."
mpicc -o jogodavida-mpi jogodavida-mpi.c
gcc -o jogodavida jogodavida.c

echo "Executando versão sequencial..."
./jogodavida > saida_seq.txt

echo "Executando versão MPI (4 processos)..."
mpirun -np 4 ./jogodavida-mpi > saida_mpi.txt

echo "Comparando saídas..."
grep 'RESULTADO' saida_seq.txt > saida_seq_result.txt
grep 'RESULTADO' saida_mpi.txt > saida_mpi_result.txt

diff saida_seq_result.txt saida_mpi_result.txt > /dev/null
if [ $? -eq 0 ]; then
  echo "Os resultados lógicos são idênticos!"
else
  echo "Os resultados lógicos são diferentes!"
  diff saida_seq_result.txt saida_mpi_result.txt
fi 
