#include <SPI.h>
#include <SD.h>
#include <DHT.h>

#define DHT_PIN 8
#define DHT_TYPE DHT11
#define SD_CS_PIN 4

DHT dht(DHT_PIN, DHT_TYPE);

File data_file;
int data_number = 1;

unsigned long previous_millis = 0;
const unsigned long interval = 1000;

void setup() {
  Serial.begin(9600);
  Serial.println("Data Logger");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("gagal_menginisialisasi_sd_card");
    return;
  }

  dht.begin();

  data_file = SD.open("datalog.txt", FILE_WRITE);

  if (data_file) {
    data_file.println("NO  |  TEMP  |  HUMI  | MISTMAKER | OUT-LAMPU");
    data_file.println("_____________________________________________");
    data_file.close();
    Serial.println("header_tersimpan");
  } else {
    Serial.println("gagal_membuka_file");
  }
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '1')
      write_data_to_file();
    else if (cmd == '2')
      read_data_from_file();
    else if (cmd == '3')
      delete_file();
  }
}

void write_data_to_file()
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("gagal_membaca_sensor_dht");
    return;
  }

  int mistmaker_value = random(10, 20);
  int lamp_value = random(0, 10);

  data_file = SD.open("datalog.txt", FILE_WRITE);

  if (data_file)
  {
    if (data_number > 100)
    {
      data_file.println("---------------------------------------------");
      data_file.println();
      data_file.println("NO  |  TEMP  |  HUMI  | MISTMAKER | OUT-LAMPU");
      data_file.println("_____________________________________________");
      data_number = 1;
    }
    data_file.print(format_number(data_number, 3));
    data_file.print(" | ");
    data_file.print(temperature);
    data_file.print("  | ");
    data_file.print(humidity);
    data_file.print("  | ");
    data_file.print(mistmaker_value);
    data_file.print("        | ");
    data_file.println(lamp_value);
    data_file.close();
    Serial.println("data_tersimpan");
    data_number++;
  }
  else
    Serial.println("gagal_membuka_file");
}


void read_data_from_file()
{
  data_file = SD.open("datalog.txt", FILE_READ);
  if (data_file)
  {
    Serial.println();
    while (data_file.available())
    {
      Serial.write(data_file.read());
    }

    data_file.close();
    Serial.println("\n membaca_data_selesai");
  }
  else
    Serial.println("gagal_membuka_file_untuk_dibaca");
}
void delete_file()
{
  if (SD.remove("datalog.txt"))
    Serial.println("file_terhapus");
  else
    Serial.println("gagal_menghapus_file");
}

String format_number(int number, int width)
{
  String result = String(number);
  while (result.length() < width) result = "0" + result;
  return result;
}
