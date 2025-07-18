#!/bin/bash
ENGINE_SEQ=../jogodavida-mpi-omp-server/engines/jogodavida_seq
ENGINE_OMP=../jogodavida-mpi-omp-server/engines/jogodavida_omp
ENGINE_MPI=../jogodavida-mpi-omp-server/engines/jogodavida_mpi
SIZE=100
GEN=10

start=$(date +%s.%N)
$ENGINE_SEQ $SIZE $GEN > /dev/null
end=$(date +%s.%N)
time_seq=$(echo "$end - $start" | bc)
echo "{\"engine\":\"seq\",\"size\":$SIZE,\"gen\":$GEN,\"time\":$time_seq}" # Simula envio para Kafka

export OMP_NUM_THREADS=4
start=$(date +%s.%N)
$ENGINE_OMP $SIZE $GEN > /dev/null
end=$(date +%s.%N)
time_omp=$(echo "$end - $start" | bc)
echo "{\"engine\":\"omp\",\"size\":$SIZE,\"gen\":$GEN,\"threads\":4,\"time\":$time_omp}" # Simula envio para Kafka

start=$(date +%s.%N)
mpirun -np 2 $ENGINE_MPI $SIZE $GEN > /dev/null
end=$(date +%s.%N)
time_mpi=$(echo "$end - $start" | bc)
echo "{\"engine\":\"mpi\",\"size\":$SIZE,\"gen\":$GEN,\"procs\":2,\"time\":$time_mpi}" # Simula envio para Kafka 