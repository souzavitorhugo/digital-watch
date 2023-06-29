// Definição dos pinos do display de 7 segmentos
int displaySegments[7] = { 13, 12, 11, 10, 9, 8, 7 };
int displayPin[4] = { A3, A2, A1, A0 };
const int dp = 6;  // Pino do ponto decimal

// Indica a porta analógica ligada ao potenciômetro
const int pinPot = A5;

// Indica a porta digital ligada ao buzzer
const int pinBuzzer = A4;

// Definição dos pinos dos botões
int buttonIncrement = 1, buttonStop = 3, buttonAdjustment = 0, buttonMode = 2;

// Variáveis para controle do potenciômetro
int potValue = 0;
int frequency = 500;
float volt = 0;

// Variáveis para controle do buzzer
const int time = 100;  // Define o tempo em ms de cada tone

// Variáveis para controlar o tempo
int horas = 23;
int minutos = 59;
int segundos = 20;
unsigned long lastTime = 0;

// Variáveis Globais de ajuste
int mode = 0;
int settingAdjustment = 0;
int settingAlarmMode = 0;
bool adjustment = false;

// Variáveis do alarme
int alarmHours = 0;
int alarmMinutes = 0;
bool alarmStop = true;
bool Alarm = true;
bool activeAlarm = false;
bool stop = false;
bool Alarm1 = true;

// Matriz com os Numeros
int numbers[17][8] = {
  { 1, 1, 1, 1, 1, 1, 0, 0 },  // 0 = O
  { 0, 1, 1, 0, 0, 0, 0, 0 },  // 1
  { 1, 1, 0, 1, 1, 0, 1, 0 },  // 2
  { 1, 1, 1, 1, 0, 0, 1, 0 },  // 3
  { 0, 1, 1, 0, 0, 1, 1, 0 },  // 4
  { 1, 0, 1, 1, 0, 1, 1, 0 },  // 5
  { 1, 0, 1, 1, 1, 1, 1, 0 },  // 6
  { 1, 1, 1, 0, 0, 0, 0, 0 },  // 7
  { 1, 1, 1, 1, 1, 1, 1, 0 },  // 8
  { 1, 1, 1, 1, 0, 1, 1, 0 },  // 9
  { 0, 0, 0, 0, 0, 0, 0, 1 },  // Ponto Decimal (DP)
  { 0, 0, 1, 0, 1, 0, 1, 0 },  // n
  { 1, 0, 0, 0, 1, 1, 1, 0 },  // f
  { 1, 0, 0, 1, 1, 1, 0, 0 },  // (
  { 1, 1, 1, 1, 0, 0, 0, 0 },  // )
  { 1, 0, 0, 0, 0, 0, 0, 0 },  // -
  { 0, 0, 0, 1, 0, 0, 0, 0 },  // _
  //{ a, b, c, d, e, f, g, DP },  //
};

// Botão de Interrupção do alarme
void handleButtonInterrupt_stop() {

  if (stop) {
    Serial.println("entrou stop");
    noTone(pinBuzzer);
    alarmStop = false;
    delay(60000);  // Delay de 1 minuto
  }
}

// Botão de Interrupção para modo
void handleButtonInterrupt_mode() {
  Serial.print("mode: ");
  mode++;
  if (mode == 4)
    mode = 0;
  Serial.println(mode);
}


// Chamada das funçoes

void controlTime();                          // função para controlar o tempo
void showLetter(bool isOn);                  // Função para Mostrar as letras On ou Off
void showDP(int display, bool isOn);         // Função para Mostrar o Ponto Decimal
void showTime(int horas);                    // Função para Mostrar as Horas
void showMinutes(int minutos);               // Função para Mostrar os Minutos
void playBuzzer();                           // Função para controlar a frequencia do buzzer usando o potenciometro
void checkAlarm();                           // Função para verificar se o alarme e a hora estão iguais
void updateDisplay(int horas, int minutos);  // Função para Atualizar o display
void showFrequency(int frequency);           // Função para mostrar a frequencia no display

