const int QUANTITYUSER = 5;
long user[5] = {-1, -1, -1, -1, -1};
String data = "zero";
//первые 2 строки - моё гавно

#include <Servo.h>
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

Servo servos[5] = {servo1, servo2, servo3, servo4, servo5};

#include <Keypad.h> // подключаем библиотеку для управление клавиатурой

const byte ROWS = 4; //число строк у нашей клавиатуры
const byte COLS = 4; //число столбцов у нашей клавиатуры
char hexaKeys[ROWS][COLS] = {// здесь мы располагаем названия наших клавиш, как на клавиатуре,для удобства пользования
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};

byte rowPins[ROWS] = {29, 27, 25, 23}; //к каким выводам подключаем управление строками
byte colPins[COLS] = {37, 35, 33, 31}; //к каким выводам подключаем управление столбцами

char readUser[64];
char readEngine[10];
char radKeypad[64];
int index;
bool writingUser;
bool didWrite = false;

//передаем все эти данные библиотеке:
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

unsigned int timer[] = {0, 0, 0, 0, 0};

//всё что дальше тоже мои проделки

void pricessing_user(char* nameUser, char* engine) {
  //  Serial.println(val);
  long userInt = atol(nameUser);
  bool writeName = true;
  String userName = "";
  byte numEngine = 2;
  for (int i = 0; i < strlen(engine); i++)
  {
    if (engine[i] == '1') {
      digitalWrite(numEngine, HIGH);
      delay(1000);//Задержка на подачу положительного сигнала на транзисторы
      digitalWrite(numEngine, LOW);
    }
    numEngine++;
  }

  // Добавление пользователей в список существующих
  byte activeUser = 0;
  for (int i = 0; i < QUANTITYUSER; i++) {
    if (user[i] == -1)
    {
      user[i] = userInt;
      activeUser = i;
      break;
    } else if (user[i] == userInt) {
      activeUser = i;
      break;
    }
  }
  digitalWrite(11, HIGH);
  delay(3000);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  delay(2000 - activeUser * 500); //задержка проезда подноса
  digitalWrite(12, LOW);
  digitalWrite(13 + activeUser, HIGH);//выезд подноса
  delay(1000);
  digitalWrite(13 + activeUser, LOW);
}

void open_door(char key) {
  if (key == 0) {
    servo1.write(90);
  } else if (key == 1) {
    servo2.write(90);
  } else if (key == 2) {
    servo3.write(90);
  } else if (key == 3) {
    servo4.write(90);
  } else if (key == 4) {
    servo5.write(90);
  }
  user[key] = -1;
  timer[key] = millis();
}


void close_door() {
  for (int i = 0; i < 5; i++) {
    if (millis() - timer[i] > 3000) {
      if (i == 0) {
        servo1.write(0);
      } else if (i == 1) {
        servo2.write(0);
      } else if (i == 2) {
        servo3.write(0);
      } else if (i == 3) {
        servo4.write(0);
      } else if (i == 4) {
        servo5.write(0);
      }
    }
  }
}

void setup()
{
  memset(readUser, 0, 64);
  memset(readEngine, 0, 10);
  Serial.begin(9600);
  for (int i = 2; i < 11; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  for (int i = 13; i < 18; i++) {
    pinMode(i, OUTPUT);
  }
  servo1.attach(18);
  servo2.attach(19);
  servo3.attach(20);
  servo4.attach(21);
  servo5.attach(22);
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  servo5.write(0);
  //  pricessing_user("aboba.010101");
  //  pricessing_user("yura.010101");
}

char readBlocking() {
  while(!Serial.available()) {}
  return Serial.read();
}

int waitKey() {
  char key;
  while((key = customKeypad.getKey()) == 0) {}
  return key;
}


void loop()
{
  index = 0;
  writingUser = true;
  didWrite = false;
  if(Serial.available()) while(!didWrite) {
    char chr = readBlocking();
    if(chr != '.') {
      if(writingUser) {
        readUser[index++] = chr;
      }else{
        readEngine[index] = chr;
        if(index >= 8) {
          while(Serial.available()) Serial.read();
          didWrite = true;
          break;
        }
        index++;
      }
    }else{
      index = 0;
      writingUser = false;
    }
  }
  
  if(didWrite) {
    Serial.println(readUser);
    Serial.println(readEngine);
    pricessing_user(readUser, readEngine);
    memset(readUser, 0, 64);
    memset(readEngine, 0, 10);
  }
  
  /*if (Serial.available())
  {
    char info = int(Serial.read());
    Serial.print(info);
    pricessing_user(String(info));
  }*/

  char key = customKeypad.getKey();
  if (key) {
    index = 0;
    while(key != '#' && index < 64) {
      readUser[index++] = key;
      key = waitKey();
    }
    Serial.println(readUser);
    long userin = atol(readUser);
    Serial.println(userin);
    for (int i = 0; i < QUANTITYUSER; i++) {
      Serial.println(user[i]);
      if(user[i] == userin) open_door(i);
    }
    memset(readUser, 0, 64);
  }
  close_door();
}
