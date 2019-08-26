
void sensor(){
	int error;
	double dT;
	accel_t_gyro_union accel_t_gyro;
	
 	curSensoredTime = millis();

//	Serial.println(F(""));
//	Serial.println(F("MPU-6050"));
	
	// Read the raw values.
	// Read 14 bytes at once,
	// containing acceleration, temperature and gyro.
	// With the default settings of the MPU-6050,
	// there is no filter enabled, and the values
	// are not very stable.
	error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
	if(error != 0) {
		Serial.print(F("Read accel, temp and gyro, error = "));
		Serial.println(error,DEC);
	}
	// Swap all high and low bytes.
	// After this, the registers values are swapped,
	// so the structure name like x_accel_l does no
	// longer contain the lower byte.
	uint8_t swap;
	#define SWAP(x,y) swap = x; x = y; y = swap
	SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
	SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
	SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
	SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
	SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
	SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
	SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

	if(prevSensoredTime > 0) {
		int gx1=0, gy1=0, gz1 = 0;
		float gx2=0, gy2=0, gz2 = 0;

		int loopTime = curSensoredTime - prevSensoredTime;
                
//                Serial.print("loop time = ");
//                Serial.println(loopTime);
                
		gx2 = angleInDegrees(lowX, highX, accel_t_gyro.value.x_gyro);
		gy2 = angleInDegrees(lowY, highY, accel_t_gyro.value.y_gyro);
		gz2 = angleInDegrees(lowZ, highZ, accel_t_gyro.value.z_gyro);

		predict(&angX, gx2, loopTime);
		predict(&angY, gy2, loopTime);
		predict(&angZ, gz2, loopTime);

		gx1 = update(&angX, accel_t_gyro.value.x_accel) / 10;
		gy1 = update(&angY, accel_t_gyro.value.y_accel) / 10;
		gz1 = update(&angZ, accel_t_gyro.value.z_accel) / 10;

		/////////////////////////////////////////////////////////////////////////////
		//  ���� ����� �� �����Ǵ� n���� ���� ��� => ���� �����Ǵ� ���� ����
		/////////////////////////////////////////////////////////////////////////////
		if(initIndex < initSize) {
			xInit[initIndex] = gx1;
			yInit[initIndex] = gy1;
			zInit[initIndex] = gz1;
			if(initIndex == initSize - 1) {
				int sumX = 0; int sumY = 0; int sumZ = 0;
				for(int k=1; k <= initSize; k++) {
					sumX += xInit[k];
					sumY += yInit[k];
					sumZ += zInit[k];
				}

				xCal -= sumX/(initSize -1);
				yCal -= sumY/(initSize -1);
				zCal = (sumZ/(initSize -1) - zCal);
			}
			initIndex++;
		}
		
		else {
			// ������ ����
  			gx1 += xCal;
  			gy1 += yCal;
  			//gz1 += zCal;
  
			// �������� ���� �ʿ��� ó���� ����
			// if(gz1 < 1400 && -250 < gy1 && gy1 < 250 && gx1 < 500) {
			//	Serial.print(F("Turn right"));
			//	Serial.println(F(""));
			//}

		}
                // 삼각함수를 이용하여 각도 계산
//              	Serial.print(F("offset_X : ")); //기체 각도
//              	Serial.print(offset_X); //기체 각도
                Degree_X = Floor(atan2(gx1,gz1) * 57.2957786 - offset_X); //offset
                Degree_Y = Floor(atan2(gy1,gz1) * 57.2957786 - offset_Y);   
                Degree_Z = gz1;
      }//end if 

}

float Floor(float num){ //소숫점 2자리 이하 버림
  num = floor(num*10)/10;
  return num;
}

/**************************************************
 * Sensor read/write
 **************************************************/
int MPU6050_read(int start, uint8_t *buffer, int size)
{
	int i, n, error;
	
	Wire.beginTransmission(MPU6050_I2C_ADDRESS);
	
	n = Wire.write(start);
	if (n != 1)
		return (-10);
	
	n = Wire.endTransmission(false); // hold the I2C-bus
	if (n != 0)
		return (n);
	
	// Third parameter is true: relase I2C-bus after data is read.
	Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
	i = 0;
	while(Wire.available() && i<size)
	{
		buffer[i++]=Wire.read();
	}
	if ( i != size)
		return (-11);
	return (0); // return : no error
}

int MPU6050_write(int start, const uint8_t *pData, int size)
{
	int n, error;
	
	Wire.beginTransmission(MPU6050_I2C_ADDRESS);
	
	n = Wire.write(start); // write the start address
	if (n != 1)
		return (-20);
		
	n = Wire.write(pData, size); // write data bytes
	if (n != size)
		return (-21);
		
	error = Wire.endTransmission(true); // release the I2C-bus
	if (error != 0)
		return (error);
	return (0); // return : no error
}

int MPU6050_write_reg(int reg, uint8_t data)
{
	int error;
	error = MPU6050_write(reg, &data, 1);
	return (error);
}