// Fim da chamada das funçoes

void setup() {
  Serial.begin(9600);
  // IMPUT PULLUP
  pinMode(buttonIncrement, INPUT_PULLUP);
  pinMode(buttonMode, INPUT_PULLUP);
  pinMode(buttonStop, INPUT_PULLUP);
  pinMode(buttonAdjustment, INPUT_PULLUP);

  // OUTPUT
  pinMode(pinBuzzer, OUTPUT);
  pinMode(dp, OUTPUT);

  // INPUT
  pinMode(pinPot, INPUT);  // Por padrão porta analógica é sempre INPUT

  attachInterrupt(digitalPinToInterrupt(buttonStop), handleButtonInterrupt_stop, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonMode), handleButtonInterrupt_mode, FALLING);

  for (int i = 0; i <= 6; i++) {
    pinMode(displaySegments[i], OUTPUT);
    if (i <= 3) {
      pinMode(displayPin[i], OUTPUT);
      digitalWrite(displayPin[i], HIGH);
    }
  }
}

void loop() {

  // Controla as horas, minutos e segundos
  controlTime();
  checkAlarm();

  switch (mode) {
    case 0:  //relogio funcionando

      // Atualizar o display de 7 segmentos com as horas, minutos e segundos
      updateDisplay(horas, minutos);
      noTone(pinBuzzer);
      settingAdjustment = 0;
      settingAlarmMode = 0;
      showDP(3, Alarm);

      break;

    case 1:  //Ajuste de relógio

      adjustment = true;

    case 2:  //Ajuste de alarme

      if (mode == 2) {
        adjustment = false;
        activeAlarm = true;
      }

      break;
    case 3:
      if (mode == 3) {
        activeAlarm = false;
        mode = 0;
      }
      break;
  }

  // Controla o Ajuste de horas
  if (adjustment) {

    // Pisca o DP 1
    static bool dpOn = true;
    if (millis() % 1000 < 500) {
      dpOn = true;
    } else {
      dpOn = false;
    }
    showDP(1, dpOn);  // Desligar o ponto decimal

    if (!digitalRead(buttonAdjustment)) {
      delay(100);
      Serial.print("settingAdjustment: ");
      settingAdjustment++;
      if (settingAdjustment == 5)
        settingAdjustment = 0;
      Serial.println(settingAdjustment);
    }
    switch (settingAdjustment) {

      case 0:  // Controla o ajuste das Horas

        noTone(pinBuzzer);
        showTime(horas);
        showMinutes(minutos);

        if (!digitalRead(buttonIncrement)) {
          Serial.print("Horas Anterior: ");
          Serial.println(horas);
          if (horas > 22) {
            horas = 0;
          } else {
            horas++;
          }
          Serial.print("Horas Agora: ");
          Serial.println(horas);
          delay(50);
        }

        break;
      case 1:  // Controla o ajuste dos Minutos
        showTime(horas);
        showMinutes(minutos);

        if (!digitalRead(buttonIncrement)) {
          if (minutos > 58) {
            minutos = 0;
          } else {
            minutos++;
          }
          delay(100);
        }

        break;

      case 2:  // Controla o ajuste das Horas do Alarme

        showTime(alarmHours);

        if (!digitalRead(buttonIncrement)) {
          Serial.print("Horas do alarme Anterior: ");
          Serial.println(alarmHours);
          if (alarmHours > 22) {
            alarmHours = 0;
          } else {
            alarmHours++;
          }
          Serial.print("Horas do alarme Agora: ");
          Serial.println(alarmHours);
          delay(100);
        }

        break;

      case 3:  // Controla o ajuste dos Minutos do Alarme

        showMinutes(alarmMinutes);

        if (!digitalRead(buttonIncrement)) {
          Serial.print("Incremento do alarme de minutos: ");
          if (alarmMinutes > 58) {
            alarmMinutes = 0;
          } else {
            alarmMinutes++;
          }
          delay(100);
        }

        break;

      case 4:  // ajusta frequencia do buzzer com o potenciometro

        playBuzzer();

        break;
    }
  }

  // Controla se o Alarme toca ou não
  if (activeAlarm) {
    showLetter(Alarm1);

    if (!digitalRead(buttonIncrement)) {
      Serial.print("Conf alarme anterior: ");
      Serial.println(Alarm1);

      if (Alarm1) {
        Alarm1 = false;
        Alarm = false;
      } else {
        Alarm1 = true;
        Alarm = true;
      }

      Serial.print("Conf alarme atual: ");
      Serial.println(Alarm1);
      delay(100);
    }
  }
}

