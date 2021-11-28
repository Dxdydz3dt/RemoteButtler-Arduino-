#include <SoftwareSerial.h>
#include <NanoESP.h>

#define WLAN_SSID "nanoesp"
#define WLAN_PWD "hammad10"

#define RECV_IP_ADRESS "192.168.137.166"
#define RECV_PORT "80"

#define LOCAL_PORT 80

#define command_1 8
#define command_2 7
#define command_3 6
#define ack_led 5

NanoESP esp8266 = NanoESP();

int id; 
int len;

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
  pinMode(command_1, INPUT);
  pinMode(command_2, INPUT);
  pinMode(command_3, INPUT);
  pinMode(ack_led, OUTPUT);
  
  Serial.begin(19200);
  
  esp8266.init();
  esp8266.configWifi(STATION, WLAN_SSID, WLAN_PWD);

  String local_ip = esp8266.getIp();
  Serial.println(local_ip);

  esp8266.startUdpServer(0, RECV_IP_ADRESS, LOCAL_PORT, LOCAL_PORT, 0);

  digitalWrite(ack_led, HIGH);
}

void loop() 
{
  ereignisseRegistrieren();
  zustandsAuswahl();  
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
  digitalWrite(ack_led, HIGH);
}

void zustand2()
{
  zustand = 2;
  digitalWrite(ack_led, LOW);

  if(command == command_1)
  {
    char test[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    esp8266.sendRaw(0, test, 5);
  }
  else if(command == command_2)
  {
    char test1[5] = {0x22, 0x22, 0x33, 0x44, 0x55};
    esp8266.sendRaw(0, test1, 5);
  }
  else if(command == command_3)
  {
    char test2[5] = {0x33, 0x22, 0x33, 0x44, 0x55};
    esp8266.sendRaw(0, test2, 5);
  }
}

void ereignisseRegistrieren()
{
  ereignis1();
  ereignis2();
}

void ereignis1()
{
  if(digitalRead(command_1) && !(digitalRead(command_2) || digitalRead(command_3)))
  {
    ereignis = 1;
    command = command_1;
  }
  else if(digitalRead(command_2) && !(digitalRead(command_1) || digitalRead(command_3)))
  {
    ereignis = 1;
    command = command_2;
  }
  else if(digitalRead(command_3) && !(digitalRead(command_2) || digitalRead(command_1)))
  {
    ereignis = 1;
    command = command_3;
  }
}

void ereignis2()
{
  if(esp8266.recvData(id, len))
  {
    String message = esp8266.readStringUntil('\r');

    if(message.compareTo("ACK") < 0)
    {
      ereignis = 2;
    }
  }
}
