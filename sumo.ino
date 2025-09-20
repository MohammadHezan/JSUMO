#define L_RPWM      10
#define L_LPWM      9
#define R_RPWM      5
#define R_LPWM      3

#define LINE_FL     A1
#define LINE_FR     A3
#define LINE_BL     A0
#define LINE_BR     A2

#define IR_LEFT     2
#define IR_CENTER   8
#define IR_RIGHT    7

const int ATTACK_SPEED = 255;
const int TURN_SPEED   = 180;
const int SEARCH_SPEED = 150;
const int BACK_SPEED   = 200;

unsigned long lastAction = 0;
const unsigned long ACTION_INTERVAL = 50;

int readSensor(int pin)
{
  int sum = 0;
  for (int i = 0; i < 3; i++)
    sum += digitalRead(pin);
  return (sum >= 2);
}

void moveLeft(int speed)
{
  analogWrite(L_RPWM, speed);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, speed);
}

void moveRight(int speed)
{
  analogWrite(R_RPWM, speed);
  analogWrite(R_LPWM, 0);
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, speed);
}

void moveForward(int speed)
{
  analogWrite(L_RPWM, speed);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, speed);
  analogWrite(R_LPWM, 0);
}

void moveBackward(int speed)
{
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, speed);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, speed);
}

void stopMotors()
{
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, 0);
}

void handleLineEdgeCases(int fl, int fr, int bl, int br)
{
	if(!fl && !fr)
		moveBackward(BACK_SPEED);
	else if(!bl && !br)
		moveForward(BACK_SPEED);
	else if(!fl && !bl)
	{
		moveBackward(BACK_SPEED);
		moveRight(TURN_SPEED);
	}
	else if(!fr && !br)
	{
		moveBackward(BACK_SPEED);
		moveLeft(TURN_SPEED);
	}
	else if(!fl)
	{
		moveBackward(BACK_SPEED);
		moveRight(TURN_SPEED);
	}
	else if(!fr)
	{
		moveBackward(BACK_SPEED);
		moveLeft(TURN_SPEED);
	}
	else if(!bl)
	{
		moveForward(BACK_SPEED);
		moveRight(TURN_SPEED);
	}
	else if(!br)
	{
		moveForward(BACK_SPEED);
		moveLeft(TURN_SPEED);
	}
	else if(!fl && !fr && !bl && !br)
		stopMotors();
}

void setup()
{
  Serial.begin(9600);

  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);

  pinMode(LINE_FL, INPUT);
  pinMode(LINE_BL, INPUT);
  pinMode(LINE_BR, INPUT);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);

  stopMotors();
}

void loop()
{
  if (millis() - lastAction < ACTION_INTERVAL)
    return;
  lastAction = millis();

  int fl = readSensor(LINE_FL);
  int fr = readSensor(LINE_FR);
  int bl = readSensor(LINE_BL);
  int br = readSensor(LINE_BR);

  if(!fl || !fr || !bl || !br)
	{
		handleLineEdgeCases(fl, fr, bl, br);
		return;
	}
  
  int irL = readSensor(IR_LEFT);
  int irC = readSensor(IR_CENTER);
  int irR = readSensor(IR_RIGHT);

  if (!irC || !irL || !irR)
  {
    if (!irC)
      moveForward(ATTACK_SPEED);
    else if (!irL)
      moveLeft(TURN_SPEED);
    else if (!irR)
      moveRight(TURN_SPEED);
    return;
  }
  else
    moveRight(SEARCH_SPEED);
} 