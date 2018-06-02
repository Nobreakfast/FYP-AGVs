#define numb 20
#define ns 9
int speedPin_M1 = 5;     //M1 Speed Control
int speedPin_M2 = 6;     //M2 Speed Control
int directionPin_M1 = 4;     //M1 Direction Control
int directionPin_M2 = 7;     //M1 Direction Control
byte table[10][10] = {{0, 9, 100, 100, 100, 100, 100, 6, 100},
                      {9, 0, 5, 100, 100, 100, 100, 100, 100},
                      {100, 5, 0, 6, 100, 100, 100, 100, 3},
                      {100, 100, 6, 0, 8, 100, 100, 100, 100},
                      {100, 100, 100, 8, 0, 2, 100, 100, 100},
                      {100, 100, 100, 100, 2, 0, 4, 100, 9},
                      {100, 100, 100, 100, 100, 4, 0, 7, 100},
                      {6, 100, 100, 100, 100, 100, 7, 0, 5},
                      {100, 100, 3, 100, 100, 9, 100, 5, 0}};
byte shortest[10][10];
byte pos = {{0, 5, 7.5, 10, 10, 6, 2.5, 0, 5}, {10, 10, 7.5, 5, 0, 0, 0, 5, 5}};
int angle;
byte i, j = 0, times, error = 0, s = 0;
byte led1 = 0, led2 = 0, led3 = 0, led4 = 0, led5 = 0, led6 = 0, led7 = 0;

byte start = 0, dest = 0;
byte old = 1, pre = 0, next = 0, ID = 2, function = 1;
byte pre_o, next_o = 0, ID_o = 1, function_o = 0;
String MessSend = "", MessRece = "";

byte status = 9;

void setup() {
  delay(2000);
  Serial.begin(115200);
  setup_IR();
  shortestmap();
  if (Serial.read() >= 0) {}
  delay(1000);
  MessSend = "";
  MessSend = "#" + String(ID) + String(function) + String(pre) + String(next);
  Serial.println(MessSend);
}

void loop() {
  switch (status) {
    case 0:
      delay(500);
      MessSd();
      switch (function)
      {
        case 1: status = 1; break;
        case 2: status = 2; break;
        case 0: status = 9; break;
      }
      break;
    case 1: //Dest
      if (byte(Serial.read()) == 45) {
        delay(100);
        MessRece = Serial.parseInt();
        MessDe();
        delay(500);
        if (dest==9)
          function = 1;
        else
          function = 2;
        status = 0;
      }
      break;

    case 2:
      if (byte(Serial.read()) == 45) {
        MessRece = Serial.parseInt();
        MessDe_o();
        Serial.println(MessRece);
        delay(500);
        status = 3;
      }
      break;
    case 3: //calculate the route
      next = pre_dest(pre, dest, pre_o, next_o);
      angle = turnAngle(old, pre, next);
      if (angle <= 0)
      {
        carRight(150, 150);
        angle = -angle;
      }
      else
        carLeft(150, 150);
      angle=int(angle*250);
      delay(angle);
      Serial.println(angle);
      MessSend = "";
      MessSend = "#" + String(ID) + String(function) + String(pre) + String(next);
      Serial.println(MessSend);
      status = 4;
      break;
    case 4: //line tracking
      linetracking();
      times = 0;
      old = pre;
      pre = next;
      if (pre == dest)
        status = 5;
      else status = 0;
      break;
    case 5: //finish
      Serial.println("Finish!");
      status = 0;
      function = 0;
      break;
    default:
      delay(2000);
      Serial.println("");
      Serial.println("New Mession Comes");
      status = 0;MessSend = "";
      MessSend = "#" + String(ID) + String(function) + String(pre) + String(next);
      Serial.println(MessSend);
      function = 1;
      break;
  }
}
// Decode Task from ESP8266
void MessDe()
{
  ID = byte(MessRece[0]) - 48;
  function = byte(MessRece[1]) - 48;
  pre = byte(MessRece[2]) - 48;
  dest = byte(MessRece[3]) - 48;
}
// Decode Other car information from ESP8266
void MessDe_o()
{
  ID_o = byte(MessRece[0]) - 48;
  function_o = byte(MessRece[1]) - 48;
  pre_o = byte(MessRece[2]) - 48;
  next_o = byte(MessRece[3]) - 48;
}
// Send the position information to ESP8266
void MessSd()
{
  MessSend = "";
  MessSend = "+" + String(ID) + String(function) + String(pre) + String(dest);
  Serial.println(MessSend);
}
// Setup the infrared sensors
void setup_IR()
{
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
}
//gather the return value of infrared sensors
void test()
{
  led1 = digitalRead(2);
  led2 = digitalRead(3);
  led3 = digitalRead(8);
  led4 = digitalRead(9);
  led5 = digitalRead(10);
  led6 = digitalRead(11);
  led7 = digitalRead(12);
}
// calculate the position of black line
int cal()
{
  test();
  int sum, i = 0;
  if (led1 == 0) {sum -= 6;i += 1;}
  if (led2 == 0) {sum -= 4;i += 1;}
  if (led3 == 0) {sum -= 2;i += 1;}
  if (led4 == 0) {sum += 0;i += 1;}
  if (led5 == 0) {sum += 2;i += 1;}
  if (led6 == 0) {sum += 4;i += 1;}
  if (led7 == 0) {sum += 6;i += 1;}
  if (i == 0) sum = 7;
  else if (i > 6) sum = 8;
  else if (i < 3) sum = sum / i;
  else if (sum < 0) sum = -6;
  else if (sum > 0) sum = 6;
  else sum = 0;
  return sum;
}
// Motor control
void carStop() {                //  Motor Stop
  digitalWrite(speedPin_M2, 0);
  digitalWrite(directionPin_M1, LOW);
  digitalWrite(speedPin_M1, 0);
  digitalWrite(directionPin_M2, LOW);
}

