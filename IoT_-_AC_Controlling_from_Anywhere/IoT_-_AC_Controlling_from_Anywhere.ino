#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <FirebaseESP8266.h>
#include <LiquidCrystal_I2C.h>

//Inisiasi Pin Arduino WeMos D1
int sensor = A0;
int IR = D11;

//Data IR
int dataSekarang = 0;
int dataPowerSekarang = 0;
int dataSebelum = 25;
int dataIr = 0;
int dataPower = 0;

//WiFi
char ssid [] = "AccessPoint_TA";
char password[] = "12345668";

//LCD
LiquidCrystal_I2C lcd(0x3F,20,4);
uint8_t heart [8] = {0x0,0xa,0x1f,0xe,0x4,0x0};

//Firebase dan IR
int status = WL_IDLE_STATUS;
IRsend irsend(IR) ;
FirebaseData firebaseData;
WiFiServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  irsend.begin();
  pinMode (IR, OUTPUT);
  lcd.createChar(1,heart);
  lcd.backlight();
  lcdbegin();
  Wifi();
  Firebase.begin("https://remotac-3c7bc.firebaseio.com/", "HCLDMRFrwkBbPKaVYJyyLuVZKbRPq6QGjPOM1o0J");
}

void loop() {
  // put your main code here, to run repeatedly:
  Suhu();
}

void lcdbegin(){
  lcd.clear();
  lcd.init();
  lcd.setCursor(3,0);
  lcd.write(byte(1));
  lcd.print("Tugas Akhir");
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.print("Dipersembahkan Oleh:");
  lcd.setCursor(1,2);
  lcd.print("M. Ramadikal Kahfi");
  lcd.setCursor(1,3);
  lcd.print("Bagaskara Farhan J");
  delay(5000);
  lcd.clear();
}

