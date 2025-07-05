FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Instala pacotes essenciais + Java + Python + pip + SSH
RUN apt update && apt upgrade -y && \
    apt install -y \
    sudo \
    curl \
    wget \
    vim \
    nano \
    git \
    net-tools \
    iputils-ping \
    unzip \
    build-essential \
    software-properties-common \
    locales \
    openjdk-8-jdk \
    python3 \
    python3-pip \
    openssh-server \
    sshpass && \
    locale-gen pt_BR.UTF-8

# Configura localidade
ENV LANG=pt_BR.UTF-8
ENV LANGUAGE=pt_BR:pt
ENV LC_ALL=pt_BR.UTF-8

# Cria usuário pspd com sudo
RUN useradd -ms /bin/bash pspd && \
    echo 'pspd:senha123' | chpasswd && \
    usermod -aG sudo pspd && \
    echo 'pspd ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/pspd && \
    chmod 0440 /etc/sudoers.d/pspd

# Diretório de trabalho
WORKDIR /home/pspd

# Instalar Hadoop
RUN cd /opt && \
    wget https://dlcdn.apache.org/hadoop/common/hadoop-3.4.1/hadoop-3.4.1.tar.gz && \
    tar -xvzf hadoop-3.4.1.tar.gz && \
    mv hadoop-3.4.1 hadoop && \
    rm hadoop-3.4.1.tar.gz && \
    mkdir -p /opt/hadoop/logs /opt/hadoop/tmp && \
    chown -R pspd:pspd /opt/hadoop

# Configura hadoop-env.sh e core-site.xml corretamente
RUN echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/jre" >> /opt/hadoop/etc/hadoop/hadoop-env.sh && \
    cat <<EOF > /opt/hadoop/etc/hadoop/core-site.xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <property>
        <name>fs.defaultFS</name>
        <value>hdfs://0.0.0.0:9000</value>
    </property>
</configuration>
EOF

# Bashrc
RUN echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/jre" >> /home/pspd/.bashrc && \
    echo "export PATH=\$PATH:\$JAVA_HOME" >> /home/pspd/.bashrc && \
    echo "export HADOOP_HOME=/opt/hadoop" >> /home/pspd/.bashrc && \
    echo "export PATH=\$PATH:\$HADOOP_HOME/bin:\$HADOOP_HOME/sbin" >> /home/pspd/.bashrc && \
    chown pspd:pspd /home/pspd/.bashrc

# SSH config (cria pasta para o serviço)
RUN mkdir /var/run/sshd

# Copia o script de configuração do SSH
COPY setupssh.sh /home/pspd/setup_ssh.sh
RUN chmod +x /home/pspd/setup_ssh.sh && chown pspd:pspd /home/pspd/setup_ssh.sh

WORKDIR /home/pspd
# Expor portas
EXPOSE 9870 9000

# Iniciar como usuário pspd com o script
USER pspd
CMD ["/home/pspd/setup_ssh.sh"]

