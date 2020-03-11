//Desenvolvido por: Ana Beatriz Freires Ferreira
//Projeto Controle e Verificação de Alarmes 

//Bibliotecas 
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "DHT.h"

//Definição de palavra chave de fácil acesso
#define PALAVRA_CHAVE "status"

//Definição de números usuários de fácil acesso
#define codigo_pais   "+55"
#define codigo_DDD    "61"
#define cel_number_1  "996183201"
#define cel_number_2  "995492433"
#define cel_number_3  "981117447"

//Definição de string para números usuários
#define number_chip_1 String(F(codigo_pais)) + String(F(codigo_DDD)) + String(F(cel_number_1))
#define number_chip_2 String(F(codigo_pais)) + String(F(codigo_DDD)) + String(F(cel_number_2))
#define number_chip_3 String(F(codigo_pais)) + String(F(codigo_DDD)) + String(F(cel_number_3))

//Variaveis sensor de temperatura
#define DHTPIN 8
#define DHTTYPE DHT11
#define AMOSTRAS 12
float temperatura_ar;
const int filtro  = 500;

//Variáveis para leitura de tensão na porta analógica
float tensaoA0;
int voltimetro;
int DC_AC;
float aRef=5;
float relacaoA0=11;

//Variáveis de armazenamento 
char rcv [100] = {0};
uint8_t index  = 0;
char incoming  = 0;
int flag_envio = 0;
int contador   = 0;

//Variaveis portas analógicas habilitáveis
const int LDR1 = A1; 
const int LDR2 = A2;
const int LDR3 = A3;
const int statusLDR1;
const int statusLDR2;
const int statusLDR3;

//Variaveis portas digitais habilitáveis
const int buttom1 = 2;
const int buttom2 = 3;
const int buttom3 = 4;
const int buttom4 = 5;
const int status_buttom1;
const int status_buttom2;
const int status_buttom3;

//Declaração de portas utilizadas pelo ArduinoMEGA2560
SoftwareSerial mySerial(50,51);
//Declaração tipo de função para sensor DHT
DHT dht(DHTPIN, DHTTYPE);

//Protótipo de funções
void envia_mensagem(char *msg);
void recebe_mensagem ();
bool analisa_numero(char *tok, bool is_done);
void atualizacao_status(char *tok);
void atualizacao_pane();
void deleta_mensagem();

//Le porta analogica
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

//Dado pequeno espaço de armazenamento deleta mensagens antigas
void deleta_mensagem ()
{
  mySerial.print ("AT+CMGDA=\"");
  mySerial.println("DEL ALL\"");
  delay(2000);
  Serial.println("Mensagens deletadas.");
}

//Analisa o numero, se for algum permitido prossegue
bool analisa_numero(char *tok, bool is_done)
{
  if (is_done)
  {
    return true;
  }

    char *pch1 = 0;
    char *pch2 = 0;
    char *pch3 = 0;
    pch1 = strstr(tok, cel_number_1);
    pch2 = strstr(tok, cel_number_2);
    pch3 = strstr(tok, cel_number_3);
    
  if (pch1 != NULL && pch2 == NULL && pch3 == NULL)
  {
    Serial.println("Numero de comando 1 OK.");
    flag_envio =  1;
    Serial.print("Analisa numero: ");
    Serial.println(flag_envio);
    return true;
  }

  if (pch1 == NULL && pch2 != NULL && pch3 == NULL)
  {
    Serial.println("Numero de comando 2 OK.");
    flag_envio =  2;
    Serial.print("Analisa numero: ");
    Serial.println(flag_envio);
    return true;
  }

  if (pch1 == NULL && pch2 == NULL && pch3 != NULL)
  {
    Serial.println("Numero de comando 3 OK.");
    flag_envio =  3;
    return true;
  }
  
    flag_envio = 0;
    return false;
}


//Le a mensagem e analisa
void atualizacao_status(char *tok)
{
  String msg = tok;
  if (msg.indexOf("status") >-1)
  {
    envia_mensagem();
    flag_envio = 0;   
  }
  
  else
  {
  }
}

void atualizacao_pane()
{
  float horario_da_pane = 0;
  if ((voltimetro)<=3 || (temperatura_ar)>=25) // Condicional generica
  { 
    while (contador <2)
    {  
      flag_envio = 4;
      envia_mensagem();
      contador= contador +1;
      delay (5000);
    }

    if (horario_da_pane == 0)
    {
      horario_da_pane = millis ();
    }

    else
    {
      if (millis () - horario_da_pane >=300000)
      {
        while (contador <2)
        {
          flag_envio = 4;
          envia_mensagem ();
          contador = contador + 1;
          delay (5000);
        }
        horario_da_pane = 0;
      }
    }
    delay (60000);
    contador = 0;
  }

  else
  {
  }

}

//Rotina de envio de SMS
void envia_mensagem()
{
  
  Serial.print("Envia mensagem: ");
  Serial.println(flag_envio);
  
  if (flag_envio == 1 && flag_envio != 2 && flag_envio != 3 && flag_envio != 4)
  {
   mensagem_numero_1 ();
  }
  
  else if (flag_envio != 1 && flag_envio == 2 && flag_envio != 3 && flag_envio != 4)
  {
   mensagem_numero_2();
  }

  else if (flag_envio != 1 && flag_envio != 2 && flag_envio == 3 && flag_envio != 4)
  {
   mensagem_numero_3();
  }
  
  else if (flag_envio != 1 && flag_envio != 2 && flag_envio != 3 && flag_envio == 4)
  {
    mensagem_numero_1 ();
    mensagem_numero_2 ();
    mensagem_numero_3 ();
  }

  
  else 
  {
    
  }
    flag_envio = 0;
}