void carAdvance(int leftSpeed, int rightSpeed) {       //Move forward
  analogWrite (speedPin_M2, leftSpeed);             //PWM Speed Control
  digitalWrite(directionPin_M1, HIGH);
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, HIGH);
}

void carLeft(int leftSpeed, int rightSpeed) {         //Turn Right
  analogWrite (speedPin_M2, leftSpeed);
  digitalWrite(directionPin_M1, LOW);
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, HIGH);
}

void carRight(int leftSpeed, int rightSpeed) {        //Turn Left
  analogWrite (speedPin_M2, leftSpeed);
  digitalWrite(directionPin_M1, HIGH);
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, LOW);
}
// Calculate the shortest distance table
void shortestmap() {
  bool status[10];
  byte min = 100, bit = 0, factor = 0, num;
  for (i = 0; i < ns; i++)
    for (j = 0; j < ns; j++)
      shortest[i][j] = 100;
  for (i = 0; i < ns; i++)
  {
    for (j = 0; j < ns; j++)
      status[j] = false;
    factor = i;
    num = 0;
    while (status[factor] != true)
    {
      min = 100;
      status[factor] = true;
      for (j = 0; j < ns; j++)
      {
        if ((table[factor][j] + num) < shortest[i][j])
          shortest[i][j] = table[factor][j] + num;
        if ((shortest[i][j] < min) && (status[j] == false))
        {
          bit = j;
          min = shortest[i][j];
        }
      }
      num = shortest[i][bit];
      factor = bit;
    }
  }
}
// Calculate the next point of car
byte pre_dest(byte start, byte dest, byte changeA, byte changeB)
{
  byte point = start, num = 0, min = 100;
  byte m = table[changeA][changeB];
  byte n = table[changeB][changeA];
  table[changeA][changeB] = 100;
  table[changeB][changeA] = 100;
  shortestmap();
  for (i = 0; i < ns; i++)
    if ((table[point][i] < 100) && (table[point][i] > 0) && (shortest[dest][i] + table[point][i] < min))
    {
      min = shortest[dest][i] + table[point][i];
      num = i;
    }
  point = num;
  table[changeA][changeB] = m;
  table[changeB][changeA] = n;
  return point;
}
// calculate the angle it need to rotate
int turnAngle(byte old, byte pre, byte next)
{
 int x1, x2, y1, y2;
 x1 = pos[0][pre] - pos[0][old];
 y1 = pos[1][pre] - pos[1][old];
 x2 = pos[0][next] - pos[0][pre];
 y2 = pos[1][next] - pos[1][pre];
 float angle = atan2(y2, x2) - atan2(y1, x1);
 if (angle > 3.1415926)
   angle -= 2 * 3.1415926;
 else if (angle < -3.1415926)
   angle += 2 * 3.1415926;
return angle;
}
// based on the position of black line, follow the line
// and stop at the perpendicular black line
void linetracking()
{
  while (times < numb)
  {
    s = cal();
    switch (s)
    {
      case 6: carAdvance(20, 100); break;
      case 5: carAdvance(40, 100); break;
      case 4: carAdvance(55, 100); break;
      case 3: carAdvance(70, 100); break;
      case 2: carAdvance(80, 100); break;
      case 1: carAdvance(90, 100); break;
      case  0: carAdvance(100, 100); break;
      case -1: carAdvance(100, 90); break;
      case -2: carAdvance(100, 80); break;
      case -3: carAdvance(120, 70); break;
      case -4: carAdvance(120, 55); break;
      case -5: carAdvance(120, 40); break;
      case -6: carAdvance(120, 20); break;
      default: carAdvance(120, 100); break;
    }
    if (s == 8) times += 1;
    else {
      times = 0;
    }
  }
  carAdvance(100, 100);
  delay(500);
  carStop();
}
