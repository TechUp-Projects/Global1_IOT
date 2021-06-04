#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);


/* 

Calculo aproximado da temperatura usando o a equação de Steinhart–Hart

Referencia sobre o calculo https://en.wikipedia.org/wiki/Thermistor#NTC
http://sin.lyceeleyguescouffignal.fr/make-an-arduino-temperature-sensor-thermistor-tutorial

*/
int Botao = 2;
int Estado = 0;
int Modo = 0;

int ExposiUv = A1;
int Vout;
float R3 = 10000;
float Alpha=0.85;
float Rdark = 127410;
float Rldr, uv, L;

int TermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf, temp;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; //valores constante para calculo

void setup() {
  Serial.begin(9600);
  pinMode(TermistorPin, INPUT);
  pinMode(ExposiUv, INPUT);
  pinMode(Botao, INPUT);
  //Config do meu display oled
  oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  oled.clearDisplay();
}

void loop() {
  Estado = digitalRead(Botao);
  temp = readTemp(TermistorPin);
  uv = readUv(ExposiUv);
  printTemp(temp);
  printUv(uv);
  if (Estado == LOW){
	Modo=!Modo;
	delay(1000);
  }
  if (Modo == 1){
    oled.clearDisplay();
	printOledtemp(temp);
  }
  else {
    oled.clearDisplay();
	printOleduv(uv);
  }
  delay(500);
}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float readTemp(int ThermistorPin){
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculo R2, demonstração no arquivo pdf da aula
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));// Equação de Steinhart–Hart 
  Tc = T - 273.15; //temperatura em Graus celcius
  return Tc;
}
//função que faz a leitura dos raios em UV
float readUv (int ExposiUv){
  Vout = analogRead(ExposiUv);
  Rldr = R3 *( 1023.0 / (float) Vout - 1.0);
  L = pow(Rdark/Rldr, 1/Alpha);
  return L;
} 

//função que printa o valor da temperatura na serial
void printTemp(float Tc){
  Serial.print("Temperatura: ");
  Serial.print(Tc);
  Serial.println(" C");   

}
//função que printa o valor da exposição UV na serial
void printUv(float L){
  Serial.print("Exposicao: ");
  Serial.print(L);
  Serial.println(" Uv");   

}
void printOledtemp(float Tc){
  //Disparar o display oled
  oled.setTextSize(1); // Draw 1X-scale text
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(10, 0);
  oled.println();
  oled.print(" Sensor Temp: ");
  oled.println(Tc);
  oled.display();
}
void printOleduv(float L){
  //Disparar o display oled
  oled.setTextSize(1); // Draw 1X-scale text
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(10, 0);
  oled.println();
  oled.print(" Sensor UV: ");
  oled.println(L);
  oled.display();
}
