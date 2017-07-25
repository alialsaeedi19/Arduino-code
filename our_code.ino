#include <SoftwareSerial.h>
SoftwareSerial SIM808(6, 7);
int Powerkey = 8;
int ct = 1;

typedef struct {
  String latitude;
  String longitude;
  String datetime;
} MyDataGPS;
MyDataGPS dataGPS;

void setup()
{
  SIM808.begin(9600);
  Serial.begin(9600);
  delay(100);

  //pinMode(Powerkey, OUTPUT);   // initialize the digital pin as an output.
  //power();
  //power on the sim808 or power down the sim808
  SIM808.println("AT+CLIP=1"); //We activate Caller ID
  delay(300);
  SIM808.println("AT+DDET=1"); //set the reception of the DTMF **
  delay(300);
   gprs_init();

  delay(1000);
   dataGPS = getGPSLocation();
    gprs_send(dataGPS.latitude, dataGPS.longitude);
  //  //try_send_HTTP();
  //   SIM808.println("AT+CGNSPWR=0"); //Turn on GNSS(Global Navigation Satellite System) power
  //  delay(1000);
  //  Serial.println(ReadGSM());
}

void loop()
{

  //Envíamos y recibimos datos
  if (Serial.available() > 0)
  {
    SIM808.println(Serial.readString());
  }
  if (SIM808.available() > 0)
  {
    Serial.println( ReadGSM());
  }
}


void power(void)
{
  digitalWrite(Powerkey, LOW);
  delay(1000);               // wait for 1 second
  //digitalWrite(Powerkey, HIGH);
  Serial.println("Powering done");
}


String ReadGSM() {  //функция чтения данных от GSM модуля
  char c;
  String str;
  //Serial.println(GSMport.available());
  while (SIM808.available()) {  //сохраняем входную строку в переменную str
    c = SIM808.read();
    str += c;
    delay(20);
  }
  str = "<<< " + str;
  //Serial.println(str);
  return str;
}

MyDataGPS getGPSLocation(void) {
  Serial.println("inside getGPSLocation");
  String readLocation;
  String data[5];
  MyDataGPS d;
  int a = 0, b = 0;
  SIM808.println("AT+CGNSPWR=1"); //Turn on GNSS(Global Navigation Satellite System) power
  delay(1000);
  Serial.println(ReadGSM());
  SIM808.println("AT+CGNSINF");  //GNSS navigation information parsed from NMEA sentences
  delay(400);
  delay(400);
  readLocation = ReadGSM();
  Serial.println("data is " + readLocation);

  for (int i = 0; i < 5; i++) { //parsing the location we got as one sentence
    a = readLocation.indexOf(",", a); //First occurrence of a comma
    if (a != -1) {
      b = readLocation.indexOf(",", a + 1); //The second occurrence of a comma after our first comma if a is not -1
      data[i] = readLocation.substring(a + 1, b);
      Serial.println(String("Poluchennaya data: " + String(i) + " - " + data[i]));
      a = b;
    }
  }
  d.datetime = data[1];
  d.latitude = data[2];
  d.longitude = data[3];
  return d;
}

void gprs_init(void) {  // Procedure GSM module initialization for GPRS
  
  int d = 500;
  Serial.println("---------------GPRS init start----------------");
  delay(d * 4);
  SIM808.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  //Set the connection settings
  delay(d * 5);
  Serial.println(ReadGSM());  //show a response from the GSM module
  delay(d * 5); //pinternet.interkom.de
  SIM808.println("AT+SAPBR=3,1,\"APN\",\"internet\""); //APN(acess point name is gatway between mobile
  //3g ,gprs ,.. and computer network) installation
  delay(d);
  Serial.println(ReadGSM());
  delay(d * 2);
  SIM808.println("AT+SAPBR=3,1,\"USER\",\"\""); //Setting the user name for the network
  delay(d);
  Serial.println(ReadGSM());

  SIM808.println("AT+SAPBR=3,1,\"PWD\",\"\""); //Установка пароля для сети
  delay(d);
  Serial.println(ReadGSM());  //показываем ответ от GSM модуля
  delay(d * 2);
  SIM808.println("AT+SAPBR=1,1");  //Устанавливаем соединение GPRS
  delay(d * 3);
  Serial.println(ReadGSM());  //показываем ответ от GSM модуля
  delay(d * 2);
 SIM808.println("AT+SAPBR=2,1"); //Проверяем как настроилось
  delay(d);
  Serial.println(ReadGSM());  //показываем ответ от GSM модуля
  delay(d * 5);

  Serial.println("----------------GPRS init complete-------------");
  Serial.println("");
}

void gprs_send(String dataa, String datab) {  //The procedure for sending data to the server
  //Sending data to the site
  String tempstr;
  Serial.println("Send start");
  SIM808.println("AT+HTTPINIT"); //  initializes the HTTP service
  delay(2000);
  Serial.println(ReadGSM());
  SIM808.println("AT+HTTPPARA=\"CID\",1");
  delay(1000);
  Serial.println(ReadGSM());
  Serial.println("setup url");
 SIM808.println("AT+HTTPPARA=\"URL\",\"http://jsonplaceholder.typicode.com/posts/1\"");
  Serial.println(tempstr);
  delay(5000);
  Serial.println(ReadGSM());
  Serial.println("GET url");
  SIM808.println("AT+HTTPACTION=0");
  delay(3000);
  Serial.println(ReadGSM());

  
  SIM808.println("AT+HTTPREAD");  // read the data from the website you access
  delay(3000);
  Serial.println(ReadGSM());
  Serial.println("Send done");
  SIM808.println("AT+HTTPTERM");
  delay(200);
  Serial.println(ReadGSM());
}

void try_send_HTTP() {

  // Initializes HTTP service
  SIM808.println("AT+CSQ"); // to check the signal strength
  delay(500);
  Serial.println(ReadGSM());

  // Sets CID parameter
  SIM808.println("AT+CGATT?"); // check if attached or detached the device to packet domain service
  delay(500);
  Serial.println(ReadGSM());

  SIM808.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  //setting the SAPBR, the connection type is using gprs
  delay(1000);
  Serial.println(ReadGSM());
  // Starts GET action
  SIM808.println("AT+SAPBR=3,1,\"APN\",\"internet\"");//setting the APN, the second need you fill in your local apn server
  delay(4000);
  Serial.println(ReadGSM());


SIM808.println("AT+SAPBR=1,1");//setting the SAPBR, for detail you can refer to the AT command mamual
  delay(5000);
  Serial.println(ReadGSM());


SIM808.println("AT+HTTPINIT");  // //init the HTTP request
  delay(3000);
  Serial.println(ReadGSM());

  SIM808.println("AT+HTTPPARA=\"URL\",\"www.google.com.hk\""); // setting the httppara, the second parameter is the website you want to access
  delay(3000);
  Serial.println(ReadGSM());

  SIM808.println("AT+HTTPACTION=0"); // sumbit the request
  delay(2000);
  Serial.println(ReadGSM());

  SIM808.println("AT+HTTPREAD");  // read the data from the website you access
  delay(3000);
  Serial.println(ReadGSM());


  SIM808.println("AT+HTTPTERM");

}
