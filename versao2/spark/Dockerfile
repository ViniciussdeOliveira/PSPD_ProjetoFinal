# Base: Spark pré-instalado da Bitnami
FROM bitnami/spark:3.5

USER root

# Instala Python + pip
RUN install_packages python3 python3-pip && \
    pip3 install --upgrade pip

# Instala PySpark (mesmo já vindo com Spark, é necessário para scripts Python)
RUN pip3 install pyspark kafka-python numpy

# Adiciona conector Kafka para Structured Streaming e dependências necessárias
RUN mkdir -p /opt/bitnami/spark/jars && \
    curl -L -o /opt/bitnami/spark/jars/spark-sql-kafka-0-10_2.12-3.5.0.jar https://repo1.maven.org/maven2/org/apache/spark/spark-sql-kafka-0-10_2.12/3.5.0/spark-sql-kafka-0-10_2.12-3.5.0.jar && \
    curl -L -o /opt/bitnami/spark/jars/spark-token-provider-kafka-0-10_2.12-3.5.0.jar https://repo1.maven.org/maven2/org/apache/spark/spark-token-provider-kafka-0-10_2.12/3.5.0/spark-token-provider-kafka-0-10_2.12-3.5.0.jar && \
    curl -L -o /opt/bitnami/spark/jars/kafka-clients-3.5.0.jar https://repo1.maven.org/maven2/org/apache/kafka/kafka-clients/3.5.0/kafka-clients-3.5.0.jar && \
    curl -L -o /opt/bitnami/spark/jars/commons-pool2-2.11.1.jar https://repo1.maven.org/maven2/org/apache/commons/commons-pool2/2.11.1/commons-pool2-2.11.1.jar

# Define diretório de trabalho
WORKDIR /opt/spark-apps

CMD [ "/bin/bash" ]
