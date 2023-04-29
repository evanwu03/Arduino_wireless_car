/*
 Name:		main.ino
 Created:	4/29/2023 1:23:41 PM
 Author:	Evan Wu, Om Davra


 Program allows for wireless control of an arduino-powered robot using the ArduinoBlue library and joystick app. 
 The HM-10 Module is used for bluetooth. 
*/






#include <ArduinoBlue.h>
#include <SoftwareSerial.h>
#include <Servo.h>


// Pin numbers 
const int RX_PIN = 8;
const int TX_PIN = 9;
const int LF_PIN = 10;
const int LB_PIN = 11;
const int RF_PIN = 12;
const int RB_PIN = 13;


// Speed definition 
const int STILL = 90;
const int MAX_FORWARD_SPEED = 180;
const int MIN_FORWARD_SPEED = 145;
const int MAX_BACKWARD_SPEED = 0;
const int MIN_BACKWARD_SPEED = 65;

// Servo definition 
Servo LeftFront;
Servo LeftBack;
Servo RightFront;
Servo RightBack;


// Bluetooth transmission 
SoftwareSerial bluetooth(RX_PIN, TX_PIN);
ArduinoBlue phone(bluetooth);


// Function prototypes
void setServos();
void controlDriving(int throttle, int steering);



void setup()
{

	// Start up bluetooth and serial monitor 
	bluetooth.begin(9600);
	Serial.begin(9600);

	// Attach pins to servos 
	setServos();

	// Give time for blueooth module to "get ready". 
	delay(100);

	Serial.println("Setup complete");

}



void loop()
{
	// Return throttle and steering values from Joystick 
	int throttle = phone.getThrottle();
	int steering = phone.getSteering();
	controlDriving(throttle, steering);
}




void setServos()
{
	pinMode(LF_PIN, OUTPUT);
	pinMode(LB_PIN, OUTPUT);
	pinMode(RF_PIN, OUTPUT);
	pinMode(RB_PIN, OUTPUT);

	LeftFront.attach(LF_PIN, 1000, 2000);
	LeftBack.attach(LB_PIN, 1000, 2000);
	RightFront.attach(RF_PIN, 1000, 2000);
	RightBack.attach(RB_PIN, 1000, 2000);

}



void controlDriving(int throttle, int steering)
{
	 throttle -= 49; 
	 steering -= 40;


	 // throttle 0 -> stopped
	 // steering 0 -> straight 
	 // throttle 49 -> max throttle forward
	 // throttle -49 -> max throttle backwards
	 // steering -49 -> turn left 
	 // steering 49 - > turn right
	
 	int mappedSpeed;
	int reducedSpeed;
	int leftMotorSpeed;
	int rightMotorSpeed;


	if (throttle == 0) // If throttle is 0, the robot will not move
	{
		leftMotorSpeed = STILL;
		rightMotorSpeed = STILL;
	}
	else if (throttle > 0) // If robot is moving forward
	{
		mappedSpeed = map(throttle, 0, 50, MIN_FORWARD_SPEED, MAX_FORWARD_SPEED);
		reducedSpeed = map(steering, 0, 50, mappedSpeed, MIN_FORWARD_SPEED);

		if (steering <= 0) // slow down left motors, so we can turn left forward
		{
			reducedSpeed = map(steering, 0, -49, mappedSpeed, MIN_FORWARD_SPEED);
			leftMotorSpeed = reducedSpeed;
			rightMotorSpeed = mappedSpeed;
		}
		else  // slow down right motors in order to turn forward right 
		{
			reducedSpeed = map(steering, 0, 50, mappedSpeed, MIN_FORWARD_SPEED);
			leftMotorSpeed = mappedSpeed;
			rightMotorSpeed = reducedSpeed;
		}




	}
	else  // If robot is moving backward
	{
		mappedSpeed = map(throttle, 0, -49, MIN_BACKWARD_SPEED, MAX_BACKWARD_SPEED);
		

		if (steering <= 0) // slow down left motors, so we can turn backward forward
		{
			reducedSpeed = map(steering, 0, -49, mappedSpeed, MIN_BACKWARD_SPEED);
			leftMotorSpeed = reducedSpeed;
			rightMotorSpeed = mappedSpeed;
		}
		else  // slow down right motors in order to turn backward right 
		{
			reducedSpeed = map(steering, 0, 50, mappedSpeed, MIN_BACKWARD_SPEED);
			leftMotorSpeed = mappedSpeed;
			rightMotorSpeed = reducedSpeed;
		}


	}

	// Debugging info
	Serial.print("Throttle: "); 
	Serial.print(throttle);
	Serial.print("\tSteering: ");
	Serial.print(steering);
	Serial.print("\tMappedSpeed: ");
	Serial.print(mappedSpeed);
	Serial.print("\tReducedSpeed: ");
	Serial.print(reducedSpeed);
	Serial.print("\tLeftMotorSpeed: ");
	Serial.print(leftMotorSpeed);
	Serial.print("\tRightMotorSpeed: ");
	Serial.print(rightMotorSpeed);
	

	// Write values to servos
	LeftFront.write(leftMotorSpeed);
	LeftBack.write(leftMotorSpeed);
	RightFront.write(rightMotorSpeed);
	RightBack.write(rightMotorSpeed);
}