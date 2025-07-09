# Guia de Instalação e Uso do Docker com Hadoop (Single Node)

## 1. Clone o repositório

```bash
git clone https://github.com/ViniciussdeOliveira/PSPD_ProjetoFinal.git
```

Em seguida, navegue até o diretório do Hadoop:

```bash
cd PSPD_ProjetoFinal/versao1/hadoop
```

## 2. Inicie o contêiner Docker

Dentro do diretório `Hadoop`, execute o script `iniciar_hadoop.sh`.
Esse script realiza o build da imagem e inicia o contêiner com os volumes mapeados para persistência dos dados.

Use o comando abaixo, substituindo `NomeContainer` pelo nome desejado para o contêiner e `path/para/pasta/local` pelo caminho da pasta onde os arquivos internos do contêiner serão salvos:

```bash
./iniciar_hadoop NomeContainer path/para/pasta/local
```

**Exemplo:**

```bash
./iniciar_hadoop Hadoop ~/hadoop_files
```

## 3. Formate o NameNode

Após a execução do script, o terminal será redirecionado para o contêiner. Antes de iniciar o Hadoop, é necessário formatar o NameNode:

```bash
hdfs namenode -format
```

> Alternativamente, você pode usar:

```bash
hadoop namenode -format
```

## 4. Inicie os serviços do Hadoop

Com o NameNode formatado, inicie o HDFS:

```bash
start-dfs.sh
```

## 5. Crie as pastas necessárias no HDFS

Com os serviços em execução, crie o diretório de usuário no HDFS:

```bash
hdfs dfs -mkdir -p /user/pspd
```

## 6. Acesse o monitoramento via navegador

Abra o seguinte endereço no seu navegador para visualizar o painel do Hadoop:

> [http://localhost:9870](http://localhost:9870)

---

**Observação:**
O `Dockerfile` e os scripts fornecidos estão configurados para rodar o Hadoop em modo **Single Node**.
