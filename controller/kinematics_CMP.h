/*
    Kinematics implementation using first order complementary filter by cTn
    
    Extra:
    1. accelerometer data normalization
    2. accelerometer data cut-off
    3. accelerometer angle overflow/flip handling

    Kinematics input is averaged (from multiple samples) and scaled (gyro is in radians/s) and accel is in m/s^2
    accel measurement is normalized before any angles are computed.
*/

unsigned long kinematics_timer;

void kinematics_update(double* accelX, double* accelY, double* accelZ, double* gyroX, double* gyroY, double* gyroZ) {

    // Normalize accel values
    double norm = sqrt(*accelX * *accelX + *accelY * *accelY + *accelZ * *accelZ);
    *accelX /= norm;
    *accelY /= norm;
    *accelZ /= norm;
    
    // Determinate Sensor orientation
    bool orientation = true; // up-side UP
    if (*accelZ < 0.00) orientation = false; // up-side DOWN    
    
    // Standard angle calculation
    double accelXangle = atan2(*accelY, *accelZ);
    double accelYangle = atan2(*accelX, *accelZ);    
    
    // Trigonometric equation to calculate roll and pitch angles (constant sensitivity over 360deg rotation)
    // double accelXangle = atan(*accelY / sqrt(*accelX * *accelX + *accelZ * *accelZ));
    // double accelYangle = atan(*accelX / sqrt(*accelY * *accelY + *accelZ * *accelZ));
    
    // Accelerometer cut-off
    double accelWeight = 0.0025; // normal operation
    if (norm > 12.0) accelWeight = 0.00; // gyro only
    
    // Save current time into variable for better computation time
    unsigned long now = micros();    
    
    // Fuse in gyroscope
    kinematicsAngleX = kinematicsAngleX + (*gyroX * (double)(now - kinematics_timer) / 1000000);
    kinematicsAngleY = kinematicsAngleY + (*gyroY * (double)(now - kinematics_timer) / 1000000);
    kinematicsAngleZ = kinematicsAngleZ + (*gyroZ * (double)(now - kinematics_timer) / 1000000);  
    
    // Normalize gyro kinematics (+ - PI)
    if (kinematicsAngleX > PI) kinematicsAngleX -= 2 * PI;
    else if (kinematicsAngleX < -PI) kinematicsAngleX += 2 * PI;    
    
    if (kinematicsAngleY > PI) kinematicsAngleY -= 2 * PI;
    else if (kinematicsAngleY < -PI) kinematicsAngleY += 2 * PI;    
    
    // Fuse in accel (handling accel flip)
    if ((kinematicsAngleX - accelXangle) > PI) {
        kinematicsAngleX = (1.00 - accelWeight) * kinematicsAngleX + accelWeight * (accelXangle + 2 * PI);
    } else if ((kinematicsAngleX - accelXangle) < -PI) {
        kinematicsAngleX = (1.00 - accelWeight) * kinematicsAngleX + accelWeight * (accelXangle - 2 * PI);
    } else {
        kinematicsAngleX = (1.00 - accelWeight) * kinematicsAngleX + accelWeight * accelXangle;
    }
    
    if ((kinematicsAngleY - accelYangle) > PI) {
        kinematicsAngleY = (1.00 - accelWeight) * kinematicsAngleY + accelWeight * (accelYangle + 2 * PI);
    } else if ((kinematicsAngleY - accelYangle) < -PI) {
        kinematicsAngleY = (1.00 - accelWeight) * kinematicsAngleY + accelWeight * (accelYangle - 2 * PI);
    } else {
        kinematicsAngleY = (1.00 - accelWeight) * kinematicsAngleY + accelWeight * accelYangle;
    }    
    
    // Saves time for next comparison
    kinematics_timer = now;  
}