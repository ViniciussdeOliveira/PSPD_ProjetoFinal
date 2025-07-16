from kafka import KafkaConsumer
from elasticsearch import Elasticsearch
import subprocess
import json
import time
from datetime import datetime

# Kafka Consumer
consumer = KafkaConsumer(
    'jogodavida',
    bootstrap_servers='kafka:9092',
    value_deserializer=lambda m: json.loads(m.decode('utf-8'))
)

# Elasticsearch para métricas
es = Elasticsearch(["http://elasticsearch:9200"])

# Contadores e acumuladores
total_reqs_spark = 0
total_reqs_mpi = 0
soma_tempo_spark = 0.0
soma_tempo_mpi = 0.0

# Estratégia simples: alternar entre engines
usar_spark = True

for message in consumer:
    dados = message.value

    powmin = dados['powmin']
    powmax = dados['powmax']
    client_id = dados.get('client_id', 'desconhecido')

    inicio = time.time()

    if usar_spark:
        print(f"[Middleware] Enviando requisição para Spark (client: {client_id})")
        subprocess.run(['spark-submit', 'spark/scripts/jogo_vida_spark.py', str(powmin), str(powmax)])
        engine_usado = 'spark'
    else:
        print(f"[Middleware] Enviando requisição para MPI (client: {client_id})")
        subprocess.run(['mpirun', '-np', '4', './jogo_vida_mpi', str(powmin), str(powmax)])
        engine_usado = 'mpi'

    usar_spark = not usar_spark  # Alterna

    duracao = round(time.time() - inicio, 4)

    # Atualiza métricas
    if engine_usado == 'spark':
        total_reqs_spark += 1
        soma_tempo_spark += duracao
    else:
        total_reqs_mpi += 1
        soma_tempo_mpi += duracao

    # Calcula totais
    total_requisicoes = total_reqs_spark + total_reqs_mpi
    soma_tempo_total = soma_tempo_spark + soma_tempo_mpi

    tempo_medio_spark = round(soma_tempo_spark / total_reqs_spark, 4) if total_reqs_spark > 0 else 0.0
    tempo_medio_mpi = round(soma_tempo_mpi / total_reqs_mpi, 4) if total_reqs_mpi > 0 else 0.0
    tempo_medio_total = round(soma_tempo_total / total_requisicoes, 4) if total_requisicoes > 0 else 0.0


    # Envia métricas
    es.index(index="job-metrics", document={
        "client_id": client_id,
        "engine": engine_usado,
        "powmin": powmin,
        "powmax": powmax,
        "duracao_processamento": duracao,
        "timestamp": datetime.now().isoformat()
    })

    es.index(index="global-metrics", document={
        "timestamp": datetime.now().isoformat(),
        "total_reqs_spark": total_reqs_spark,
        "total_reqs_mpi": total_reqs_mpi,
        "total_requisicoes": total_requisicoes,
        "tempo_medio_spark": tempo_medio_spark,
        "tempo_medio_mpi": tempo_medio_mpi,
        "tempo_medio_total": tempo_medio_total
    })

    print(f"[{engine_usado.upper()}] {client_id} concluído em {duracao}s")
    print(f"Qtd Requisições - Spark: {total_reqs_spark},\tMPI: {total_reqs_mpi},\tTotal: {total_requisicoes}")
    print(f"Tempo médio     - Spark: {tempo_medio_spark}s,\tMPI: {tempo_medio_mpi}s,\tGeral: {tempo_medio_total}s\n")
