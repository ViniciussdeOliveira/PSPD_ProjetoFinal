from pyspark.sql import SparkSession
from pyspark.sql.functions import from_json, col
from pyspark.sql.types import StructType, IntegerType
from elasticsearch import Elasticsearch
from kafka import KafkaProducer
from datetime import datetime
import numpy as np
import json
import time

# --- Funções do jogo da vida ---
def init_tabul(tam):
    tabulIn = np.zeros((tam + 2, tam + 2), dtype=np.int8)
    tabulOut = np.zeros((tam + 2, tam + 2), dtype=np.int8)
    tabulIn[1, 2] = 1
    tabulIn[2, 3] = 1
    tabulIn[3, 1:4] = 1
    return tabulIn, tabulOut

def correto(tabul, tam):
    expected_iter = 4 * (tam - 3)
    distance = expected_iter // 4
    i = min(1 + distance, tam - 2)
    j = min(2 + distance, tam - 2)
    return (tabul[i, j+1] and
            tabul[i+1, j+2] and
            tabul[i+2, j] and
            tabul[i+2, j+1] and
            tabul[i+2, j+2])

def processa_linha(data, tam):
    idx, linhas = data
    if idx == 0 or idx >= tam + 1:
        return np.zeros(tam + 2, dtype=np.int8)
    nova = np.zeros(tam + 2, dtype=np.int8)
    for j in range(1, tam + 1):
        vizviv = (
            linhas[0][j-1] + linhas[0][j] + linhas[0][j+1] +
            linhas[1][j-1] +              linhas[1][j+1] +
            linhas[2][j-1] + linhas[2][j] + linhas[2][j+1]
        )
        if linhas[1][j] and (vizviv < 2 or vizviv > 3):
            nova[j] = 0
        elif not linhas[1][j] and vizviv == 3:
            nova[j] = 1
        else:
            nova[j] = linhas[1][j]
    return (idx, nova)

def simula_jogo(sc, pow):
    tam = 1 << pow
    t0 = time.time()
    tabulIn, tabulOut = init_tabul(tam)
    t1 = time.time()

    iterations = 4 * (tam - 3)
    for _ in range(iterations):
        linhas_tripla = [(i, tabulIn[i-1:i+2]) for i in range(1, tam + 1)]
        rdd = sc.parallelize(linhas_tripla)
        result = rdd.map(lambda x: processa_linha(x, tam)).collect()
        for i, linha in result:
            tabulOut[i] = linha
        tabulIn, tabulOut = tabulOut, tabulIn

    t2 = time.time()
    return {
        "pow": pow,
        "tam": tam,
        "correto": bool(correto(tabulIn, tam)),
        "init": round(t1 - t0, 4),
        "comp": round(t2 - t1, 4),
        "total": round(t2 - t0, 4)
    }

# --- Início do Spark + Kafka ---

es = Elasticsearch(["http://elasticsearch:9200"])

clientes_ativos = 0
total_requisicoes = 0
tempo_processamento_total = 0.0

spark = SparkSession.builder \
    .appName("JogoDaVidaKafka") \
    .master("spark://spark-master:7077") \
    .getOrCreate()

sc = spark.sparkContext

# Inicializa o produtor Kafka para publicar resultados
producer = KafkaProducer(
    bootstrap_servers='kafka:9092',
    value_serializer=lambda v: json.dumps(v).encode('utf-8')
)

# Schema das mensagens de entrada
schema = StructType().add("powmin", IntegerType()).add("powmax", IntegerType())

# Lê mensagens do tópico "jogodavida"
df = spark.readStream.format("kafka") \
    .option("kafka.bootstrap.servers", "kafka:9092") \
    .option("subscribe", "jogodavida") \
    .option("startingOffsets", "latest") \
    .load()

dados = df.selectExpr("CAST(value AS STRING)") \
    .select(from_json(col("value"), schema).alias("data")) \
    .select("data.powmin", "data.powmax")

def processa_batch(batch_df, epoch_id):
    global clientes_ativos, total_requisicoes, tempo_processamento_total

    registros = batch_df.collect()

    clientes_ativos += len(registros)

    for row in registros:
        powmin = row['powmin']
        powmax = row['powmax']
        tempo_inicial_cliente = time.time()

        for pow in range(powmin, powmax + 1):
            resultado = simula_jogo(sc, pow)
            print(">>> Resultado:", json.dumps(resultado))
            # Envia para o tópico "resultados"
            producer.send('resultados', resultado)
            # Atualiza métricas acumuladas
            total_requisicoes += 1
            tempo_processamento_total += resultado['total']
    
    tempo_total_cliente = round(time.time() - tempo_inicial_cliente, 4)
    
    # Envia métricas desse cliente para o Elasticsearch
    es.index(index="spark-metrics", document={
        "client_id": f"cliente_epoch_{epoch_id}",
        "timestamp": datetime.now().isoformat(),
        "total_requisicoes": total_requisicoes,
        "clientes_ativos": clientes_ativos,
        "tempo_total_cliente": tempo_total_cliente,
        "tempo_medio_processamento": round(tempo_processamento_total / total_requisicoes, 4) if total_requisicoes > 0 else 0.0
    })

    clientes_ativos -= len(registros)
    producer.flush()

# Ativa o streaming com foreachBatch
dados.writeStream \
    .foreachBatch(processa_batch) \
    .start() \
    .awaitTermination()