// Função para a transição de telas
/* void showDetail(){

  // Ativar os segmentos para exibir o milhar
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[13][i]);
  }
  digitalWrite(displayPin[0], HIGH);  //Liga displayPin 0
  delay(5);
  digitalWrite(displayPin[0], LOW);  //Desliga displayPin 0

}*/

// Função para mostrar a frequencia no display
void showFrequency(int frequency) {
  int thousand = frequency / 1000;
  int rest = frequency % 1000;

  int hundred = rest / 100;
  rest = rest % 100;

  int ten = rest / 10;
  rest = rest % 10;

  int unit = rest;

  // Ativar os segmentos para exibir o milhar
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[thousand][i]);
  }
  digitalWrite(displayPin[0], HIGH);  //Liga displayPin 0
  delay(5);
  digitalWrite(displayPin[0], LOW);  //Desliga displayPin 0

  // Ativar os segmentos para exibir a centena
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[hundred][i]);
  }
  digitalWrite(displayPin[1], HIGH);  //Liga displayPin 1
  delay(5);
  digitalWrite(displayPin[1], LOW);  //Desliga displayPin 1

  // Ativar os segmentos para exibir a dezena
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[ten][i]);
  }
  digitalWrite(displayPin[2], HIGH);  //Liga displayPin 1
  delay(5);
  digitalWrite(displayPin[2], LOW);  //Desliga displayPin 1

  // Ativar os segmentos para exibir a unidade
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[unit][i]);
  }
  digitalWrite(displayPin[3], HIGH);  //Liga displayPin 1
  delay(5);
  digitalWrite(displayPin[3], LOW);  //Desliga displayPin 1
}

// Função para Atualizar o display
void updateDisplay(int horas, int minutos) {

  // Desligar todos os segmentos
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], LOW);
  }

  showTime(horas);
  showDP(1, true);
  showMinutes(minutos);
}

// função para controlar o tempo
void controlTime() {
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= 1000) {  // A cada 1000ms (1 segundo)
    lastTime = currentTime;

    segundos++;  // Incrementa os segundos

    if (segundos >= 60) {  // Se passou 1 minuto
      segundos = 0;
      minutos++;  // Incrementa os minutos

      if (minutos >= 60) {  // Se passou 1 hora
        minutos = 0;
        horas++;  // Incrementa as horas
        if (horas > 23) {
          horas = 0;
        }
      }
    }
  }
}

// Função para Mostrar as letras On
void showLetter(bool isOn) {
  int ligaAlarm = 0;
  if (isOn) {
    ligaAlarm = 0;
  } else {
    ligaAlarm = 1;
  }

  switch (ligaAlarm) {
    case 0:
      for (int i = 0; i <= 6; i++) {
        digitalWrite(displaySegments[i], numbers[0][i]);
      }
      digitalWrite(displayPin[0], HIGH);  //Liga displayPin 0
      delay(5);
      digitalWrite(displayPin[0], LOW);  //Desliga displayPin 0

      for (int i = 0; i <= 6; i++) {
        digitalWrite(displaySegments[i], numbers[11][i]);
      }
      digitalWrite(displayPin[1], HIGH);  //Liga displayPin 0
      delay(5);
      digitalWrite(displayPin[1], LOW);  //Desliga displayPin 0

      break;
    case 1:
      for (int i = 0; i <= 6; i++) {
        digitalWrite(displaySegments[i], numbers[0][i]);
      }
      digitalWrite(displayPin[0], HIGH);  //Liga displayPin 0
      delay(5);
      digitalWrite(displayPin[0], LOW);  //Desliga displayPin 0

      for (int i = 0; i <= 6; i++) {
        digitalWrite(displaySegments[i], numbers[12][i]);
      }
      digitalWrite(displayPin[1], HIGH);  //Liga displayPin 0
      delay(5);
      digitalWrite(displayPin[1], LOW);  //Desliga displayPin 0

      for (int i = 0; i <= 6; i++) {
        digitalWrite(displaySegments[i], numbers[12][i]);
      }
      digitalWrite(displayPin[2], HIGH);  //Liga displayPin 0
      delay(5);
      digitalWrite(displayPin[2], LOW);  //Desliga displayPin 0
      break;
  }
}