//Rotina de recebimento de SMS
void recebe_mensagem ()
{
  mySerial.print("AT+CMGF=1\r");
  Serial.flush();
  delay(500);
  mySerial.println("AT+CNMI=1,2,0,0,0");
  delay(500);
  memset(rcv, 0, 100);
  
  while (mySerial.available())
  {
    incoming   = mySerial.read();
    rcv[index] = incoming;
    index ++; 
  }
  index = 0;

  char *rest = rcv;
  char *token;
  bool accept = false;
  if (strstr (rcv, "+CMT"))
  {
    while ((token = strtok_r(rest,",",&rest)) != NULL)
    {
      accept = analisa_numero(token, accept);
      if (accept)
      {
       atualizacao_status (token);
      }
    }
  }
  delay (1000);
}

void mensagem_numero_1 ()
{
  
  leitura_portas ();

  Serial.flush();
  delay(1000);
  mySerial.println("AT+CMGF=1\r"); 
  Serial.flush();
  delay(1000);
  mySerial.println(String(F("AT+CMGS=\"")) + number_chip_1 + String(F("\"\r")));
  flag_envio = 0;
  Serial.flush();
  delay(1000);

  mensagem_estruturada ();
}

void mensagem_numero_2 ()
{
  leitura_portas ();
  
  Serial.flush();
  delay(1000);
  mySerial.println("AT+CMGF=1\r"); 
  Serial.flush();
  delay(1000);
  mySerial.println(String(F("AT+CMGS=\"")) + number_chip_2 + String(F("\"\r"))); 
  flag_envio = 0;
  Serial.flush();
  delay(1000);

  mensagem_estruturada ();
}

void mensagem_numero_3 ()
{
  leitura_portas ();
  
  Serial.flush();
  delay(1000);
  mySerial.println("AT+CMGF=1\r"); 
  Serial.flush();
  delay(1000);
  mySerial.println(String(F("AT+CMGS=\"")) + number_chip_3+ String(F("\"\r"))); 
  flag_envio = 0;
  Serial.flush();
  delay(1000);

  mensagem_estruturada ();
}

void leitura_portas ()
{
  const int valueLDR1 = (analogRead(LDR1));
  const int valueLDR2 = (analogRead(LDR2));
  const int valueLDR3 = (analogRead(LDR3));
  const int value_buttom1 = (digitalRead (buttom1));
  const int value_buttom2 = (digitalRead (buttom2));
  const int value_buttom3 = (digitalRead (buttom3));
  const int value_buttom4 = (digitalRead (buttom4)); 
}

void mensagem_estruturada ()
{
  mySerial.print("Local - Bancada\nTemperatura: "); mySerial.print(temperatura_ar); mySerial.print("C\n"); mySerial.print("Tensao: ");
  mySerial.print(DC_AC); mySerial.println("V");
  Serial.flush();
  delay(1000);
  mySerial.print("A1: "); 
  const int statusLDR1 = (valueLDR1 <600 ? mySerial.println("Desabilitado") : mySerial.println(valueLDR1));
  mySerial.print("A2: ");
  const int statusLDR2 = (valueLDR2 <600 ? mySerial.println("Desabilitado") : mySerial.println(valueLDR2));
  mySerial.print("A3: ");
  const int statusLDR3 = (valueLDR3 <600 ? mySerial.println("Desabilitado") : mySerial.println(valueLDR3)); 
  
    if (value_buttom4 == HIGH)
    {
      mySerial.print("D1: ");
      const int status_buttom1 = (value_buttom1 == HIGH ? mySerial.println("0") : mySerial.println("1"));
      mySerial.print("D2: ");
      const int status_buttom2 = (value_buttom2 == HIGH ? mySerial.println("0") : mySerial.println("1"));
      mySerial.print("D3: ");
      const int status_buttom3 = (value_buttom3 == HIGH ? mySerial.println("0") : mySerial.println("1")); 
    }

    else
    {
      mySerial.println("D1: Desabilitado");
      mySerial.println("D2: Desabilitado");
      mySerial.println("D3: Desabilitado");
    } 
    
  delay(1000);
  mySerial.write(26);
}

void setup()
{
  mySerial.begin(9600);  
  Serial.begin(9600);
  dht.begin();
  Serial.println("Inicializando...");
  mySerial.println("AT"); 
  Serial.flush();
  delay(1000);
  mySerial.println("AT+CSQ"); 
  Serial.flush();
  delay(1000);
  mySerial.println("AT+CCID"); 
  Serial.flush();
  delay(1000);
  mySerial.println("AT+CREG?"); 
  Serial.flush();
  delay(1000);
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  pinMode(LDR3, INPUT);
  pinMode(buttom1, INPUT_PULLUP);
  pinMode(buttom2, INPUT_PULLUP);
  pinMode(buttom3, INPUT_PULLUP);
  
  deleta_mensagem();
}

void loop ()
{
 
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
     

  atualizacao_pane ();
  recebe_mensagem();
}
