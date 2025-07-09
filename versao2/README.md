# Projeto PSPD — Jogo da Vida com Apache Spark e Kafka

## Como Executar o Jogo da Vida com Spark (modo batch)

### 1. Clone o repositório e acesse o diretório do projeto

```bash
git clone https://github.com/ViniciussdeOliveira/PSPD_ProjetoFinal.git
cd PSPD_ProjetoFinal/versao2/spark
```

### 2. Suba o ambiente com Docker Compose

```bash
docker compose up --build
```

Aguarde até que o Spark Master e Workers estejam funcionando. Você pode verificar a interface do Spark em:

🔗 [`http://localhost:8080`](http://localhost:8080)

---

### 3. Acesse o container do Spark Master

```bash
docker exec -it spark-master bash
```

### 4. Execute o script `jogo_vida_spark.py`

```bash
cd /opt/spark-apps
spark-submit jogo_vida_spark.py 4 5
```

O script aceita dois argumentos:
- `powmin`: potência mínima (ex: 2 → tabuleiro 4x4)
- `powmax`: potência máxima (ex: 5 → tabuleiro 32x32)

## Como Executar com Kafka Streaming

### 1. Execute o script de streaming dentro do spark-master (primeiro crie os tópicos):

```bash
docker exec -it spark-master bash
cd /opt/spark-apps
spark-submit jogo_vida_kafka.py
```

Esse script ficará escutando o tópico Kafka `jogodavida`.


## Como Enviar Mensagens para o Kafka

### 1. Acesse o container Kafka:

```bash
docker exec -it kafka bash
```

### 2. Crie os tópicos (caso ainda não existam):

```bash
kafka-topics --bootstrap-server localhost:9092 --create --topic jogodavida --partitions 1 --replication-factor 1

kafka-topics --bootstrap-server localhost:9092 --create --topic resultados --partitions 1 --replication-factor 1
```

### 3. Envie mensagens com o producer:

```bash
kafka-console-producer --bootstrap-server localhost:9092 --topic jogodavida
```

Digite no terminal (pressione Enter após):

```json
{"powmin": 3, "powmax": 5}
```

⚠️ **Atenção:** As mensagens **devem estar no formato JSON válido**, como acima. Não use `"<3, 5>"` — isso causará erro.


## Como Ler os Resultados

Em outro terminal:

```bash
docker exec -it kafka bash
```

```bash
kafka-console-consumer   --bootstrap-server localhost:9092   --topic resultados   --partition 0   --offset earliest
```


## Como Adicionar Mais Workers

### Comando `--scale` (recomendado)

```bash
docker compose up --build --scale spark-worker=3
```

Isso criará `spark-worker`, `spark-worker-1` e `spark-worker-2`, todos conectados ao Master.

Verifique os workers conectados na interface:  
🔗 [`http://localhost:8080`](http://localhost:8080)

---

### Opção 2 — Editando manualmente o `docker-compose.yml`

Duplique a seção `spark-worker` e altere o nome e porta da UI:

```yaml
spark-worker-2:
  build: .
  container_name: spark-worker-2
  hostname: spark-worker-2
  depends_on:
    - spark-master
  command: /opt/bitnami/spark/bin/spark-class org.apache.spark.deploy.worker.Worker spark://spark-master:7077
  environment:
    SPARK_MODE: worker
  ports:
    - "8082:8081"
  volumes:
    - ./scripts:/opt/spark-apps
  networks:
    - sparknet
```

---

## Rebuild após alteração nos scripts

Sempre que modificar os scripts ou o Dockerfile:

```bash
docker compose down
docker compose up --build
```

---

## Testes Locais do Producer via Python

Opcionalmente, você pode usar o `produtor.py` no host ou dentro do container para enviar mensagens Kafka.

Certifique-se de que o script utilize o formato:

```python
{"powmin": 3, "powmax": 5}
```

---

## Referência de Portas

| Serviço        | Porta Local | Descrição             |
|----------------|-------------|------------------------|
| Spark Master   | `8080`      | Interface Web UI       |
| Spark Master   | `7077`      | Conexão dos Workers    |
| Spark Worker   | `8081+`     | Interface de cada Worker |
| Kafka          | `9092`      | Broker Kafka           |
| Zookeeper      | `2181`      | Coordenação Kafka      |

---

## Parar e limpar containers

```bash
docker compose down -v
```
