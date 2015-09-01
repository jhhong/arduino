/*
**************** 제작자 : naver cafe '아두이노 스토리' 닉네임 : 개파 *****************
현재 조종기 없이 만들었고, 호버링 테스트 중 입니다.
A1핀에 가변저항을 길게해서 연결하고 아날로그 입력을 받아 스로틀 값을 받고 있습니다.
지금 Yaw축 제어법을 몰라서 놔두고 Roll, Pitch만으로 제어하고 있습니다.
지금 문제는 호버링 중 PID값이 안맞는지 평형이 안맞습니다.
틀린부분도 있을 수 있지만 일단 저의 생각으로는 PID게인을 잘 맞추면 성공 할 것 같습니다.
수정 할 부분 수정하시면서 같이 완성합시다.
**************************************************************************************
*아두이노 나노 전원은 모터전원과 분리되어 있고 9V건전지를 사용했습니다.

[motor 방향]
     1
  4     3
     2
(1,2 : 반 시계방향 회전/ 3,4 : 시계방향 회전)

[핀번호]
모터1 : D3 (490Hz)
모터2 : D9 (490Hz)
모터3 : D10 (490Hz)
모터4 : D11 (490Hz)
준비중 LED : D13
비행시작 LED : D12
GY-86 SCL : A5
GY-86 SDA : A4
*/

//////////////////////////////////////////////////////////////////////
///                           PID                                  ///
//////////////////////////////////////////////////////////////////////
#include <PID_v1.h>
double gap = 2; //오차각도가 gap 보다 작을 때와 클 때 PID 게인을 다르게 주기 위해 추가한 부분, 실험적으로 몇 도가 좋은지 찾아내야 함.
double consKp=5, consKi=0.01, consKd=1; //오차가 gap보다 작을 때 PID게인 (이 값을 실험으로 찾는 것이 주요 포인트)
double aggKp=1, aggKi=0.01, aggKd=0.5; //오차가 gap보다 클 때 PID게인 (이 값을 실험으로 찾는 것이 주요 포인트)

double Setpoint_X, Input_X, Output_X, //setpoint : 목표값(기체가 0도를 유지해야 하므로 0)
       Setpoint_Y, Input_Y, Output_Y, 
       Setpoint_Z, Input_Z, Output_Z;

//PID(센서피드백PORT,제어출력OUTPUT,목표값,kp,ki,kd,컨트롤방향)
PID myPID_X(&Input_X, &Output_X, &Setpoint_X, consKp, consKi, consKd, REVERSE);
PID myPID_Y(&Input_Y, &Output_Y, &Setpoint_Y, consKp, consKi, consKd, REVERSE);
//PID myPID_Z(&Input_Z, &Output_Z, &Setpoint_Z, 0, 0, 0, REVERSE);

float swing = 2000; //스윙폭 최댓값(기울어진 쪽 모터의 최대 출력 범위)

float standard_1, standard_2, standard_3, standard_4 = 0; //모터 기본 출력값

//////////////////////////////////////////////////////////////////////
///                           sensor                               ///
//////////////////////////////////////////////////////////////////////
#include <math.h> // (no semicolon)
#include <Wire.h>

/* MPU-6050 sensor */
#define MPU6050_ACCEL_XOUT_H 0x3B // R
#define MPU6050_PWR_MGMT_1 0x6B // R/W
#define MPU6050_PWR_MGMT_2 0x6C // R/W
#define MPU6050_WHO_AM_I 0x75 // R
#define MPU6050_I2C_ADDRESS 0x68
float Degree_X, Degree_Y, Degree_Z;
float offset_X, offset_Y;
float offset_tmp_X, offset_tmp_Y;

//LPF
#define MPU6050_CONFIG 0x1A 
#define MPU6050_DLPF_5HZ 0x0E

typedef union accel_t_gyro_union
{
	struct
	{
		uint8_t x_accel_h;	uint8_t x_accel_l;
		uint8_t y_accel_h;	uint8_t y_accel_l;
		uint8_t z_accel_h;	uint8_t z_accel_l;
		uint8_t t_h;	
		uint8_t t_l;
		uint8_t x_gyro_h;	uint8_t x_gyro_l;
		uint8_t y_gyro_h;	uint8_t y_gyro_l;
		uint8_t z_gyro_h;	uint8_t z_gyro_l;
	} reg;

	struct
	{
		int x_accel;
		int y_accel;
		int z_accel;
		int temperature;
		int x_gyro;
		int y_gyro;
		int z_gyro;
	} value;
};

int xInit[5] = {0,0,0,0,0};
int yInit[5] = {0,0,0,0,0};
int zInit[5] = {0,0,0,0,0};
int initIndex = 0;
int initSize = 5;
int xCal = 0;
int yCal = 0;
int zCal = 1800;
//////////////////////////////////////////////////////////////////////
///                     Kalman filter                              ///
//////////////////////////////////////////////////////////////////////

