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
long degree[maxJunction*4];
int curJunct; //junction pertama nilainya 1
char kata[15];
int nkata = 0;

//calib results
short pathTreshold = 45;
short lineTreshold = 17;

//aubprogram inti
void initialize();			//inisiasi setting motor & sensor
void BFSAction();
void Display(int way);
//fungsi dasar
bool inColor();					//Ngecek posisi di blok warna
bool inLine();          //Ngecek di garis atau path
long normalizeHeading(long heading);
//basic movement
void followLine(short treshold, short leftoffset, short rightoffset);			//Ngikutin garis & path
void searchPath();			//Ngikutin sampe ketemu path
void followPath();			//Ngikutin garis doang
void turnBack();        //balik kanan grak (180")
void moveForward(long t);     //maju lurus doang
void brake();
void extinguish();

task main()
{
	initialize();
	BFSAction();
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
	degree[curJunct] = getGyroHeading(gyroSensor);
	eraseDisplay();
}

void BFSAction()
{

	bool mode[maxJunction]; //1 seek, 0 explore
	bool finish = false;
	bool returning = false;
	bool willreturn = false;

	searchPath();            //blok biru start
	while (finish==false)
	{
		followPath();
		getColorRGB(colorSensor,red, green, blue);
		if (blue>=35 && red<35 && green<35)
		{
			finish = true;
			brake();
		}
		else if (getColorName(colorSensor)==colorRed)
		{
			turnBack();
			willreturn = false;
			returning = true;
			followPath();
			//tidak dicatat agar tidak ditelusuri ketika penambahan level

		}
		else if (getColorName(colorSensor)==colorYellow)
		{
			extinguish();
			finish = true;
			turnBack();
			returning = true;
			followPath();
		}
		else //Junction
		{
			if (mode[curJunct]==false) //explore
			{
				if (willreturn==true && returning == false) //new junction to acknowledge
				{
					curJunct++;
					counter[curJunct]=0;
					mode[curJunct]=false;
					degree[4*curJunct]=normalizeHeading(getGyroHeading(gyroSensor));
					willreturn=false;
					returning = true;
					curJunct--;
					turnBack();         //go back to explore
				}
				else if (willreturn==false && returning==true) //returning from explore
				{
					counter[curJunct]++;
					if (counter[curJunct]==3)
					{
						//nothing to explore
						mode[curJunct] = true;
					}
					searchPath();
					if (-30<(normalizeHeading(degree[curJunct*4]-getGyroHeading(gyroSensor)-180)) && (normalizeHeading(degree[curJunct*4]-getGyroHeading(gyroSensor)-180))<30)
					{
						//nothing to explore
						mode[curJunct] = true;
					}
					if (mode[curJunct] == true)
					{
						willreturn=false;
						returning=false;
						searchPath();  //last hit adalah path kembali
					}
					else
			 		{
			 			willreturn = true;
			 			returning = false;
			 		}
				}

			}
			else //seek or initial
			{
				if (willreturn==false && returning==true) //return from seek
				{
					searchPath();
					if (-30<(normalizeHeading(degree[curJunct*4]-getGyroHeading(gyroSensor)-180)) && (normalizeHeading(degree[curJunct*4]-getGyroHeading(gyroSensor)-180))<30)
					{	//no object remaining, path adalah path kembali
						willreturn = false;
						returning = true;
						followPath();
					}
					else
					{
						willreturn = true;
						returning = false;
						followPath(); //ready to search
					}
				}
				else if (willreturn==false && returning==false)//new junction to explore
				{
					curJunct++;
					counter[curJunct]=0;
					mode[curJunct]=false;
					degree[4*curJunct]=normalizeHeading(getGyroHeading(gyroSensor));
					willreturn=true;
					searchPath(); //ready to search
				}
			}
		}
	}
}

void Display(int way)
{
 eraseDisplay();
 char x;
 x = way +'0';
 kata[nkata] = x;
 nkata++;
 displayString(1,"Lintasan : ");
 displayString(2,kata);
 sleep(50);
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
	while (inColor())
		followLine(lineTreshold,20,0);
}

void turnBack()
{

	brake();
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
	brake();
}

void moveForward(long t)
{
	setMotorSpeed(rightMotor,100);
	setMotorSpeed(leftMotor,100);
	delay(t);
	//brake();
}

void brake()
{
	setMotorSpeed(leftMotor,0);
	setMotorSpeed(rightMotor,0);
}

void extinguish()
{
	brake();
	for (int i=0; i<5; i++)
	{
		setMotorSpeed(armMotor,100);
		delay(70);
		setMotorSpeed(armMotor,-100);
		delay(70);
	}
}
