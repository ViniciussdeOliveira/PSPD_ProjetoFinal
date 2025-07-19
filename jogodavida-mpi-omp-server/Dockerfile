FROM ubuntu:20.04

# Instala dependências
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y gcc g++ make openmpi-bin libopenmpi-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copia todo o código-fonte
COPY . /app

# Compila todos os componentes
RUN make all

# Expõe a porta do socket_server
EXPOSE 8080

# Comando padrão: inicia o servidor
CMD ["./server/socket_server"] 