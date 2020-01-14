#include <SoftwareSerial.h>
#include "DHT.h"

float tensaoA0;
int voltimetro;
int DC_AC;
float aRef=5;
float relacaoA0=11;
float temperatura_ar;
float umidade_ar;
const int filtro  = 500;
#define DHTPIN 8
#define DHTTYPE DHT11
#define AMOSTRAS 12


DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial mySerial (50,51);

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

float lePorta(uint8_t portaAnalogica) 
{
  float total=0;  
  for (int i=0; i<AMOSTRAS; i++) 
  {
    total += 1.0 * analogRead(portaAnalogica);
    delay(5);
  }
  return total / (float)AMOSTRAS;
}  

void envia_mensagem() 
{
 if ((voltimetro)<=3 || (temperatura_ar)>=25) // Condicional generica
 { 
    mySerial.begin(9600);
    mySerial.println("AT"); //Estabelecendo comunicacao com SIM900
    updateSerial();
    mySerial.println("AT+CMGF=1"); // Configurando em modo de texto
    updateSerial();
    mySerial.println("AT+CMGS=\"+5561995492433\"");//Numero que será enviada a mensagem
    updateSerial();
    mySerial.print("Local - Bancada\nTemperatura: "); mySerial.print(temperatura_ar); mySerial.print("C\n"); mySerial.print("Tensao: "); mySerial.print(DC_AC); mySerial.println("V"); 
    updateSerial();
    mySerial.write(26);
  }
    
  else // Caso não seja a condicional só printa no serial monitor para acompanhar valores
  {
    Serial.print("Voltimetro: ");
    Serial.print(DC_AC);
    Serial.print ("V ");
    Serial.print("Temperatura: ");
    Serial.print(temperatura_ar);
    Serial.println ("C");  
  }

}
   

void updateSerial()// Atualização do serial do SIM900
{
  delay (500);
  while(Serial.available())
  {
    mySerial.write(Serial.read());
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());
  }
}
void loop() 
{
  updateSerial();
  tensaoA0 = ((lePorta(A0) * aRef) / 1024.0)/10; //Calculo voltimetro
  voltimetro = (tensaoA0 * relacaoA0);
  DC_AC = map(voltimetro, 0, 5, 0, 250);
  float h = dht.readHumidity(); //Leitura da umidade
  float t = dht.readTemperature(); //Leitura da temperatura
  
  if (isnan(h) || isnan(t)) 
  {
    Serial.println(F("Sensor nao detectado"));
    return;
  }
    temperatura_ar = 0; //Filtro de média móvel
    for (int j = 0; j <filtro; j++)
    {
      temperatura_ar = temperatura_ar + t;
      delay(1);
    }
    temperatura_ar = temperatura_ar / filtro;
    
    umidade_ar = 0;
    for (int k = 0; k <filtro; k++)
    {
      umidade_ar = umidade_ar + h;
      delay(1);
    }
    umidade_ar = umidade_ar / filtro;
  envia_mensagem(); 
  delay(5000);
}
