// Programa : Sensor de temperatura DS18B20 e Controlador de Brassagem
// Autor : Philipe Dias Mattos - Cervejaria Kauphi  
 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
 
// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 3
 
// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);

float tempC; //temperatura em celsius
// Armazena temperaturas minima e maxima
float tempMin;
float tempMax;
// Variáveis de controle de comando
int subindo = 0;
int passou = 0;
int buttonState;
int valor_botao = 0;
// Variáveis do contador
int seconds;
int minutes;
int hours;
int days;
long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second
 
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;
 
// Inicializa o LCD
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

const int Buzzer = 13;
const int ledRed = 9;
const int ledYellow = 10;
const int botao = 2;
 
void setup(void)
{
  pinMode(Buzzer, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(botao, INPUT);
  Serial.begin(9600);
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
     Serial.println("Sensores nao encontrados !"); 
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);
  Serial.println();
  Serial.println();
  lcd.begin(16, 2);
}
 
void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

//função que exibe tempo em formato de hora no serial
void printDigits(byte digits){
   // utility function for digital clock display: prints colon and leading 0
   Serial.print(":");
   if(digits < 10)
     Serial.print('0');
   Serial.print(digits,DEC);   
}

//função que conta tempo: cronometro
void time(){
  long timeNow = millis();

  days = timeNow / day ;                          //number of days
  hours = (timeNow) / hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
  minutes = ((timeNow) % hour) / minute ;         //and so on...
  seconds = (((timeNow) % hour) % minute) / second;

  // digital clock display of current time  
  Serial.print(days,DEC);
  printDigits(hours);  
  printDigits(minutes);
  printDigits(seconds);
  Serial.println(); 
}

void sensor(){
  // Le a informacao do sensor e apita
  sensors.requestTemperatures();
  tempC = sensors.getTempC(sensor1);
  // Atualiza temperaturas minima e maxima
  if (tempC <= tempMin)
  {
    /*if (subindo == 0){
      digitalWrite(Buzzer, LOW);
    }
    else{
      digitalWrite(Buzzer, LOW);
    }*/
    digitalWrite(Buzzer, HIGH);
    delay(200);
    digitalWrite(Buzzer, LOW);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, LOW);
  }
  else
    if (tempC >= tempMax)
    {
      digitalWrite(Buzzer, HIGH);
      delay(200);
      digitalWrite(Buzzer, LOW);
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
    }
  else{
    digitalWrite(Buzzer, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, LOW);
    //subindo = 1;
  }
  //fim da leitura do sensor de temperatura
}

void mostra_serial(){
  // Mostra dados no serial monitor
  Serial.println();
  Serial.print("VALOR BOTAO ");
  Serial.println(valor_botao);//mostra valor do botao no serial  
  Serial.print("Temp C: ");
  Serial.print(tempC); //mostra temperatura atual
  Serial.print(" Min : ");
  Serial.print(tempMin); //mostra temperatura minima informada
  Serial.print(" Max : ");
  Serial.println(tempMax); //mostra temperatura maxima informada
  /*Serial.print(" Subindo : ");
  if (subindo == 0){ 
    Serial.println("Sim ");
  }
  else{
    Serial.print("Não ");
  }*/
}

