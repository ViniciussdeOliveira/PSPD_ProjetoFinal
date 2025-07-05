#!/bin/bash

# Nome da imagem base
NOME_IMAGEM="ubuntu-pessoal"

# Nome do container e volume persistente (podem ser passados como argumentos)
NOME_CONTAINER="${1:-ubuntu-dev}"
PASTA_LOCAL="${2:-$HOME/$NOME_CONTAINER}"
PASTA_CONTAINER="/home/pspd/workspace"

# Arquivo que guarda o hash da ultima versao do Dockerfile
HASH_FILE=".dockerfile.hash"

# Verifica se o Docker esta instalado
if ! command -v docker &> /dev/null; then
    echo "Docker nao esta instalado. Instale o Docker primeiro:"
    echo "https://docs.docker.com/get-docker/"
    exit 1
fi

# Verifica se o Dockerfile existe
if [ ! -f Dockerfile ]; then
    echo "Dockerfile nao encontrado no diretorio atual!"
    exit 1
fi

# Gera hash atual do Dockerfile
HASH_ATUAL=$(sha256sum Dockerfile | cut -d ' ' -f1)

# Builda ou rebuilda imagem se necessario
if [ ! -f "$HASH_FILE" ]; then
    echo "Nenhum hash encontrado. Buildando imagem '$NOME_IMAGEM' pela primeira vez..."
    docker build -t "$NOME_IMAGEM" .
    echo "$HASH_ATUAL" > "$HASH_FILE"
elif [ "$HASH_ATUAL" != "$(cat $HASH_FILE)" ]; then
    echo "Dockerfile modificado. Rebuildando imagem '$NOME_IMAGEM'..."
    docker build -t "$NOME_IMAGEM" .
    echo "$HASH_ATUAL" > "$HASH_FILE"
else
    echo "Dockerfile nao mudou. Usando imagem '$NOME_IMAGEM'."
fi

# Cria pasta local de volume persistente
mkdir -p "$PASTA_LOCAL"

# Verifica se o container ja existe
if docker ps -a --format '{{.Names}}' | grep -q "^${NOME_CONTAINER}$"; then
    echo "Container '$NOME_CONTAINER' ja existe. Iniciando..."
    docker start -ai "$NOME_CONTAINER"
else
    echo "Criando novo container '$NOME_CONTAINER' com volume montado em '$PASTA_CONTAINER'..."
    docker run -it \
        --name "$NOME_CONTAINER" \
        --hostname "$NOME_CONTAINER" \
	--network rede-pspd \
        -p 9000:9000 \
        -p 9870:9870 \
        -v "$PASTA_LOCAL":"$PASTA_CONTAINER" \
        "$NOME_IMAGEM"
fi
