#!/usr/bin/env python3
# Simple MQTT simulator for lab bench telemetry
# Publishes JSON messages to topic: lab/bench/bench01/telemetry

import json, os, time, random, math
from datetime import datetime

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("This script requires paho-mqtt. Install with: pip install paho-mqtt")
    raise

BROKER_HOST = os.getenv("MQTT_HOST", "localhost")
BROKER_PORT = int(os.getenv("MQTT_PORT", "1883"))
TOPIC = os.getenv("MQTT_TOPIC", "lab/bench/bench01/telemetry")
INTERVAL = float(os.getenv("PUB_INTERVAL", "2"))  # seconds

def fake_measurements(step=0):
    # Generate plausible lab conditions with slight variation
    base_temp = 24.0 + 2.0*math.sin(step/50.0)
    base_hum = 50.0 + 5.0*math.sin(step/70.0 + 1.0)
    base_lux = max(200.0 + 50.0*math.sin(step/20.0), 0)
    base_noise = 48.0 + 8.0*max(math.sin(step/30.0), 0) + random.uniform(-2, 2)
    base_co2 = 600 + int(80*max(math.sin(step/60.0), 0)) + random.randint(-15, 15)
    base_voc = 120 + int(30*max(math.sin(step/90.0), 0)) + random.randint(-10, 10)
    pm25 = max(5 + int(4*max(math.sin(step/45.0), 0)) + random.randint(-1, 2), 0)
    pm10 = pm25 + random.randint(0, 3)

    return {
        "bench_id": "bench01",
        "temp": round(base_temp + random.uniform(-0.2, 0.2), 2),
        "hum": round(base_hum + random.uniform(-1.0, 1.0), 2),
        "lux": round(base_lux + random.uniform(-5.0, 5.0), 1),
        "noise_db": round(base_noise, 1),
        "co2": int(base_co2),
        "voc": int(base_voc),
        "pm25": int(pm25),
        "pm10": int(pm10),
        "ts": int(time.time())
    }

def main():
    client = mqtt.Client()
    client.connect(BROKER_HOST, BROKER_PORT, 60)
    step = 0
    print(f"Publishing to mqtt://{BROKER_HOST}:{BROKER_PORT}/{TOPIC} every {INTERVAL}s ...")
    try:
        while True:
            payload = fake_measurements(step)
            client.publish(TOPIC, json.dumps(payload), qos=0, retain=False)
            print(datetime.utcnow().isoformat(), payload)
            step += 1
            time.sleep(INTERVAL)
    except KeyboardInterrupt:
        pass
    finally:
        client.disconnect()

if __name__ == "__main__":
    main()
