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
    openjdk-17-jdk \
    python3 \
    python3-pip \
    openssh-server \
    sshpass && \
    locale-gen pt_BR.UTF-8

# Configura localidade
ENV LANG=pt_BR.UTF-8
ENV LANGUAGE=pt_BR:pt
ENV LC_ALL=pt_BR.UTF-8

# Instala Jupyter Notebook
RUN pip3 install --upgrade pip && \
    pip3 install notebook jupyterlab

# Cria usuário pspd com sudo
RUN useradd -ms /bin/bash pspd && \
    echo 'pspd:senha123' | chpasswd && \
    usermod -aG sudo pspd && \
    echo 'pspd ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/pspd && \
    chmod 0440 /etc/sudoers.d/pspd

# Diretório de trabalho
WORKDIR /home/pspd

# Spark
RUN wget https://dlcdn.apache.org/spark/spark-4.0.0/spark-4.0.0-bin-hadoop3.tgz && \
    tar -xvzf spark-4.0.0-bin-hadoop3.tgz && \
    mv spark-4.0.0-bin-hadoop3 spark && \
    rm spark-4.0.0-bin-hadoop3.tgz && \
    chown -R pspd:pspd /home/pspd/spark

# Notebooks
RUN mkdir /home/pspd/notebooks && \
    chown pspd:pspd /home/pspd/notebooks

# Bashrc
RUN echo "export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64" >> /home/pspd/.bashrc && \
    echo "export PATH=\$PATH:\$JAVA_HOME" >> /home/pspd/.bashrc && \
    echo "export SPARK_HOME=~/spark" >> /home/pspd/.bashrc && \
    echo "export PATH=\$PATH:\$SPARK_HOME/bin:\$SPARK_HOME/sbin" >> /home/pspd/.bashrc && \
    echo "alias jupyter-notebook='jupyter notebook --notebook-dir=~/notebooks --ip=0.0.0.0 --port=8888 --no-browser --NotebookApp.token=\"\" --NotebookApp.password=\"\"'" >> /home/pspd/.bashrc && \
    chown pspd:pspd /home/pspd/.bashrc

# SSH config (cria pasta para o serviço)
RUN mkdir /var/run/sshd

# Copia o script de configuração do SSH
COPY setup_ssh.sh /home/pspd/setup_ssh.sh
RUN chmod +x /home/pspd/setup_ssh.sh && chown pspd:pspd /home/pspd/setup_ssh.sh

# Expor portas do Jupyter
EXPOSE 8888

# Iniciar como usuário pspd com o script
USER pspd
CMD ["/home/pspd/setup_ssh.sh"]
