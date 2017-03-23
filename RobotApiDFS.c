#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

const short maxJunction = 15;

int constantPower, variablePower;
int integralConstant, derivativeConstant;
float previousError;
float Kp, Ki, Kd;
int corrval;
long red, green, blue;
short counter[maxJunction+1];
int curJunct; //junction pertama nilainya 1

//calib results
short pathTreshold = 45;
short lineTreshold = 17;

//aubprogram inti
void initialize();			//inisiasi setting motor & sensor
void DFSAction();
void Display();
//fungsi dasar
bool inColor();					//Ngecek posisi di blok warna
bool inLine();          //Ngecek di garis atau path
long normalizeHeading(long heading);
//basic movement
void followLine(short treshold, short leftoffset, short rightoffset);			//Ngikutin garis & path
void searchPath();			//Ngikutin sampe ketemu path
void followPath();			//Ngikutin garis doang
void turnBack();        //balik kanan grak (180")
void moveForward();     //maju lurus doang
void brake();

task main()
{
	initialize();

}

void initialize()
{
	setMotorBrakeMode(leftMotor,motorBrake);
	setMotorBrakeMode(rightMotor,motorBrake);
	resetGyro(gyroSensor);
	constantPower = 55;
	variablePower = 45;
	integralConstant = 0;
	derivativeConstant = 55;
	previousError = 0;
	Kp = 0;
	Ki = 0;
	Kd = 0;
	curJunct = 0;
}

void DFSAction()
{
	bool finish = false;
	searchPath();
	while (finish == false)
	{
		followpath();
		if (getColorName(colorSensor)==colorGreen)
	}
}

bool inColor()
{
	getColorRGB(colorSensor,red,green,blue);
	return (getColorName(colorSensor)==colorRed || (blue>=35 && red<35 && green<35) || getColorName(colorSensor)==colorGreen || getColorName(colorSensor)==colorYellow);
}

bool inLine()
{
	return (inColor() || getColorName(colorSensor)==colorBlack);
}

long normalizeHeading (long heading)
{
 	if (heading>0)
 	{
  	return heading % 360;
 	}
 	else
 	{
  	return ((heading%360)+360)%360;
 	}
}

void followLine(short treshold, short leftoffset, short rightoffset)
{
  corrval = (50-treshold);
	Kp = ((getColorAmbient(colorSensor) - treshold)) - corrval;
  Ki = Ki + Kp;
  Kd = (Kp - previousError);
  setMotorSpeed(rightMotor,constantPower + ((Kp * variablePower) / 50) + ((Ki * integralConstant) / 50) + ((Kd * derivativeConstant) / 50) + rightoffset);
  setMotorSpeed(leftMotor,constantPower - ((Kp * variablePower) / 50) - ((Ki * integralConstant) / 50) - ((Kd * derivativeConstant)/ 50) + leftoffset);
  previousError = Kp;  // Each time through the loop we set the new value for the previousError
}

void followPath()
{
	while (!inColor())
		followLine(pathTreshold,0,20);
}

void searchPath()
{
	followLine(lineTreshold,20,0);
}

void turnBack()
{
	long curdir = normalizeHeading(getGyroHeading(gyroSensor));
	long targetdir;
	if (curdir>=180)
	{
		targetdir = curdir;
		curdir = curdir - 180;
		while (curdir<targetdir)
		{
			setMotorSpeed(leftMotor,100);
			setMotorSpeed(rightMotor,-100);
			curdir = normalizeHeading(getGyroHeading(gyroSensor) - 180);
		}
 	}
 	else
 	{
 		targetdir = curdir + 180;
		while (curdir<targetdir)
		{
			setMotorSpeed(leftMotor,100);
			setMotorSpeed(rightMotor,-100);
			curdir = normalizeHeading(getGyroHeading(gyroSensor));
		}
	}
}

void moveForward()
{
	setMotorSpeed(rightMotor,100);
	setMotorSpeed(leftMotor,100);
	delay(2);
	brake();
}

void brake()
{
	setMotorSpeed(leftMotor,0);
	setMotorSpeed(rightMotor,0);
}
