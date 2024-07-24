#include <DHT.h>
#include <WiFi.h>

String apiKey = "";        // sesuaikan dengan Key kalian dari thingspeak.com
const char *ssid = "";     // sesuaikan dengan ssid wifi kalian
const char *password = ""; // sesuaikan dengan wifi password kalian
const char *server = "api.thingspeak.com";

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup()
{
    Serial.begin(115200);
    delay(10);
    dht.begin();

    WiFi.begin(ssid, password);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
}

void loop()
{

    float kelembapan = dht.readHumidity();
    float temperatur = dht.readTemperature();
    if (isnan(kelembapan) || isnan(temperatur))
    {
        Serial.println("Gagal Membaca Sensor DHT");
        return;
    }

    if (client.connect(server, 80))
    {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(temperatur);
        postStr += "&field2=";
        postStr += String(kelembapan);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        Serial.print("Temperatur: ");
        Serial.print(temperatur);
        Serial.println(" *C");
        Serial.print(" Kelembapan: ");
        Serial.print(kelembapan);
        Serial.println(" %");
        Serial.println(" Mengirim Data Thingspeak");
    }
    client.stop();

    Serial.println("Menunggu 1 Detik untuk kirim ke thingspeak.com");
    delay(1000);
}