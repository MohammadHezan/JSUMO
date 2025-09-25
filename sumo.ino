#define L_RPWM      10
#define L_LPWM      9
#define R_RPWM      5
#define R_LPWM      3

#define LINE_FL     A1
#define LINE_FR     A3
#define LINE_BL     A0
#define LINE_BR     A2

#define IR_LEFT       2
#define IR_CENTER     8
#define IR_RIGHT      7
#define IR_SIDE_LEFT  4
#define IR_SIDE_RIGHT 6

const int ATTACK_SPEED = 255;
const int TURN_SPEED   = 220;
const int SEARCH_SPEED = 220;
const int BACK_SPEED   = 255;
const int SIDE_TURN_SPEED = 255;

unsigned long lastAction = 0;
const unsigned long ACTION_INTERVAL = 50;
unsigned long searchTimer = 0;
float searchAngle = 0;
const float SEARCH_STEP = 15; //15

const unsigned long SIDE_TURN_TIME = 500;
unsigned long turnStart = 0;
bool turning = false;
int turnDirection = 0;

unsigned long lineActionStart = 0;
bool lineActionActive = false;
const unsigned long lineActionDuration = 800; // hold motors 500ms

enum State { LINE_FOLLOW, ATTACK, SEARCH };
State robotState = SEARCH;

enum EnemyDir { LEFT, CENTER, RIGHT };
EnemyDir lastEnemyDirection = CENTER;

int fr;
int fl;
int bl;
int br;

int readSensor(int pin)
{
	int sum = 0;
	for(int i = 0; i < 3; i++)
		sum += digitalRead(pin);
	return (sum >= 2);
}

void setMotors(int l_fwd, int l_bwd, int r_fwd, int r_bwd) {
  
  analogWrite(L_RPWM, l_fwd);
  analogWrite(L_LPWM, l_bwd);
  analogWrite(R_RPWM, r_fwd);
  analogWrite(R_LPWM, r_bwd);
}

void moveRight(int speed)     { setMotors(0, speed, speed, 0); }
void moveLeft(int speed)    { setMotors(speed, 0, 0, speed); }
void moveForward(int speed)  { setMotors(speed, 0, speed, 0); }
void moveBackward(int speed) { setMotors(0, speed, 0, speed); }
void stopMotors()            { setMotors(0,0,0,0); }

void handleLineEdgeCases(int fl, int fr, int bl, int br)
{
	if(!fl && !fr) { moveBackward(BACK_SPEED); delay(150); moveRight(TURN_SPEED); }
	else if(!bl && !br) moveForward(BACK_SPEED);
	else if(!fl && !bl) { moveBackward(BACK_SPEED); delay(150); moveRight(TURN_SPEED); }
	else if(!fr && !br) { moveBackward(BACK_SPEED); delay(150); moveLeft(TURN_SPEED); }
	else if(!fl) { moveBackward(BACK_SPEED); delay(150); moveRight(TURN_SPEED); }
	else if(!fr) { moveBackward(BACK_SPEED); delay(150); moveLeft(TURN_SPEED); }
	else if(!bl) { moveForward(BACK_SPEED); delay(150); moveRight(TURN_SPEED); }
	else if(!br) { moveForward(BACK_SPEED); delay(150); moveLeft(TURN_SPEED); }
	else if(!fl && !fr && !bl && !br) stopMotors();
}

void prepareLineCorrection(int fl, int fr, int bl, int br)
{
    lineActionStart = millis();
    lineActionActive = true;
}

bool handleLineCorrection()
{
    if(lineActionActive)
    {
        handleLineEdgeCases(fl, fr,  bl, br);
        if(millis() - lineActionStart >= lineActionDuration)
            lineActionActive = false;
        return true; // correction active
    }
    return false; // not active
}

void setup()
{
	pinMode(L_RPWM, OUTPUT);
	pinMode(L_LPWM, OUTPUT);
	pinMode(R_RPWM, OUTPUT);
	pinMode(R_LPWM, OUTPUT);

	pinMode(LINE_FL, INPUT);
	pinMode(LINE_FR, INPUT);
	pinMode(LINE_BL, INPUT);
	pinMode(LINE_BR, INPUT);

	pinMode(IR_LEFT, INPUT);
	pinMode(IR_CENTER, INPUT);
	pinMode(IR_RIGHT, INPUT);
	pinMode(IR_SIDE_LEFT, INPUT);
	pinMode(IR_SIDE_RIGHT, INPUT);

}

void loop()
{
  
	if(millis() - lastAction < ACTION_INTERVAL)
		return;
	lastAction = millis();

	fl = digitalRead(LINE_FL);
	fr = digitalRead(LINE_FR);
	bl = digitalRead(LINE_BL);
	br = digitalRead(LINE_BR);

  if(handleLineCorrection()) return; // skip normal behavior while holding

  if (!fl || !fr || !bl || !br){

    prepareLineCorrection(fl, fr, bl, br);
    return;
  }

	int irL  = digitalRead(IR_LEFT);
	int irC  = digitalRead(IR_CENTER);
	int irR  = digitalRead(IR_RIGHT);
	int irSL = digitalRead(IR_SIDE_LEFT);
	int irSR = digitalRead(IR_SIDE_RIGHT);

	if(!irL || !irC || !irR || !irSL || !irSR)
		robotState = ATTACK;
	else
		robotState = SEARCH;
  
	switch(robotState)
	{
	case ATTACK:
		if(!irC)
		{
			turning = false;
			moveForward(ATTACK_SPEED);
			lastEnemyDirection = CENTER;
		}
		else if(!irL)
		{
			moveLeft(TURN_SPEED);
			lastEnemyDirection = LEFT;
		}
		else if(!irR)
		{
			moveRight(TURN_SPEED);
			lastEnemyDirection = RIGHT;
		}
		else if(!irSL)
		{
			turning = true;
			turnStart = millis();
			turnDirection = -1;
		}
		else if(!irSR)
		{
			turning = true;
			turnStart = millis();
			turnDirection = +1;
		}

		if(turning)
		{
			if(millis() - turnStart < SIDE_TURN_TIME)
			{
				if(turnDirection == -1) moveLeft(SIDE_TURN_SPEED);
				else moveRight(SIDE_TURN_SPEED);
			}
			else
				turning = false;
		}
		break;

	case SEARCH:
		if(millis() - searchTimer > 150)
		{
			searchAngle += SEARCH_STEP;
			if(searchAngle >= 180)
				searchAngle = 0;
			searchTimer = millis();
		}

		if(lastEnemyDirection == LEFT)
		{
			if(searchAngle < 90) moveLeft(SEARCH_SPEED);
			else moveRight(SEARCH_SPEED);
		}
		else if(lastEnemyDirection == RIGHT)
		{
			if(searchAngle < 90) moveRight(SEARCH_SPEED);
			else moveLeft(SEARCH_SPEED);
		}
		else
		{
			if(searchAngle < 90) moveRight(SEARCH_SPEED);
			else moveLeft(SEARCH_SPEED);
		}
		moveForward(SEARCH_SPEED);
		break;
	}
}
