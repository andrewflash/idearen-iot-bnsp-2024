#include <WiFi.h>
#include <PubSubClient.h>

// Ganti dengan SSID dan Password Wi-Fi Anda
const char *ssid = "Your_WiFi_SSID";
const char *password = "Your_WiFi_Password";

// Ganti dengan informasi broker MQTT Anda
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_user = "your_mqtt_user";
const char *mqtt_password = "your_mqtt_password";
const char *mqtt_topic = "water_level/data";

// Pin untuk HC-SR04
const int trigPin = 5;
const int echoPin = 18;

// Pin untuk LED
const int ledGreenPin = 12;
const int ledRedPin = 14;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);

    // Setup pin HC-SR04
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // Setup pin LED
    pinMode(ledGreenPin, OUTPUT);
    pinMode(ledRedPin, OUTPUT);

    // Koneksi Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Setup MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    // Membaca data dari sensor HC-SR04
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // Menyalakan LED Hijau jika ketinggian air normal
    if (distance > 0 && distance < 100)
    {
        digitalWrite(ledGreenPin, HIGH);
        digitalWrite(ledRedPin, LOW);
    }
    else
    {
        digitalWrite(ledGreenPin, LOW);
        digitalWrite(ledRedPin, HIGH);
    }

    // Kirim data ke MQTT
    String payload = String(distance);
    if (client.publish(mqtt_topic, payload.c_str()))
    {
        Serial.print("Published: ");
        Serial.println(payload);
    }
    else
    {
        Serial.println("Failed to publish data");
    }

    delay(2000); // Delay 2 detik sebelum pengukuran berikutnya
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password))
        {
            Serial.println("connected");
            client.subscribe(mqtt_topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // Fungsi callback untuk menangani pesan yang diterima dari broker MQTT
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}