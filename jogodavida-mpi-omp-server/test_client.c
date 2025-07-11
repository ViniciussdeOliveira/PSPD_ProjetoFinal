#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

typedef struct {
    unsigned char engine_type;
    unsigned int tam;
    unsigned int generations;
} Request;

typedef struct {
    unsigned char result;
    float time;
} Response;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <engine_type> <tam> <generations>\n", argv[0]);
        printf("engine_type: 0=auto, 1=seq, 2=omp, 3=mpi\n");
        return 1;
    }
    
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }
    
    Request req;
    req.engine_type = atoi(argv[1]);
    req.tam = atoi(argv[2]);
    req.generations = atoi(argv[3]);
    
    printf("Sending request: engine=%d, tam=%d, generations=%d\n", 
           req.engine_type, req.tam, req.generations);
    
    send(sock, &req, sizeof(Request), 0);
    
    Response resp;
    read(sock, &resp, sizeof(Response));
    
    printf("Result: %s (%.3f seconds)\n", 
           resp.result ? "CORRETO" : "ERRADO", resp.time);
    
    close(sock);
    return 0;
} 