void Suhu(){
  SuhuRuangan();
  Firebase.getInt(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Tombol");
  if (firebaseData.intData() != 0){
    RemoteAC();
  }
}

void Wifi(){
  WiFi.begin(ssid, password);
  Serial.println("Koneksi ke jaringan dilakukan . . .");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.write(byte(1));
  lcd.print("Koneksi ke jaringan");
  lcd.setCursor(0,2);
  lcd.print("Sedang Dilakukan . .");
  Serial.print("SSID: ");
  Serial.println(ssid);
  while (WiFi.status() !=WL_CONNECTED) {
    delay (500);
    Serial.print(". ");
    lcd.setCursor(0,3);
    lcd.print(". ");
  }
  lcd.clear();
  server.begin();
  IPAddress ip = WiFi.localIP();
  Serial.print ("\nAlamat IP: ");
  Serial.println (ip);
  Serial.print ("Siap Melayani Anda \n");
  lcd.setCursor(5,1);
  lcd.print("Alamat IP:");
  lcd.setCursor(4,2);
  lcd.print(ip);
  delay(5000);
  lcd.clear();
}

void RemoteAC(){
  if (Firebase.getInt(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Tombol")){
     //Power On
     if (firebaseData.intData() == 1){
      dataPowerSekarang = 1 ;
      powerAC();
     }
     //Power Off
     else if (firebaseData.intData() == 2){
      dataPowerSekarang = 2 ;
      powerAC();
     }
     //Menambah Suhu
     else if (firebaseData.intData() == 3){
        dataSekarang = dataSebelum + 1 ;
        suhuAC();
     }
     //Mengurangi Suhu
     else if (firebaseData.intData() == 4){
        dataSekarang = dataSebelum - 1 ;
        suhuAC();
     }
  }
} 

void perintah(){
  Serial.println("Silahkan Masukkan Perintah \n");
  digitalWrite (IR, LOW);
  delay(100);
  Suhu();
}

void SuhuRuangan(){
  int suhu2 = analogRead(sensor) / 2.0479;
  int suhu = suhu2 - 15.5;
  Serial.print("Suhu: ");
  Serial.println(suhu);
  lcd.setCursor(0,1);
  lcd.print("Suhu Ruangan: ");
  lcd.setCursor(14,1);
  lcd.print(suhu);
  lcd.setCursor(17,1);
  lcd.print((char)223);
  lcd.setCursor(0,2);
  lcd.print("Suhu Remote : ");
  lcd.setCursor(14,2);
  lcd.print(dataSekarang);
  lcd.setCursor(17,2);
  lcd.print((char)223);
  Firebase.setInt(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Suhu", suhu);
  Firebase.setInt(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/SuhuRemote", dataSekarang);
  delay(500);
}

void sendIrSuhu(){
  irsend.sendNEC(dataIr, 28);
  digitalWrite (IR, HIGH);
  Serial.print("\nCode IR: ");
  Serial.println(dataIr,16);
  Serial.print("Suhu Remote: ");
  Serial.print(dataSekarang);
  Serial.print("° \n");
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("Kode IR Dikirim:");
  lcd.setCursor(6,2);
  lcd.print(dataIr, 16);
  delay (200);
  dataSebelum = dataSekarang;
  Firebase.setInt(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Tombol", 0);
  lcd.clear();
  perintah();
}

void suhuAC(){
  if (dataSekarang == 30){
  //Suhu 30
  dataIr = 0x8808F4B;
  sendIrSuhu();
  }
  if (dataSekarang == 29){ 
  //Suhu 29
  dataIr = 0x8808E4A;
  sendIrSuhu();
  }
  else if (dataSekarang == 28){
  //Suhu 28
  dataIr = 0x8808D49;
  sendIrSuhu();
  }
  if (dataSekarang == 27){
  //Suhu 27
  dataIr = 0x8808C48;
  sendIrSuhu();
  }
  else if (dataSekarang == 26){
  //Suhu 26
  dataIr = 0x8808B47;
  sendIrSuhu();
  }
  if (dataSekarang == 25){
  //Suhu 25
  dataIr = 0x8808A46;
  sendIrSuhu();
  }
  else if (dataSekarang == 24){
  //Suhu 24
  dataIr = 0x8808945;
  sendIrSuhu();
  }
  if (dataSekarang == 23){
  //Suhu 23
  dataIr = 0x8808844;
  sendIrSuhu();
  }
  else if (dataSekarang == 22){
  //Suhu 22
  dataIr = 0x8808743;
  sendIrSuhu();
  }
  if (dataSekarang == 21){
  //Suhu 21
  dataIr = 0x8808642;
  sendIrSuhu();
  }
  if (dataSekarang == 20){
  //Suhu 20
  dataIr = 0x8808541;
  sendIrSuhu();
  }
  if (dataSekarang == 19){
  //Suhu 19
  dataIr = 0x8808440;
  sendIrSuhu();
  }
  if (dataSekarang == 18){
  //Suhu 18
  dataIr = 0x880834F;
  sendIrSuhu();
  }
  else if (dataSekarang == 17){
  //Suhu 17
  dataIr = 0x880834F;
  sendIrSuhu();
  }
  if (dataSekarang == 16){
  //Suhu 16
  dataIr = 0x880834F;
  sendIrSuhu();
  }
}

void sendPower(){
  irsend.sendLG(dataPower, 28);
  digitalWrite (IR, HIGH);
  Serial.print("Code IR: ");
  Serial.println(dataPower, 16);
  Serial.print("SuhuRemote: ");
  Serial.print(dataSekarang);
  Serial.print("° \n");
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("Kode IR Dikirim:");
  lcd.setCursor(6,2);
  lcd.print(dataIr, 16);
  dataSebelum = dataSekarang;
  dataPowerSekarang = 0;
  Firebase.setInt(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Tombol", 0);
  delay(200);
  lcd.clear();
  perintah();
}

void powerAC(){
  //AC ON
  if (dataPowerSekarang == 1){
    dataPower = 0x8800A4E;
    Serial.print("\nKet: ");
    Serial.println("AC On 25°");
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("Keterangan :");
    lcd.setCursor(5,2);
    lcd.print("AC On 25");
    lcd.setCursor(14,2);
    lcd.print((char)223);
    Firebase.setString(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Kondisi", "ON");
    dataSekarang = 25;
    delay(200);
    sendPower();
  }
  //AC OFF
  if (dataPowerSekarang == 2){
    dataPower = 0x88C0051;
    Serial.print("\nKet: ");
    Serial.println("AC Off");
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("Keterangan :");
    lcd.setCursor(7,2);
    lcd.print("AC Off");
    Firebase.setString(firebaseData, "/data/-M7xH5YMAuUss3nyK0Fy/Kondisi", "OFF");
    dataSekarang = 25;
    delay(200);
    sendPower();
  }
}
