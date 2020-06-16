#include <SPI.h>
#include <Ethernet.h>
const int relay1 = 6;
const int relay2 = 7;
const int relay3 = 8;
const int relay4 = 9;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
String request;
unsigned long StartTime;
EthernetClient client;
void connecttoangeal()
{
    Serial.println("connecting...");
  if (client.connect("YOUR_SERVER", 8008)) {
    Serial.println("connected");
  } else {
    Serial.println("connection failed");
  }
}
void setup() {
  // put your setup code here, to run once:
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay1,HIGH);
  digitalWrite(relay2,HIGH);
  digitalWrite(relay3,HIGH);
  digitalWrite(relay4,HIGH);
  StartTime=millis();
  Ethernet.begin(mac);
  Serial.begin(9600);

  delay(1000);

  connecttoangeal();
  
  Serial.write("Initialized!\n");
}
String array_to_string(byte array[], unsigned int len)
{
  String result=String("");
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        result = String(result+String((char)(nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA)));
        result = String(result+String((char)(nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA)));
        if(i!=len-1)
        result = String(result+String(":"));
    }
    return result;
}
void Restarter(int relay)
{
    digitalWrite(relay,LOW);
    delay(5000);
    digitalWrite(relay,HIGH);
}
void Restartall()
{
    digitalWrite(relay1,LOW);
    digitalWrite(relay2,LOW);
    digitalWrite(relay3,LOW);
    digitalWrite(relay4,LOW);
    delay(5000);
    digitalWrite(relay1,HIGH);
    digitalWrite(relay2,HIGH);
    digitalWrite(relay3,HIGH);
    digitalWrite(relay4,HIGH);
}
boolean Responder(EthernetClient client,String request)
{
  request.toLowerCase();
  if(request.indexOf(String("relay1\r"))>-1)
  {
    Restarter(relay1);
    client.print(String("relay1:OK\r\n"));
  }
  else if(request.indexOf(String("relay2\r"))>-1)
  {
    Restarter(relay2);
    client.print(String("relay2:OK\r\n"));
  }
  else if(request.indexOf(String("relay3\r"))>-1)
  {
    Restarter(relay3);
    client.print(String("relay3:OK\r\n"));
  }
  else if(request.indexOf(String("relay4\r"))>-1)
  {
    Restarter(relay4);
    client.print(String("relay4:OK\r\n"));
  }
  else if(request.indexOf(String("relayall\r"))>-1)
  {
    Restartall();
    client.print(String("relayall:OK\r\n"));
  }
  else if(request!=String("")&&request.indexOf('\r')>-1)
  {
    client.print(String("Unknown Command\r\n"));
  }
  if(request.indexOf('\r')>-1)
  return true;
  else
  return false;
}

void loop() {
  // put your main code here, to run repeatedly:
    if (!client.connected()) {
      delay(5000);
      connecttoangeal();
      return;
    }
    if(millis()-StartTime>5000)
    {
      client.print("keepalive\r\n");
      StartTime=millis(); 
    }
  while (client.connected() && client.available() > 0) {
      char ch=(char)client.read();
      Serial.write(ch);
      request=String(request+String(ch));
    }
    boolean result=Responder(client,request);
    if(result)
    request=String("");
  }
