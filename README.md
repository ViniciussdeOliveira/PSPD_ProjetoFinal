# **Trabalho final PSPD - Jogo da Vida**

## **Integrantes do grupo**

| **Nome** | **Matrícula** |
| --- | --- |
| Carlos Gabriel Cardoso Ramos | 211042327 |
| Douglas Farias de Castro | 170140571 | 
| Lucas Rodrigues Monteiro | 180125974 | 
| Vinícius de Oliveira | 202017263 | 
| Vinícius Roriz | 190020814 |

## **Descrição**

Este projeto é o desenvolvimento de uma aplicação de larga escala baseada no Jogo da Vida, realizado como parte da disciplina Programação Paralela e Distribuída.

O objetivo principal é explorar e aplicar conceitos de computação paralela, distribuída e elasticidade em aplicações de alta escala. Para isso, foram utilizadas as seguintes tecnologias:

* Apache Kafka para o broker de mensagens para comunicação entre componentes

* Apache Spark para a engine de processamento distribuído

* OpenMP e MPI para a programação paralela e distribuída

* Kubernetes para a orquestração de contêineres para elasticidade e escalabilidade

## **Estrutura do Projeto**

* **Versão com Apache Spark e Kafka:**
Implementação usando um cluster Spark orquestrado via Docker Compose, integrado ao broker Kafka para comunicação de mensagens.

* **Versão com OpenMP e MPI:**
Implementação paralela e distribuída usando sockets tradicionais, com código adaptado a partir do jogodavida.c fornecido pelo professor.

* **Orquestração e Elasticidade:**

Tentativa de implementação de cluster Kubernetes local (self-hosted) para orquestração e elasticidade da aplicação, porém a configuração prática não foi concluída com sucesso.

## **Como Executar**

As instruções para execução de cada versão se encontra dentro de cada uma das seguintes pastas:

* Versão com OpenMP e MPI: /jogodavida-mpi-omp-server
* Versão com Apache Spark e Kafka: /versao2
