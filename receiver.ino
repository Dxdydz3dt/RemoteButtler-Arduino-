
 // Using Arduino Nano Board and Esp8266 wireless board The receiver side code

#include <SoftwareSerial.h>
#include <NanoESP.h>

#define WLAN_SSID "nanoesp"
#define WLAN_PWD "hammad10"

#define SENDER_IP_ADRESS "192.168.137.50"
#define SENDER_PORT "80"

#define LOCAL_PORT 80

#define command_1 6
#define command_2 5
#define command_3 4
#define ack_button 3
#define buzzer 7

NanoESP esp8266 = NanoESP();

int id; 
int len;

bool positiveFlankenerkennung(int taste);

uint8_t command = 0;

int zustand = 1;
int ereignis = 0; // 0 entspricht kein ereignis

void zustandsAuswahl();
void zustand1();
void zustand2();
void ereignisseRegistrieren();
void ereignis1();
void ereignis2();

void setup() 
{
  pinMode(command_1, OUTPUT);
  pinMode(command_2, OUTPUT);
  pinMode(command_3, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ack_button, INPUT);
  
  Serial.begin(19200);
  
  esp8266.init();
  esp8266.configWifi(STATION, WLAN_SSID, WLAN_PWD);

  String local_ip = esp8266.getIp();
  Serial.println(local_ip);

  esp8266.startUdpServer(0, SENDER_IP_ADRESS, LOCAL_PORT, LOCAL_PORT, 0);
}

void loop() 
{
  ereignisseRegistrieren();
  zustandsAuswahl();  
}

bool positiveFlankenerkennung(int taste)
{
  static bool tasteAlt[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  static bool tasteNeu[14];
  tasteNeu[taste] = digitalRead(taste);
  //delay(50); //debounce
  if (tasteNeu[taste] == 0 && tasteAlt[taste] == 1)
  {
    tasteAlt[taste] = 0;
    return 1;
  }
  if (tasteNeu[taste] == 1)
  {
    tasteAlt[taste] = 1;
  }
  return 0;
}

void zustandsAuswahl()
{
  switch (ereignis)
  {
  case 0:
    break;
  case 1:
    switch (zustand)
    {
    case 1:
      zustand2();
      break;
    case 2:
      zustand2();
      break;
    }
    ereignis = 0;
    break;
  case 2:
    switch (zustand)
    {
    case 1:
      zustand1();
      break;
    case 2:
      zustand1();
      break;
    }
    ereignis = 0;
    break;
  }
}

void zustand1()
{
  zustand = 1;
  digitalWrite(command, LOW);
  //buzzer without delay
}

void zustand2()
{
  zustand = 2;
  digitalWrite(command, HIGH);
  esp8266.sendData(0, "ACK");
  Serial.println("SEND ACK");
}

void ereignisseRegistrieren()
{
  ereignis1();
  ereignis2();
}

void ereignis1()
{
  bool rec = esp8266.recvData(id, len);
  if(rec)
  {
    char buffer[10];
    Serial.print("REC?: ");
    Serial.println(rec);
    esp8266.readBytes(buffer, 10);
    for(int i = 0; i < 10 ; i++)
    {
      Serial.println(buffer[i], HEX);
    }
    command = buffer[1];
    Serial.println("");

    ereignis = 1;
    Serial.print("Zustand: ");
    Serial.print(zustand);
    Serial.print(" Ereignis");
    Serial.println(ereignis);
    switch(command)
    {
      case 0x11: command = command_1;
                 ereignis = 1;
                 break;
      case 0x22: command = command_2;
                 ereignis = 1;
                 break;
      case 0x33: command = command_3;
                 ereignis = 1;
                 break;
    }
    Serial.print("REC command: ");
    Serial.println(command, HEX);
  }
}

void ereignis2()
{
  if(digitalRead(ack_button))
  {
    ereignis = 2;
    Serial.print("Zustand: ");
    Serial.print(zustand);
    Serial.print(" Ereignis");
    Serial.println(ereignis);
  }
}
