# IOT – PI4

Controle de ambiente de bancada eletrônica com ESP32‑S3, InfluxDB, Node‑RED, MQTT e dashboard.

## Sobre
Este projeto, "IOT – PI4", é um sistema completo para monitoramento e controle de ambiente de bancada eletrônica.  
Ele utiliza o ESP32‑S3 para coleta de dados, MQTT para comunicação em tempo real, InfluxDB para armazenamento de séries temporais e Node‑RED para visualização e automação.

## Tecnologias
- **ESP32‑S3** – microcontrolador responsável por ler sensores e enviar dados via MQTT.
- **MQTT / Mosquitto** – protocolo leve de mensagens para comunicação entre dispositivos.
- **InfluxDB** – banco de dados de séries temporais para armazenamento das leituras dos sensores.
- **Node‑RED** – plataforma de automação e visualização de dados, com fluxos configuráveis.
- **Docker Compose** – orquestração dos serviços (MQTT, InfluxDB, Node‑RED) em containers.
- **Python (simulator.py)** – script opcional para simular dados sensoriais.

## Estrutura do Repositório
- `Programa_ESP32_S3_PI4.ino` – código para o ESP32‑S3.
- `mosquitto.conf` – configuração do broker MQTT.
- `influxdb-init.iql` – script para inicialização do banco InfluxDB.
- `docker-compose.yml` – orquestração dos containers Docker.
- `nodered_flow.json` – fluxo de automação e visualização no Node‑RED.
- `simulator.py` – simula dados sensoriais (opcional).

## Instalação e Execução

1. **Clone o repositório**
```bash
git clone https://github.com/brunohss/IOT---PI4.git
cd IOT---PI4
