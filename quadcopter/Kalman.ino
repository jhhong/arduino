
/**************************************************
 * Raw data processing
 **************************************************/
float angleInDegrees(int lo, int hi, int measured) {
	float x = (hi - lo)/180.0;
	return (float)measured/x;
}

void initGyroKalman(struct GyroKalman *kalman, const float Q_angle, const float Q_gyro, const float R_angle) {
	kalman->Q_angle = Q_angle;
	kalman->Q_gyro = Q_gyro;
	kalman->R_angle = R_angle;
	
	kalman->P_00 = 0;
	kalman->P_01 = 0;
	kalman->P_10 = 0;
	kalman->P_11 = 0;
}

/*
* The kalman predict method.
* kalman 		the kalman data structure
* dotAngle 		Derivitive Of The (D O T) Angle. This is the change in the angle from the gyro.
* 					This is the value from the Wii MotionPlus, scaled to fast/slow.
* dt 				the change in time, in seconds; in other words the amount of time it took to sweep dotAngle
*/
void predict(struct GyroKalman *kalman, float dotAngle, float dt) {
	kalman->x_angle += dt * (dotAngle - kalman->x_bias);
	kalman->P_00 += -1 * dt * (kalman->P_10 + kalman->P_01) + dt*dt * kalman->P_11 + kalman->Q_angle;
	kalman->P_01 += -1 * dt * kalman->P_11;
	kalman->P_10 += -1 * dt * kalman->P_11;
	kalman->P_11 += kalman->Q_gyro;
}

/*
* The kalman update method
* kalman 	the kalman data structure
* angle_m 	the angle observed from the Wii Nunchuk accelerometer, in radians
*/
float update(struct GyroKalman *kalman, float angle_m) {
	const float y = angle_m - kalman->x_angle;
	const float S = kalman->P_00 + kalman->R_angle;
	const float K_0 = kalman->P_00 / S;
	const float K_1 = kalman->P_10 / S;
	kalman->x_angle += K_0 * y;
	kalman->x_bias += K_1 * y;
	kalman->P_00 -= K_0 * kalman->P_00;
	kalman->P_01 -= K_0 * kalman->P_01;
	kalman->P_10 -= K_1 * kalman->P_00;
	kalman->P_11 -= K_1 * kalman->P_01;
	return kalman->x_angle;
}