void mostra_lcd(){
  // Mostra dados no LCD  
  lcd.clear(); //limpa lcd
  
  //linha 1
  lcd.setCursor(0,0);
  lcd.print(tempC); //mostra temperatura atual lida
  //Simbolo grau
  lcd.setCursor(5,0);
  lcd.write(223); //simbolo de grau
  lcd.print("C"); 
  lcd.setCursor(8,0);
  if (hours<10){
    lcd.print("0");
  }
  lcd.print(hours); //mostra hora
  lcd.print(":");
  if (minutes<10){
    lcd.print("0");
  }
  lcd.print(minutes); //mostra minutos
  lcd.print(":");
  if (seconds<10){
    lcd.print("0");
  }
  lcd.print(seconds); //mostra segundos

  //linha 2
  lcd.setCursor(0,1); //posiciona cursor
  lcd.print("L:"); //escreve low
  lcd.setCursor(2,1);
  lcd.print(tempMin,1); //mostra temperatura mínima informada
  lcd.setCursor(7,1);
  lcd.print("H:"); //escreve high
  lcd.setCursor(9,1);
  lcd.print(tempMax,1); //mostra temperatura maxima informada
  lcd.setCursor(14,1);
  lcd.print(valor_botao,1); //mostra temperatura maxima informada  
}
//Passo 0: Inicial. Aguardando acionamento do botão para iniciar o Passo 1
void passo_0(){
  tempMin = 0;
  tempMax = 99;
  //exibe mensagem 
  Serial.println("Passo Inicial: Aguardando acionamento do botão para ir ao Passo 1");
}
//Passo 1: Parada Proteica - 54.6 C - adicionar malte e ficar 10 min
void passo_1(){
  Serial.println("Passo 1: Parada Protéica");
  tempMin = 52;
  tempMax = 54.6;
  if (tempC > 54 && tempC < 54.6 && passou == 0){
    passou = 1;
    //apita e exibe mensagem de desligar fogo e adicionar malte
    digitalWrite(Buzzer, HIGH);
    delay(200);
    digitalWrite(Buzzer, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledRed, LOW);
    Serial.println("Desligar fogo, adicionar malte e aguardar 10 minutos");
  }
  else{
    if (tempC >= tempMax){
      //apita, liga led vermelho e diz pra desligar o fogo
      Serial.println("Temperatura alta, desligar fogo");
    }
    else{
      if (tempC <= tempMin){
        //apita, liga led amarelo e diz pra ligar o fogo
        Serial.println("Temperatura baixa, ligar fogo");
      }
    }
  }
}
//Passo 2: Rampa de mosturação 65 C
void passo_2(){
  Serial.println("Passo 2: Rampa de mosturação");
  tempMin = 64.8;
  tempMax = 65.7;
  if (tempC > 63.8 && tempC < 64){
    //apita POR 1 SEGUNDO, liga led vermelho e amarelo e diz pra desligar o fogo
    Serial.println("Desligar fogo");
    digitalWrite(Buzzer, HIGH);
    delay(1000);
    digitalWrite(Buzzer, LOW);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, HIGH);
  }
  else{
    if (tempC >= tempMax){
      //apita, liga led vermelho e diz pra desligar o fogo
      Serial.println("Temperatura alta, desligar fogo e mexer o mosto");
    }
    else{
      if (tempC <= tempMin){
        //apita, liga led amarelo e diz pra ligar o fogo
        Serial.println("Temperatura baixa, ligar fogo e mexer o mosto");
      }
      else{
        //desliga leds e exibe mensagem de temperatura dentro do intervalo
        Serial.println("Temperatura dentro do esperado");
      }
    }  
  }
}
//Passo 3: Mash Out 75,6 C
void passo_MashOut() {
  Serial.println("Passo 3: Mash Out");
  tempMin = 76;
  tempMax = 79;
  if (tempC >= tempMax){
    //apita, liga led vermelho e diz pra desligar o fogo
    Serial.println("Temperatura alta, desligar fogo e mexer o mosto");
  }
  else{
    if (tempC <= tempMin){
      //apita, liga led amarelo e diz pra ligar o fogo
      Serial.println("Temperatura baixa, ligar fogo e mexer o mosto");
    }
    else{
      //desliga leds e exibe mensagem de temperatura dentro do intervalo
      Serial.println("Temperatura dentro do esperado");
    }
  }
}
//Passo 4: Lavagem
void passo_4(){
  Serial.println("Passo 4: Lavagem");
  tempMin = 70;
  tempMax = 81;
  if (tempC >= tempMax){
    //apita, liga led vermelho e diz pra desligar o fogo
    Serial.println("Temperatura alta, desligar fogo da panela de lavagem");
  }
  else{
    if (tempC <= tempMin){
      //apita, liga led amarelo e diz pra ligar o fogo
      Serial.println("Temperatura baixa, ligar fogo da panela de lavagem");
    }
    else{
      //desliga leds e exibe mensagem de temperatura dentro do intervalo
      Serial.println("Temperatura dentro do esperado");
    }
  }
}
//Fervura
void passo_5(){
  Serial.println("Passo 5: Fervura 60 min");
  tempMin = 0;
  tempMax = 99;
}
//Resfriamento
void passo_6(){
  Serial.println("Passo 6: Resfriamento ate 20C");
  tempMin = 18;
  tempMax = 22;
}
void loop()
{
  sensor(); //chama função que lê informações do sensor de temperatura
  time();//chamando a função contador de tempo 
    
  switch (valor_botao) {
     case 1:
        //chama função passo_1: parada proteica
        passo_1();
     break;
     case 2:
        //chama função passo_2
        passo_2();
     break;
     case 3:
        //chama função passo_3
        passo_MashOut();
        //Serial.println("Passo 3");
     break;
     case 4:
        //chama função passo_4
        passo_4();
     break;
     case 5:
        //chama função passo_5
        passo_5();
     break;
     case 6:
        //chama função passo_5
        passo_6();
     break;
     /*case 7:
        //chama função passo_5
        valor_botao = 0;
        passo_0();
     break;*/
     default:
        // se nada mais encaixa, fazer o padrão
        valor_botao = 0;
        passo_0();
     break;
  }

  // verifica se o botao esta pressionado
  buttonState = digitalRead(botao);
  // verifica se o botao esta pressionado. em caso positivo, buttonState = HIGH:
  if (buttonState == HIGH){ 
      valor_botao++; 
      digitalWrite(Buzzer, HIGH);
  }
  else{
    digitalWrite(Buzzer, LOW);
  }
  
  mostra_serial(); //chama a função que mostra informações no serial monitor
  mostra_lcd(); //chama função que mostra informações no lcd
}
