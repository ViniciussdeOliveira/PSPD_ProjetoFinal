from kafka import KafkaProducer
import json
import time

producer = KafkaProducer(
    bootstrap_servers='kafka:9092',
    value_serializer=lambda v: json.dumps(v).encode('utf-8')
)

mensagens = [
    {"powmin": 4, "powmax": 5}
]

for m in mensagens:
    producer.send('jogodavida', m)
    print("Enviado:", m)
    time.sleep(1)

producer.flush()
