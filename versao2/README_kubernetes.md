# Guia de Implantação do Cluster Kubernetes com kubeadm

Este guia descreve como preparar e orquestrar o ambiente do projeto usando Kubernetes em modo self-hosted, atendendo aos requisitos de elasticidade e alta disponibilidade.

## 1. Preparação dos Nós (Master e Workers)

Execute em **todos** os nós (master e workers):

### a) Desative o swap
```bash
sudo swapoff -a
sudo sed -i '/ swap / s/^/#/' /etc/fstab
```

### b) Carregue módulos do kernel necessários
```bash
sudo modprobe overlay
sudo modprobe br_netfilter
```

### c) Configure parâmetros sysctl
```bash
sudo tee /etc/sysctl.d/kubernetes.conf<<EOF
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
net.ipv4.ip_forward = 1
EOF
sudo sysctl --system
```

### d) Instale o containerd
```bash
sudo apt-get update && sudo apt-get install -y containerd
sudo mkdir -p /etc/containerd
containerd config default | sudo tee /etc/containerd/config.toml
sudo systemctl restart containerd
sudo systemctl enable containerd
```

### e) Instale kubeadm, kubelet e kubectl
```bash
sudo apt-get update && sudo apt-get install -y apt-transport-https ca-certificates curl
sudo curl -fsSLo /usr/share/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg

echo "deb [signed-by=/usr/share/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list

sudo apt-get update
sudo apt-get install -y kubelet kubeadm kubectl
sudo apt-mark hold kubelet kubeadm kubectl
```

---

## 2. Inicialização do Nó Mestre

No nó master, execute:

```bash
sudo kubeadm init --pod-network-cidr=10.244.0.0/16 --apiserver-advertise-address=<IP_DO_MASTER>
```

Após a conclusão, configure o kubectl:
```bash
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

Anote o comando `kubeadm join ...` exibido ao final, pois será usado nos workers.

---

## 3. Junção dos Workers ao Cluster

Em cada worker, execute o comando `kubeadm join ...` fornecido pelo master.

---

## 4. Instalação da Rede de Pods (CNI - Flannel)

No master:
```bash
kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml
```

---

## 5. Deploy da Aplicação (Exemplo Spark Master)

Crie um arquivo `spark-master-deployment.yaml` com o seguinte conteúdo:

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spark-master
spec:
  replicas: 1
  selector:
    matchLabels:
      app: spark-master
  template:
    metadata:
      labels:
        app: spark-master
    spec:
      containers:
      - name: spark-master
        image: bitnami/spark:latest
        env:
        - name: SPARK_MODE
          value: master
        ports:
        - containerPort: 7077
        - containerPort: 8080
```

Faça o deploy:
```bash
kubectl apply -f spark-master-deployment.yaml
```

Crie um Service para expor a interface web:

```yaml
apiVersion: v1
kind: Service
metadata:
  name: spark-master-service
spec:
  type: NodePort
  selector:
    app: spark-master
  ports:
    - port: 8080
      targetPort: 8080
      nodePort: 30080
    - port: 7077
      targetPort: 7077
      nodePort: 30077
```

Deploy do service:
```bash
kubectl apply -f spark-master-service.yaml
```

---

## 6. Escalando Workers

Crie um deployment para os workers (exemplo `spark-worker-deployment.yaml`):

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spark-worker
spec:
  replicas: 2
  selector:
    matchLabels:
      app: spark-worker
  template:
    metadata:
      labels:
        app: spark-worker
    spec:
      containers:
      - name: spark-worker
        image: bitnami/spark:latest
        env:
        - name: SPARK_MODE
          value: worker
        - name: SPARK_MASTER_URL
          value: spark://spark-master-service:7077
        ports:
        - containerPort: 8081
```

Deploy dos workers:
```bash
kubectl apply -f spark-worker-deployment.yaml
```

Para escalar:
```bash
kubectl scale deployment spark-worker --replicas=5
```

---

## 7. Observações Finais
- Adapte os manifests conforme a necessidade do seu ambiente.
- Crie deployments e services semelhantes para Kafka e Zookeeper.
- Consulte a documentação oficial do Kubernetes e do Spark para configurações avançadas. 