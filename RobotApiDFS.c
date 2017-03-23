#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//



int constantPower = 35;
int variablePower = 30;
int integralConstant = 0;
int derivativeConstant = 0;
float previousError = 0;
float Kp = 0;
float Ki = 0;
float Kd = 0;

//calib results
short lineTreshold = 50;

void initialize();			//inisiasi setting motor & sensor
void DFSAction();
bool inColor();					//Ngecek posisi di blok warna
bool inLine();          //Ngecek di garis atau kaga
void followLine();			//Ngikutin garis doang
void moveForward();     //maju lurus doang
void turnRight();
void turnLeft();
void brake();

task main()
{
	initialize();
	//while(1)
		//turnLeft();
		followLine();
	//lineTreshold = getColorAmbient(colorSensor);
}

void initialize()
{
	setMotorBrakeMode(leftMotor,motorBrake);
	setMotorBrakeMode(rightMotor,motorBrake);
}

void DFSAction()
{

}

bool inColor()
{
	return (getColorName(colorSensor)==colorRed || getColorName(colorSensor)==colorBlue || getColorName(colorSensor)==colorGreen || getColorName(colorSensor)==colorYellow);
}

bool inLine()
{
	return (inColor() || getColorName(colorSensor)==colorBlack);
}

void followLine()
/*{
	bool switcher = false;             //turn slightly right if 1
	while (!inColor())
	{
		while (inLine() && !inColor())
			moveForward();
		if (!inLine() && !inColor())
		{
			brake();
			if (switcher)
			{
				resetGyro(gyroSensor);
				while (!inLine() && getGyroHeading(gyroSensor)<90)
				{
					turnRight();
				}
				if (getGyroHeading(gyroSensor)>=90)
				{
					repeatUntil(getGyroHeading(gyroSensor)<=0)
						turnLeft();
					//switcher=!switcher;
				}
				resetGyro(gyroSensor);
			}
			else
			{
				resetGyro(gyroSensor);
				while (!inLine() && getGyroHeading(gyroSensor)>-90)
				{
					turnLeft();
				}
				if (getGyroHeading(gyroSensor)<=-90)
				{
					repeatUntil(getGyroHeading(gyroSensor)>=0)
						turnRight();
					//switcher=!switcher;
				}
				resetGyro(gyroSensor);
			}
			switcher = !switcher;
		}
	}
}
*/
{
  while(!inColor())
  {
    float corrval = (50-lineTreshold)/50;
  	Kp = ((getColorAmbient(colorSensor) - lineTreshold) / 50) - corrval;
    Ki = Ki + ((getColorAmbient(colorSensor) - lineTreshold) / 50) - corrval;
    Kd = (Kp - previousError);
    motor[rightMotor] = constantPower + (Kp * variablePower * 1.5) + (Ki * integralConstant) + (Kd * derivativeConstant);
    motor[leftMotor] = constantPower - (Kp * variablePower * 1.5) - (Ki * integralConstant) - (Kd * derivativeConstant);
    previousError = Kp;  // Each time through the loop we set the new value for the previousError
  }
}
void moveForward()
{
	setMotorSpeed(rightMotor,100);
	setMotorSpeed(leftMotor,100);
	delay(2);
	brake();
}

void turnRight()
{
	setMotorSpeed(rightMotor,-100);
	setMotorSpeed(leftMotor,100);
	delay(2);
	brake();
}

void turnLeft()
{
	setMotorSpeed(rightMotor,100);
	setMotorSpeed(leftMotor,-100);
	delay(2);
	brake();
}

void brake()
{
	setMotorSpeed(leftMotor,0);
	setMotorSpeed(rightMotor,0);
}
