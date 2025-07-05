# Guia de Instalação e Uso do Docker com Spark (Single e Multi Node)

## 1. Clone o repositório

```bash
git clone https://github.com/douglasffcastro/PSPD_ProjetoFinal.git
```

Em seguida, navegue até o diretório do Spark:

```bash
cd PSPD_ProjetoFinal/Spark
```

---

## 2. Inicie o contêiner Docker

Dentro do diretório `Spark`, execute o script `iniciar_spark.sh`.
Esse script realiza o *build* da imagem e inicia o contêiner com os volumes mapeados, garantindo persistência dos dados.

Use o comando abaixo, substituindo os parâmetros conforme necessário:

```bash
./iniciar_ubuntu.sh NomeContainer path/para/pasta/local portaMaster portaJupyter portaWebUI
```

**Exemplo:**

```bash
./iniciar_ubuntu.sh spark1 ~/spark1 2313 8888 8080
```

---

## 3. Execução em *Single Node*

Se seu objetivo é executar o Spark em modo **Single Node**, siga os passos abaixo assim que o terminal do contêiner estiver disponível:

### 3.1 Inicie o Master

```bash
start-master.sh -p 2313 -h 0.0.0.0 --webui-port 8080
```

Esse comando inicia o Master na porta `2313` e a interface web na porta `8080`.
Para verificar se o Spark foi iniciado corretamente, acesse:

> [http://localhost:8080](http://localhost:8080)

### 3.2 Inicie o Worker

```bash
start-worker.sh spark://0.0.0.0:2313
```

Você encontrará a URL do master (`spark://...`) no topo da página web do Spark.
Após executar esse comando, atualize a página se o worker não aparecer imediatamente.

---

### 3.3 Utilizando o Spark

Você pode utilizar o Spark de duas formas: via terminal ou Jupyter Notebook.

#### Pelo terminal:

```bash
pyspark --master spark://0.0.0.0:2313
```

Esse comando inicia uma sessão interativa do PySpark.

#### Pelo Jupyter Notebook:

```bash
jupyter notebook --ip=0.0.0.0 --port=8888 --no-browser
```

Copie e cole o link gerado no terminal (semelhante a abaixo) no seu navegador:

```bash
http://127.0.0.1:8888/tree?token=...
```

---

## 4. Execução em *Multi Node*

Se deseja executar o Spark em modo **Multi Node**, siga os passos abaixo.

### 4.1 Crie a rede Docker

Antes de iniciar os contêineres:

```bash
docker network create rede-pspd
```

### 4.2 Inicie dois ou mais contêineres

Exemplo de segundo contêiner:

```bash
./iniciar_ubuntu.sh spark2 ~/spark2 2314 8889 8081
```

---

### 4.3 Configure a comunicação entre os nós

#### 1. Descubra o IP de cada contêiner:

```bash
hostname -I
```

#### 2. Edite o arquivo `/etc/hosts` de cada contêiner para adicionar o IP dos outros:

```bash
sudo nano /etc/hosts
```

Adicione no final:

```
IPConteiner   hostnameConteiner
```

***exemplo***

```
172.17.0.3      spark2 (esses valores do exemplo podem não ser os mesmos)
```

> Substitua o IP e o nome conforme necessário. Use `hostname` para descobrir o nome do contêiner.

> **Importante:** Repita isso em todos os contêineres participantes.

---

### 4.4 Inicie o Master no contêiner principal

```bash
start-master.sh -h spark1 -p 2313
```

### 4.5 Inicie os Workers nos demais contêineres

```bash
start-worker.sh spark://spark1:2313
```

Acesse novamente:

> [http://localhost:8080](http://localhost:8080)

E verifique se os Workers estão registrados.

---

## 5. Integração com Hadoop (opcional)

Para que o Spark funcione corretamente com um sistema de arquivos compartilhado, é necessário utilizar uma base comum. Neste projeto, utilizamos o Hadoop HDFS.

Siga o guia de instalação do Hadoop (na pasta `Hadoop`) e:

* Suba o contêiner do Hadoop.
* Crie um arquivo de teste e envie-o ao HDFS.
* Use o Jupyter Notebook (disponível na pasta Spark) para ler esse arquivo e testar o Spark em multi node.

---

## 6. Automatizando o start do Cluster

Para automatizar o processo, configure o acesso SSH entre os contêineres.

### 6.1 Configure o acesso SSH

No contêiner Master, execute:

```bash
ssh-copy-id pspd@sparkX
```

> Substitua `X` pelo número do Spark (ex: `spark2`).
> Senha padrão: `senha123`.
> Faça esse passo nos workers também.

### 6.2 Configure os Workers

No contêiner Master, edite o arquivo de configuração:

```bash
nano ~/spark/conf/workers
```

Adicione:

```
pspd@spark1
pspd@spark2
```

### 6.3 Configure as variáveis de ambiente

```bash
nano ~/spark/conf/spark-env.sh
```

Adicione:

```bash
SPARK_MASTER_HOST=spark1
SPARK_MASTER_PORT=2313
```

### 6.4 Inicie tudo automaticamente

```bash
start-all.sh
```

---

## 7. Finalização

Para encerrar os serviços do Spark:

### Se iniciou manualmente:

```bash
stop-worker.sh
stop-master.sh
```

### Se iniciou com `start-all.sh`:

```bash
stop-all.sh
```
