#!/bin/bash
python3 ../versao2/spark/scripts/produtor.py input.txt
python3 ../versao2/kafka_middleware.py 