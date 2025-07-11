# Jogo da Vida - Sistema Distribuído com OpenMP/MPI

Sistema completo do Jogo da Vida com múltiplos engines de computação (sequencial, OpenMP, MPI) e servidor socket para processamento distribuído.

## Objetivo

Desenvolver uma versão do engine do "Jogo da Vida" utilizando as bibliotecas OpenMP e MPI para resolver o requisito de performance, integrado a um Socket server para receber demandas dos clientes.

## Arquitetura Implementada

### Engines de Computação
- **Sequencial** (`jogodavida.c`): Melhor para problemas pequenos (< 512)
- **OpenMP** (`jogodavida-omp.c`): Paralelização com threads, ideal para problemas médios (512-1024)
- **MPI** (`jogodavida-mpi.c`): Processos distribuídos, otimizado para problemas grandes (> 1024)

### Servidor Socket
- **Protocolo binário eficiente**: 9 bytes request, 5 bytes response
- **Seleção automática de engine** baseada no tamanho do problema
- **Suporte a múltiplos clientes** simultâneos
- **Timeout e tratamento de erros**

### Cliente de Teste
- Interface simples para testar o servidor
- Suporte a todos os tipos de engine
- Validação de resultados

## Estrutura do Projeto

```
jogo_da_vida/
├── engines/           # Executáveis dos engines
│   ├── jogodavida_seq
│   ├── jogodavida_omp
│   └── jogodavida_mpi
├── server/           # Servidor socket
│   └── socket_server
├── client/           # Cliente de teste
│   └── test_client
├── jogodavida.c      # Engine sequencial
├── jogodavida-omp.c  # Engine OpenMP
├── jogodavida-mpi.c  # Engine MPI
├── socket_server.c   # Servidor socket
├── test_client.c     # Cliente de teste
├── Makefile          # Compilação
├── stress_test.sh    # Teste de stress
├── testa_igualdade.sh # Teste de equivalência
└── README.md         # Esta documentação
```

## Compilação

```bash
# Compilar todos os componentes
make all

# Compilar apenas engines
make engines

# Compilar apenas servidor
make server

# Compilar apenas cliente
make client

# Limpar executáveis
make clean
```

## Como Executar

### 1. Execução Direta dos Engines

```bash
# Sequencial
./engines/jogodavida_seq <tamanho> <gerações>

# OpenMP
./engines/jogodavida_omp <tamanho> <gerações>

# MPI (2 processos para 2 núcleos)
mpirun -np 2 ./engines/jogodavida_mpi <tamanho> <gerações>
```

**Exemplos:**
```bash
./engines/jogodavida_seq 256 10
./engines/jogodavida_omp 512 5
mpirun -np 2 ./engines/jogodavida_mpi 1024 3
```

### 2. Servidor Socket

```bash
# Terminal 1: Iniciar servidor
./server/socket_server

# Terminal 2: Testar cliente
./client/test_client <engine_type> <tamanho> <gerações>
```

**Engine Types:**
- `0`: Auto-seleção (baseada no tamanho)
- `1`: Sequencial
- `2`: OpenMP
- `3`: MPI

**Exemplos:**
```bash
./client/test_client 0 256 10    # Auto-seleção
./client/test_client 1 512 5     # Forçar sequencial
./client/test_client 2 1024 3    # Forçar OpenMP
./client/test_client 3 2048 2    # Forçar MPI
```

### 3. Testes Automatizados

```bash
# Teste de equivalência entre engines
./testa_igualdade.sh

# Teste de stress com múltiplos clientes
./stress_test.sh
```

## Seleção Automática de Engine

O servidor escolhe automaticamente o melhor engine baseado no tamanho:

| Tamanho | Engine | Justificativa |
|---------|--------|---------------|
| **< 512** | Sequencial | Menos overhead para problemas pequenos |
| **512-1024** | OpenMP | Paralelização eficiente com threads |
| **> 1024** | MPI | Processos distribuídos para problemas grandes |

## Configuração para Diferentes Hardwares

### Sistema com 2 Núcleos (Configuração Atual)
- MPI usa 2 processos: `mpirun -np 2`
- Estratégia otimizada para 2 cores

### Sistema com 4+ Núcleos
Edite `socket_server.c` linha 35:
```c
int nprocs = (tam > 512) ? 4 : 2;  // Para 4+ núcleos
```

### Sistema com 1 Núcleo
Edite `socket_server.c` linha 35:
```c
int nprocs = 1;  // Para 1 núcleo
```

## Protocolo de Comunicação

### Request (9 bytes)
```c
typedef struct {
    unsigned char engine_type;  // 0=auto, 1=seq, 2=omp, 3=mpi
    unsigned int tam;           // Tamanho do tabuleiro
    unsigned int generations;   // Número de gerações
} Request;
```

### Response (5 bytes)
```c
typedef struct {
    unsigned char result;       // 1=correto, 0=errado
    float time;                // Tempo de execução em segundos
} Response;
```

## Testes e Validação

### 1. Teste de Equivalência
```bash
./testa_igualdade.sh
```
Verifica se todos os engines produzem o mesmo resultado.

### 2. Teste de Performance
```bash
# Comparar engines no mesmo problema
./client/test_client 1 1024 20  # Sequencial
./client/test_client 2 1024 20  # OpenMP
./client/test_client 3 1024 20  # MPI
```

### 3. Teste de Stress
```bash
./stress_test.sh
```
Executa múltiplos clientes simultaneamente para testar concorrência.

### 4. Teste de Concorrência Manual
```bash
# Em terminais separados
./client/test_client 0 512 15 &
./client/test_client 0 1024 10 &
./client/test_client 0 2048 5 &
```

## Troubleshooting

### Erro: "not enough slots available"
- **Causa**: MPI tentando usar mais processos que núcleos disponíveis
- **Solução**: Ajuste `nprocs` em `socket_server.c` para o número de núcleos

### Erro: "Connection Failed"
- **Causa**: Servidor não está rodando
- **Solução**: Inicie o servidor primeiro: `./server/socket_server`

### Resultado "ERRADO" com tempo 0.000s
- **Causa**: Problema muito grande ou falha na execução
- **Solução**: Reduza o tamanho ou número de gerações

### Warnings de compilação
- **Causa**: Avisos menores de formatação
- **Solução**: Não afetam funcionamento, podem ser ignorados

## Performance Esperada

### Tamanhos de Problema
- **Pequeno (128-512)**: < 1 segundo
- **Médio (512-1024)**: 1-10 segundos
- **Grande (1024+)**: 10+ segundos

### Comparação de Engines
- **Sequencial**: Melhor para problemas pequenos
- **OpenMP**: Melhor para problemas médios (2 threads)
- **MPI**: Melhor para problemas grandes (2 processos)

## Fluxo de Desenvolvimento

1. **Compilar**: `make all`
2. **Testar equivalência**: `./testa_igualdade.sh`
3. **Iniciar servidor**: `./server/socket_server`
4. **Testar cliente**: `./client/test_client 0 512 10`
5. **Teste de stress**: `./stress_test.sh`

## Requisitos

- **Sistema**: Linux/Unix
- **Compilador**: GCC com suporte a OpenMP
- **MPI**: OpenMPI ou MPICH
- **Hardware**: Mínimo 2 núcleos recomendado

## Casos de Uso

### Desenvolvimento
- Teste de algoritmos paralelos
- Comparação de performance
- Validação de resultados

### Produção
- Servidor de processamento distribuído
- Balanceamento de carga automático
- Escalabilidade horizontal

### Educação
- Aprendizado de programação paralela
- Comparação OpenMP vs MPI
- Arquitetura cliente-servidor 