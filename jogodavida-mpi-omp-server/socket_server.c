#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    unsigned char engine_type;
    unsigned int tam;
    unsigned int generations;
} Request;

typedef struct {
    unsigned char result;
    float time;
} Response;

int execute_sequential(int tam, int generations) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "./engines/jogodavida_seq %d %d", tam, generations);
    return system(cmd);
}

int execute_openmp(int tam, int generations) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "./engines/jogodavida_omp %d %d", tam, generations);
    return system(cmd);
}

int execute_mpi(int tam, int generations) {
    char cmd[256];
    int nprocs = 2;
    snprintf(cmd, sizeof(cmd), "mpirun -np %d ./engines/jogodavida_mpi %d %d", nprocs, tam, generations);
    return system(cmd);
}

unsigned char select_engine(int tam) {
    if (tam < 256) return 0;
    if (tam <= 1024) return 1;
    return 2;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        Request req;
        Response resp;
        
        if (read(new_socket, &req, sizeof(Request)) <= 0) {
            close(new_socket);
            continue;
        }
        
        clock_t start = clock();
        
        int result = 0;
        unsigned char engine = (req.engine_type == 0) ? select_engine(req.tam) : req.engine_type;
        
        switch (engine) {
            case 0:
                result = execute_sequential(req.tam, req.generations);
                break;
            case 1:
                result = execute_openmp(req.tam, req.generations);
                break;
            case 2:
                result = execute_mpi(req.tam, req.generations);
                break;
        }
        
        clock_t end = clock();
        float time_taken = ((float)(end - start)) / CLOCKS_PER_SEC;
        
        resp.result = (result == 0) ? 1 : 0;
        resp.time = time_taken;
        
        write(new_socket, &resp, sizeof(Response));
        close(new_socket);
    }
    
    return 0;
} 