#!/bin/bash

echo "Building project..."
make clean && make all

echo "Starting server..."
./server/socket_server &
SERVER_PID=$!
sleep 2

echo "Running stress test with multiple clients..."

echo "Test 1: Multiple small requests (tam=128)"
for i in {1..5}; do
    ./client/test_client 0 128 5 &
done
wait

echo "Test 2: Mixed engine types"
./client/test_client 1 256 10 &
./client/test_client 2 512 8 &
./client/test_client 3 1024 5 &
wait

echo "Test 3: Large problem (tam=2048)"
./client/test_client 0 2048 3

echo "Test 4: Concurrent large problems"
for i in {1..3}; do
    ./client/test_client 0 1024 5 &
done
wait

echo "Stress test completed!"
kill $SERVER_PID 