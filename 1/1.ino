#define L_RPWM     10
#define L_LPWM     9
#define R_RPWM     5
#define R_LPWM     3
 
#define LINE_FL    A1
#define LINE_BL    A0
#define LINE_BR    A2
 
#define IR_LEFT    2
#define IR_CENTER  8
#define IR_RIGHT   7
 
const int ATTACK_SPEED = 255;
const int TURN_SPEED   = 180;
const int SEARCH_SPEED = 150;
const int BACK_SPEED   = 200;
 
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
    moveLeft(0);
    moveRight(0);
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
    delay(1000);
}
 
void loop()
{
 
    int fl = digitalRead(LINE_FL);
    int bl = digitalRead(LINE_BL);
    int br = digitalRead(LINE_BR);
 
    if (!fl || !bl || !br)
    {
        stopMotors();
        delay(50);
 
        if (!fl)
        {
            moveBackward(BACK_SPEED);
            delay(300);
            moveRight(TURN_SPEED);
            delay(250);
        }
        else if (!bl && !br)
        {
            moveForward(BACK_SPEED);
            delay(300);
        }
        else if (!fl)
        {
            moveBackward(BACK_SPEED);
            delay(250);
            moveRight(TURN_SPEED);
            delay(200);
        }
        else if (!bl)
        {
            moveForward(BACK_SPEED);
            delay(250);
            moveRight(TURN_SPEED);
            delay(200);
        }
        else if (!br)
        {
            moveForward(BACK_SPEED);
            delay(250);
            moveLeft(TURN_SPEED);
            delay(200);
        }
        return;
    }
 
    int irL = digitalRead(IR_LEFT);
    int irC = digitalRead(IR_CENTER);
    int irR = digitalRead(IR_RIGHT);
 
    if (!irC || !irL || !irR)
    {
        if (!irC)
        {
            moveForward(ATTACK_SPEED);
            delay(50);
        }
        else if (!irL)
        {
            moveLeft(TURN_SPEED);
            delay(50);
        }
        else if (!irR)
        {
            moveRight(TURN_SPEED); 
            delay(50);
        }
        return;
    }
    moveRight(SEARCH_SPEED);
}
 