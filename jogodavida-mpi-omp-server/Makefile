CC = gcc
MPICC = mpicc
CFLAGS = -O3 -Wall
OMPFLAGS = -fopenmp

ENGINES_DIR = engines
SERVER_DIR = server
CLIENT_DIR = client

all: engines server client

engines: $(ENGINES_DIR)/jogodavida_seq $(ENGINES_DIR)/jogodavida_omp $(ENGINES_DIR)/jogodavida_mpi

$(ENGINES_DIR)/jogodavida_seq: jogodavida.c
	@mkdir -p $(ENGINES_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(ENGINES_DIR)/jogodavida_omp: jogodavida-omp.c
	@mkdir -p $(ENGINES_DIR)
	$(CC) $(CFLAGS) $(OMPFLAGS) -o $@ $<

$(ENGINES_DIR)/jogodavida_mpi: jogodavida-mpi.c
	@mkdir -p $(ENGINES_DIR)
	$(MPICC) $(CFLAGS) -o $@ $<

server: $(SERVER_DIR)/socket_server

$(SERVER_DIR)/socket_server: socket_server.c
	@mkdir -p $(SERVER_DIR)
	$(CC) $(CFLAGS) -o $@ $<

client: $(CLIENT_DIR)/test_client

$(CLIENT_DIR)/test_client: test_client.c
	@mkdir -p $(CLIENT_DIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(ENGINES_DIR) $(SERVER_DIR) $(CLIENT_DIR)

test: all
	@echo "Starting server..."
	@$(SERVER_DIR)/socket_server &
	@sleep 2
	@echo "Testing client..."
	@$(CLIENT_DIR)/test_client 0 256 10
	@$(CLIENT_DIR)/test_client 1 512 5
	@$(CLIENT_DIR)/test_client 2 1024 3
	@pkill -f socket_server

.PHONY: all engines server client clean test 