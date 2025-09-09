# Real-Time Multi-Sensor Monitoring System

This project integrates multiple sensors on Arduino to create a **real-time monitoring system** with an LCD display. The sensors include:

* **Ultrasonic Sensors (HC-SR04)** for distance measurement
* **MPU6050 Accelerometer & Gyroscope** for orientation and movement
* **PIR Sensor** for motion detection
* **LDR (Light Dependent Resistor)** for ambient light detection
* **RTC DS1307** for real-time clock functionality
* **LCD Display (I2C)** for output

The system also incorporates **error minimisation mechanisms** such as weighted averaging and (optionally) a Kalman filter to improve sensor accuracy.

---

## Code Overview

### 1. **Ultrasonic Distance Measurement**

* Two ultrasonic sensors are used: one short-range and one long-range.
* Logic selects the appropriate reading depending on measured time:

  * Short range for objects closer than \~100 cm
  * Long range for farther distances
  * A **weighted average** between the two in the overlapping range

This approach reduces **measurement error due to dead zones**.

```cpp
if(timeshort <= 5800) {
   return timeshort / 58;
}
else if(timeshort >= 5800 && timelong <= 8700) {
   return (0.4*timeshort + 0.6*timelong) / 58;
}
else {
   return timelong / 58;
}
```

---

### 2. **Accelerometer (MPU6050)**

* Raw values from the gyroscope/accelerometer are mapped to the 0–255 range.
* Z-axis values are displayed to track tilt or motion.

```cpp
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
data.Z = map(gz, -17000, 17000, 0, 255);
```

---

### 3. **PIR Motion + LDR Light Sensor**

* PIR detects motion (`PRESENT`/`ABSENT`).
* LDR reads light intensity and also adjusts an LED’s brightness accordingly.

```cpp
b = analogRead(A0);
analogWrite(5, 255 - b); // Adjust torch brightness
```

---

### 4. **RTC Module**

* Real-time clock displays current time on the LCD.
* Time is also printed to the serial monitor for debugging.

---

### 5. **LCD Display**

* Displays distance, Z-axis accelerometer value, PIR status, time, and LDR readings in real-time.

```cpp
lcd.setCursor(0, 0);
lcd.print("Distance ");
lcd.print(ultrasensor());
```

---

## Error Minimisation Mechanisms

### 1. **Weighted Averaging (Implemented)**

* For ultrasonic sensors, a weighted average between short and long range improves accuracy in the mid-range zone.

### 2. **Smoothing via Kalman Filter (Proposed)**

* Ultrasonic and accelerometer readings are prone to noise.
* A **Kalman filter** can be applied to reduce fluctuations:

  * Predicts the next state of the sensor value
  * Corrects it using the measured value
  * Produces a smoothed estimate with reduced error

### Example Kalman Filter Pseudocode

```cpp
float estimate = 0;
float error_estimate = 1;
float error_measure = 1;
float kalman_gain;

float kalmanFilter(float measurement) {
    // Prediction update
    error_estimate += 0.01; // process noise

    // Measurement update
    kalman_gain = error_estimate / (error_estimate + error_measure);
    estimate = estimate + kalman_gain * (measurement - estimate);
    error_estimate = (1 - kalman_gain) * error_estimate;

    return estimate;
}
```

This filter can wrap around `ultrasensor()` or `acclerometer()` outputs.

---

## Future Improvements

* Implement Kalman filter directly in the code for accelerometer and ultrasonic data.
* Add averaging buffer (moving average) for PIR and LDR to reduce false triggers.
* Enable SD card logging for long-term data collection.

---

## Applications

* Smart surveillance systems
* Robotics navigation
* Environment monitoring
* IoT-based home automation

---

## Authors

* **Shivanshu Aryan**  (IIT Delhi, COL106 Assignment)
