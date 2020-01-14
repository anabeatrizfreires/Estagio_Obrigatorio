#define RED 5 // pino PWM 5 para a cor vermelha
#define GREEN 9 // pino PWM 9 para a cor verde
#define BLUE 6 // pino PWM 6 para a cor azul
int sensor = A0;
int sensorValue = 0;
float tensaoA2;
int voltimetro;
float relacaoA2=11;
#define AMOSTRAS 12


 
void setup(){
  // definindo os pinos como output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
    // Define o fotoresistor como uma entrada
  pinMode(sensor, INPUT);
  
  // Inicia a comunicação serial com uma taxa de transmissão de 9600 boud rate
  Serial.begin(9600);

  // No setup, um teste das cores
  
  // Acende e apaga o vermelho
  analogWrite(RED, 255);
  delay(1000);
  analogWrite(RED, 0);
  
  // Acende e apaga o verde
  analogWrite(GREEN, 255);
  delay(1000);
  analogWrite(GREEN, 0);
  
  // Acende e apaga o azul
  analogWrite(BLUE, 255);
  delay(1000);
  analogWrite(BLUE, 0);
  
  // Acendendo o azul, pois o loop do efeito de cores tem que começar com o azul aceso
  delay(1000);
  analogWrite(BLUE, 255);
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

void loop(){
  Serial.println("- .. -. .... .- -- .");
  tensaoA2 = ((lePorta(A2) * 5) / 1024.0)/10; //Calculo voltimetro
  voltimetro = (tensaoA2 * relacaoA2);

  // Comente uma das funções para ver o efeito desejado
  sensorValue = analogRead(sensor);
  if (sensorValue < 600) {
     if(voltimetro<=3.5)
  {
  // Declara as variáveis dos loops
  int r, g, b;
  
  // Fade do azul (ja estava aceso) até o violeta, adicionando o vermelho
  for(r = 0; r <= 255; r++){
    analogWrite(RED, r);
    delay(10);
  }
  
  // Fade do violeta até o vermelho, removendo o azul
  for(b = 255; b >= 0; b--){
    analogWrite(BLUE, b);
    delay(10);
  }
  
  // Fade do vermelho até o amarelo, adicionando o verde
  for(g = 0; g <= 255; g++){
    analogWrite(GREEN, g);
    delay(10);
  }
  
  // Fade do amarelo até o verde, removendo o vermelho
  for(r = 255; r >= 0; r--){
    analogWrite(RED, r);
    delay(10);
  }
  
  // Fade do verde até o turquesa, adicionando o azul
  for(b = 0; b <= 255; b++){
    analogWrite(BLUE, b);
    delay(10);
  }
  
  // Fade do turquesa até o azul, removendo o verde
  for(g = 255; g >= 0; g--){
    analogWrite(GREEN, g);
    delay(10);
  }
  
  // Em seguida o loop continua, com o azul aceso
}
 
  
   else 
   {
  analogWrite(RED, 255);
  analogWrite(GREEN, 255);
  analogWrite(BLUE, 255);
  delay(50);
  
  // Apaga todas as cores juntas
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
  delay(50);
   
   }
  }
  else {
    analogWrite(BLUE, 0);
    analogWrite(GREEN, 0);
    analogWrite(RED, 0);
      
  }
  
  // Imprime as leituras atuais no monitor serial da IDE do Arduino
  Serial.print ("Leitura atual do sensor: ");
  Serial.println(sensorValue);
  delay(1000);
}


