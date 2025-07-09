#!/bin/bash

echo "==> Iniciando o serviço SSH..."
sudo service ssh start

# Gera chave SSH caso não exista
if [ ! -f ~/.ssh/id_rsa ]; then
    echo "==> Gerando chave RSA..."
    mkdir -p ~/.ssh
    ssh-keygen -t rsa -f ~/.ssh/id_rsa -N ""
fi

# Copia chave para si mesmo (localhost)
echo "==> Copiando chave pública para o próprio usuário..."
sleep 2
sshpass -p 'senha123' ssh-copy-id -o StrictHostKeyChecking=no pspd@localhost

echo "==> Ambiente pronto. Use 'ssh pspd@localhost -p 2222' para testar sem senha."
exec bash
