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
- **Grafana** – dashboard para visualização dos dados coletados.
- **Docker Compose** – orquestração dos serviços (MQTT, InfluxDB, Node‑RED, Grafana) em containers.
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
```
2. Inicialize os serviços com Docker Compose na pasta do repositório
```bash
docker-compose up -d
```
. Apos isso abre o docker desktop clica em container e clica em RUN

3. Carregue o firmware no ESP32‑S3 ou utilize o Simulador

    Abra Programa_ESP32_S3_PI4.ino no Arduino IDE ou VSCode PlatformIO e envie para o dispositivo.

    Rodar simulador de dados
```bash
pip install paho-mqtt
python3 simulator.py
```

5. Acesse as interfaces
  
    - MQTT: tcp://localhost:1883 (e websockets em 9001)

    - Node-RED: http://localhost:1880

    - InfluxDB 1.8: http://localhost:8086

    - Grafana: http://localhost:3000    (login admin / admin)
 

Dica: veja logs se algo não subir:

```bash
docker compose ps
docker logs mosquitto
docker logs nodered
docker logs influxdb
docker logs grafana
```

6. Importar o fluxo do Node-RED

    Abra http://localhost:1880

   Menu ≡ → Import → Upload e selecione nodered_flow.json.

    Clique Deploy.

    Esse fluxo assina o tópico lab/bench/+/telemetry, monta a linha Influx e faz POST em http://influxdb:8086/write?db=iot. (É exatamente o endpoint oficial do Influx v1.) 
    docs.influxdata.com

7. Rodar o simulador (sem hardware)

    No seu host:
```bash
cd iot-bench-env
pip install paho-mqtt
python simulator.py
```

   Ele publica a cada 2s no tópico lab/bench/bench01/telemetry (JSON com temp, hum, lux, noise_db, co2, voc, pm25, pm10).
   
   Volte ao Node-RED e veja no painel Debug a resposta do Influx (status 204 = gravou com sucesso). 
   
   Referências: imagem oficial do Mosquitto e requisitos de config; Node-RED em Docker. 
   hub.docker.com
   nodered.org

8. Configurar o Grafana

    Entre em http://localhost:3000
     (admin/admin).
    
    Em dashboard → Grafico por tempo
    Connections → Data sources → Add data source → InfluxDB:
    
    URL: http://influxdb:8086
    
    Query Language: InfluxQL
    
    Database: iot
    
    Sem auth (esta stack está sem usuário/senha no Influx para simplificar o MVP).
    
    Save & test (deve ficar verde).