struct GyroKalman
{
	float x_angle, x_bias;
	float P_00, P_01, P_10, P_11;
	float Q_angle, Q_gyro;
	float R_angle;
};

struct GyroKalman angX;
struct GyroKalman angY;
struct GyroKalman angZ;

static const float R_angle = 0.3; 	//.3 default

static const float Q_angle = 0.01;	//0.01 (Kalman)
static const float Q_gyro = 0.04;	//0.04 (Kalman)

//These are the limits of the values I got out of the Nunchuk accelerometers (yours may vary).
const int lowX = -2150;
const int highX = 2210;
const int lowY = -2150;
const int highY = 2210;
const int lowZ = -2150;
const int highZ = 2550;

/* time */
unsigned long prevSensoredTime = 0;
unsigned long curSensoredTime = 0;

//////////////////////////////////////////////////////////////////////
///                             MOTOR                              ///
//////////////////////////////////////////////////////////////////////
//PWM신호를 만드는 것을 서보모터 제어 헤더파일을 사용했습니다.
#include <Servo.h> 
Servo motor_1, motor_2, motor_3, motor_4;

float Speed_Motor_1 = 0, //기본 회전속도
      Speed_Motor_2 = 0,
      Speed_Motor_3 = 0,
      Speed_Motor_4 = 0;
 
void setup()
{
        pinMode(13,OUTPUT);
        pinMode(12,OUTPUT);
	int error;
	uint8_t c;

	initGyroKalman(&angX, Q_angle, Q_gyro, R_angle);
	initGyroKalman(&angY, Q_angle, Q_gyro, R_angle);
	initGyroKalman(&angZ, Q_angle, Q_gyro, R_angle);

	Serial.begin(9600);
	Wire.begin();

	// default at power-up:
	// Gyro at 250 degrees second
	// Acceleration at 2g
	// Clock source at internal 8MHz
	// The device is in sleep mode.
	//
	error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
	Serial.print(F("WHO_AM_I : "));
	Serial.print(c,HEX);
	Serial.print(F(", error = "));
	Serial.println(error,DEC);

	// According to the datasheet, the 'sleep' bit
	// should read a '1'. But I read a '0'.
	// That bit has to be cleared, since the sensor
	// is in sleep mode at power-up. Even if the
	// bit reads '0'.
	error = MPU6050_read (MPU6050_PWR_MGMT_2, &c, 1);
	Serial.print(F("PWR_MGMT_2 : "));
	Serial.print(c,HEX);
	Serial.print(F(", error = "));
	Serial.println(error,DEC);

	// Clear the 'sleep' bit to start the sensor.
	MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
	MPU6050_write_reg (MPU6050_CONFIG, MPU6050_DLPF_5HZ);


//////////////////////////////////////////////////////////////////////
///                       setup MOTOR                              ///
//////////////////////////////////////////////////////////////////////
	//PWM신호 출력핀 설정. 주기시간 최소 10000부터 20000까지
	motor_1.attach(3,1000,2000);
	motor_2.attach(9,1000,2000);
	motor_3.attach(10,1000,2000);
	motor_4.attach(11,1000,2000);

	//모터 시동(캘리브레이션) : 캘리브레이션이 이렇게 하는 것이 맞는지 확실히 모르겠음.
        digitalWrite(13,HIGH);////준비 LED 점등
        delay(1000);
	motor_1.write(5);  motor_2.write(5); //시동
	motor_3.write(5);  motor_4.write(5);
	delay(4000);
        digitalWrite(13,LOW);

//////////////////////////////////////////////////////////////////////
///                        setup PID                               ///
//////////////////////////////////////////////////////////////////////

	//목표값 0도
	Setpoint_X, Setpoint_Y = 0; //, Setpoint_Z = 0;

	//turn the PID on
	myPID_X.SetMode(AUTOMATIC);  myPID_Y.SetMode(AUTOMATIC);  //myPID_Z.SetMode(AUTOMATIC);

} //setup