// Função para Mostrar o Ponto Decimal
void showDP(int display, bool isOn) {
  for (int i = 0; i <= 7; i++) {
    digitalWrite(displaySegments[i], numbers[10][i]);
  }
  digitalWrite(displayPin[display], HIGH);  // Liga displayPin 0
  if (isOn) {
    digitalWrite(dp, HIGH);  // Ligar o ponto decimal
    delay(5);
    digitalWrite(displayPin[display], LOW);  // Desliga displayPin 0
    digitalWrite(dp, LOW);                   // Desligar o ponto decimal
  } else {
    digitalWrite(displayPin[display], LOW);  // Desliga displayPin 0
    digitalWrite(dp, LOW);                   // Desligar o ponto decimal
  }
}

// Função para Mostrar as Horas
void showTime(int horas) {
  int tenValueHoras = horas / 10;
  int unitValueHoras = horas % 10;

  // Ativar os segmentos para exibir as horas
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[tenValueHoras][i]);
  }
  digitalWrite(displayPin[0], HIGH);  //Liga displayPin 0
  delay(5);
  digitalWrite(displayPin[0], LOW);  //Desliga displayPin 0

  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[unitValueHoras][i]);
  }
  digitalWrite(displayPin[1], HIGH);  //Liga displayPin 1
  delay(5);
  digitalWrite(displayPin[1], LOW);  //Desliga displayPin 1
}

// Função para Mostrar os Minutos
void showMinutes(int minutos) {
  int tenValueMinutos = minutos / 10;
  int unitValueMinutos = minutos % 10;

  // Ativar os segmentos para exibir os minutos
  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[tenValueMinutos][i]);
  }
  digitalWrite(displayPin[2], HIGH);  //Liga displayPin 2
  delay(5);
  digitalWrite(displayPin[2], LOW);  //Desliga displayPin 2

  for (int i = 0; i <= 6; i++) {
    digitalWrite(displaySegments[i], numbers[unitValueMinutos][i]);
  }
  digitalWrite(displayPin[3], HIGH);  //Liga displayPin 3
  delay(5);
  digitalWrite(displayPin[3], LOW);  //Desliga displayPin 3
}

// Função para controlar a frequencia do buzzer usando o potenciometro
void playBuzzer() {

  potValue = analogRead(pinPot);                // Lê valores no potenciômetro
  frequency = map(potValue, 0, 1023, 0, 2500);  //define valores de frequência
  volt = potValue * (5.00 / 1023.0);            //Valores da tensão no circuito

  // Serial.print("P = ");
  // Serial.print(potValue);
  // Serial.print(" | T = ");
  // Serial.print(volt);
  // Serial.print(" | F = ");
  // Serial.println(frequency);

  showFrequency(frequency);
  tone(pinBuzzer, frequency, time);
  delay(70);
}

// Função para verificar se o alarme e a hora estão iguais
void checkAlarm() {

  if (Alarm) {

    if (alarmStop) {

      if (horas == alarmHours && minutos == alarmMinutes) {
        tone(pinBuzzer, frequency, time);
        stop = true;
      }
    } else if (!alarmStop && minutos != alarmMinutes) {
      alarmStop = true;
    }
  }
}
