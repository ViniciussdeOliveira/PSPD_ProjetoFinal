#!/bin/bash
set -e

# 1. Build da imagem Docker
echo "[1/6] Buildando a imagem Docker do socket_server..."
docker build -t jogodavida-mpi-omp-server:latest .

# 2. Carregar imagem no Minikube/Kind se necessário
echo "[2/6] (Opcional) Carregando imagem no Minikube/Kind..."
if command -v minikube &> /dev/null; then
  minikube image load jogodavida-mpi-omp-server:latest || true
fi
if command -v kind &> /dev/null; then
  kind load docker-image jogodavida-mpi-omp-server:latest || true
fi

# 3. Deploy dos componentes
echo "[3/6] Aplicando YAMLs do socket_server, ElasticSearch e Kibana..."
kubectl apply -f k8s-deployment.yaml
kubectl apply -f k8s-service.yaml
kubectl apply -f k8s-elasticsearch.yaml
kubectl apply -f k8s-kibana.yaml

# 4. Espera os pods ficarem prontos
echo "[4/6] Aguardando pods ficarem prontos..."
kubectl wait --for=condition=Ready pod -l app=jogodavida-mpi-omp-server --timeout=120s || true
kubectl wait --for=condition=Ready pod -l app=elasticsearch --timeout=180s || true
kubectl wait --for=condition=Ready pod -l app=kibana --timeout=180s || true

# 5. Mostra status dos pods e serviços
echo "[5/6] Status dos pods e serviços:"
kubectl get pods -o wide
kubectl get svc

# 6. Instruções finais
echo "[6/6] Teste manual:"
echo "- Descubra o IP do node com: minikube ip  (ou veja o IP do seu node)"
echo "- Teste o socket_server: ./client/test_client 0 256 10 (apontando para <IP_DO_NODE>:30090)"
echo "- Teste o ElasticSearch: curl http://<IP_DO_NODE>:30920"
echo "- Teste o Kibana: http://<IP_DO_NODE>:31561"
echo "- Para escalar: kubectl scale deployment jogodavida-mpi-omp-server --replicas=3"
echo "- Para limpar: kubectl delete -f k8s-deployment.yaml; kubectl delete -f k8s-service.yaml; kubectl delete -f k8s-elasticsearch.yaml; kubectl delete -f k8s-kibana.yaml" 