void loop()
{   
        digitalWrite(12,HIGH);//비행가능 led 점등
	sensor(); //센서함수 호출

        //센서를 수평으로 놓아도 0도가 되지 않기 때문에 아래부분을 추가하였습니다.
        //처음 켰을 때 수평에서 시작해야 합니다. 그래야 옵셋오차를 계산하여 빼줍니다.
	//loop가 10번 돌았을 때  부터 20번 돌 때 까지의 오차값의 평균을 구합니다.

        int Loop = 0;
	if(Loop < 22) 
          Loop++; 
	else if ( (Loop>9)&&(Loop<21) )//10번 째 루프부터 10번 옵셋 오차의 누적합을 구합니다.
        { 
		offset_tmp_X += Degree_X; 
		offset_tmp_Y += Degree_Y;
	}
	else if(Loop ==21)
        {
		offset_X = offset_tmp_X/10;
		offset_Y = offset_tmp_Y/10;
	}

//////////////////////////////////////////////////////////////////////
///                        loop PID                                ///
//////////////////////////////////////////////////////////////////////
	// PID입력은 항상 양수 이어야 합니다. 따라서 절댓값 함수 적용.
	Input_X = abs(Degree_X);
	Input_Y = abs(Degree_Y); //PID 입력(절댓값)

        double gap_X = Setpoint_X - Input_X;
        double gap_Y = Setpoint_Y - Input_Y;
 
        if (gap_X < gap) //오차각도가 작을때의 PID게인 설정
          myPID_X.SetTunings(consKp, consKi, consKd);
        else //오차각도가 클 때의 PID게인 설정
           myPID_X.SetTunings(aggKp, aggKi, aggKd); 
             
        if(gap_Y < gap)
           myPID_Y.SetTunings(consKp, consKi, consKd);
        else
           myPID_Y.SetTunings(aggKp, aggKi, aggKd); 

	myPID_X.Compute(); //PID 계산 수행
	myPID_Y.Compute(); 
	//myPID_Z.Compute(); 

//////////////////////////////////////////////////////////////////////
///                       loop  MOTOR                              ///
//////////////////////////////////////////////////////////////////////
        // map함수: 0~255까지의 입력을 0부터swing값 까지 매핑해줌
	float M1 = map(Output_X,0,255,0,swing)/10.00, 
	      M2 = map(Output_X,0,255,0,swing)/10.00,
              M3 = map(Output_Y,0,255,0,swing)/10.00, 
              M4 = map(Output_Y,0,255,0,swing)/10.00;

	if(  Degree_X < 0 && Degree_Y < 0 ) // -, -
	{     
		Speed_Motor_1 = standard_1 + ( M1 );      
		Speed_Motor_2 = standard_2 - ( M2 );
		Speed_Motor_3 = standard_3 + ( M3 );
		Speed_Motor_4 = standard_4 - ( M4 );
	}

	else if(  Degree_X > 0 && Degree_Y > 0 ) // +, +
	{    
		Speed_Motor_1 = standard_1 - ( M1 );      
		Speed_Motor_2 = standard_2 + ( M2 );
		Speed_Motor_3 = standard_3 - ( M3 );
		Speed_Motor_4 = standard_4 + ( M4 );
	}

	else if(  Degree_X > 0 && Degree_Y < 0 ) // +, -
	{     
		Speed_Motor_1 = standard_1 - ( M1 );      
		Speed_Motor_2 = standard_2 + ( M2 );
		Speed_Motor_3 = standard_3 + ( M3 );
		Speed_Motor_4 = standard_4 - ( M4 );
	}

	else if(  Degree_X < 0 && Degree_Y > 0 ) // -, +
	{      
		Speed_Motor_1 = standard_1 + ( M1 );      
		Speed_Motor_2 = standard_2 - ( M2 );
		Speed_Motor_3 = standard_3 - ( M3 );
		Speed_Motor_4 = standard_4 + ( M4 );
	}

        int thro = 0; //스로틀
        thro = map(analogRead(1),0,1024,0,170);
        
        //캘리브레이션이 안된 것인지 저는 3번 모터가 늦게 돌기 시작합니다. 
        //그래서 영점조절을 했습니다.
        if( analogRead(1) ) //영점맞추기
        {
          standard_1 = 0;
          standard_2 = 0;
          standard_3 = 22;
          standard_4 = 0;
        }
        else
        {
          standard_1 = 0;
          standard_2 = 0;
          standard_3 = 0;
          standard_4 = 0;
        }
 
	//모터 출력
	motor_1.write(Speed_Motor_1 + thro);
	motor_2.write(Speed_Motor_2 + thro);  
	motor_3.write(Speed_Motor_3 + thro);
	motor_4.write(Speed_Motor_4 + thro); 

//////////////////////////////////////////////////////////////////////
///                       loop  Serial                             ///
//////////////////////////////////////////////////////////////////////

	Serial.print(thro); Serial.print(F(" |\t"));//스로틀 값

	Serial.print(F("Degree : ")); //기체 각도
	Serial.print(Degree_X);  Serial.print(F(", "));
	Serial.print(Degree_Y);	  //Serial.print(F(", "));
	//Serial.print(Degree_Z);	  
	Serial.print(F(" |\t"));

	Serial.print("M1,M2 : "); //1,2번 모터 출력
	Serial.print(Speed_Motor_1 + thro);  Serial.print(F(", "));
	Serial.print(Speed_Motor_2 + thro);  Serial.print(F(" |\t"));

	Serial.print("M3,M4 : "); //3,4번 모터 출력
	Serial.print(Speed_Motor_3 + thro);  Serial.print(F(", "));
	Serial.println(Speed_Motor_4 + thro);
	prevSensoredTime = curSensoredTime;

}	// End of loop()




