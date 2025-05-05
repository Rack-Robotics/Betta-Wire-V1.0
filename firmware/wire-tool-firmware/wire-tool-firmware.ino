/*
 * Rack Robotics, Inc.
 * Wire Tool Firmware - Velocity Control Implementation
 * For use with Rarduino-Pico Plugin by EarlePhilhower
 * 
 * Dependencies: Wire.h (built-in)
 * Hardware: RP2040, AS5600, DRV8871, ACS723
 * 
 */

#include <Wire.h>

// Pin Definitions
const int I2C_SDA                             = 4;                                                    // I2C Data for AS5600 encoder
const int I2C_SCL                             = 5;                                                    // I2C Clock for AS5600 encoder
const int TOP_MOTOR_DRV8871_IN1               = 19;                                                   // Motor driver IN1 for top tool motor
const int TOP_MOTOR_DRV8871_IN2               = 18;                                                   // Motor driver IN2 for top tool motor
const int BOTTOM_MOTOR_DRV8871_IN1            = 17;                                                   // Motor driver IN1 for bottom tool motor
const int BOTTOM_MOTOR_DRV8871_IN2            = 16;                                                   // Motor driver IN2 for bottom tool motor
const int STATUS_LED                          = 25;                                                   // Status LED on Pico
const int TOP_MOTOR_ACS723                    = 26;                                                   // Analog output of current sensor for top tool motor
const int BOTTOM_MOTOR_ACS723                 = 27;                                                   // Analog output of current sensor for bottom tool motor

// Safety Limits
const float MOTOR_VOLTAGE                     = 9;                                                    // Votage applied to motors from driver (volts DC)
const float MAX_TOP_MOTOR_POWER               = 9;                                                    // Maximum safe power for top motor (watts)
const float MAX_BOTTOM_MOTOR_POWER            = 9;                                                    // Maximum safe power for bottom motor (watts)
const float MAX_VELOCITY                      = 50.0;                                                 // Maximum allowed velocity in mm/s
const float MIN_VELOCITY                      = -50.0;                                                // Minimum allowed velocity in mm/s
const float MAX_CURRENT                       = 2.0;                                                  // Maximum allowed current in A
const float MIN_CURRENT                       = -2.0;                                                 // Minimum allowed current in A
const float MAX_VELOCITY_ERROR                = 20.0;                                                 // Maximum allowed velocity error in mm/s
const float MAX_CURRENT_ERROR                 = 500.0;                                                // Maximum allowed current error in mA
const int ERROR_CHECK_INTERVAL                = 100;                                                  // Check errors every 

// Command String Definitions
const String COMMAND_SET_TOP_MOTOR_PWM        = "WIRETOOL: SET_TOP_MOTOR_PWM";
const String COMMAND_SET_BOTTOM_MOTOR_PWM     = "WIRETOOL: SET_BOTTOM_MOTOR_PWM";
const String COMMAND_LOAD_WIRE                = "WIRETOOL: LOAD_WIRE";
const String COMMAND_EXIT_WIRE_LOADING        = "WIRETOOL: EXIT_WIRE_LOADING";
const String COMMAND_UNLOAD_WIRE              = "WIRETOOL: UNLOAD_WIRE";
const String COMMAND_CALIBRATE_WIRE_TENSION   = "WIRETOOL: CALIBRATE_WIRE_TENSION";
const String COMMAND_SET_WIRE_VELOCITY        = "WIRETOOL: SET_WIRE_VELOCITY";
const String COMMAND_SET_WIRE_TENSION         = "WIRETOOL: SET_WIRE_TENSION";
const String COMMAND_SET_KP                   = "WIRETOOL: SET_KP";
const String COMMAND_SET_KI                   = "WIRETOOL: SET_KI";
const String COMMAND_SET_KD                   = "WIRETOOL: SET_KD";
const String COMMAND_GET_PID                  = "WIRETOOL: GET_PID";
const String COMMAND_TEST_ERROR               = "WIRETOOL: TEST_ERROR";
const String COMMAND_RESET                    = "WIRETOOL: RESET";
const String COMMAND_GET_TELEMETRY            = "WIRETOOL: GET_TELEMETRY";

// Current sensor calibration
const int CALIBRATION_SAMPLES = 100;  // Number of samples to average for calibration
float topVoltageOffset = 0.0;         // Zero-current voltage offset for top motor
float bottomVoltageOffset = 0.0;      // Zero-current voltage offset for bottom motor
const float CURRENT_SENSOR_SENSITIVITY = 0.400;  // V/Amp

// Current sensing constants and variables
const int CURRENT_AVERAGE_SAMPLES = 10;  // Number of samples for moving average
float topCurrentReadings[CURRENT_AVERAGE_SAMPLES] = {0};    // Array for top motor current readings
float bottomCurrentReadings[CURRENT_AVERAGE_SAMPLES] = {0}; // Array for bottom motor current readings
int currentReadingIndex = 0;  // Index for circular buffer

// AS5600 Encoder Constants
const uint8_t AS5600_ADDR             = 0x36;                                                         // I2C address of AS5600 magnetic encoder
const uint8_t REG_RAW_ANGLE_H         = 0x0C;                                                         // High byte register for raw angle
const uint8_t REG_RAW_ANGLE_L         = 0x0D;                                                         // Low byte register for raw angle
const float WHEEL_DIAMETER            = 30.0;                                                         // Diameter of measurement wheel in mm
const float MM_PER_REVOLUTION         = PI * WHEEL_DIAMETER;                                          // Linear distance per revolution in mm
const int SAMPLE_SIZE                 = 50;                                                           // Number of samples in moving average window

// Motor Control Constants
const int MIN_TOP_MOTOR_PWM = 0;                                                                      // Minimum PWM duty cycle value for top motor
const int MAX_TOP_MOTOR_PWM = 4095 * (MAX_TOP_MOTOR_POWER / MOTOR_VOLTAGE);                           // Maximum PWM duty cycle value for top motor, calculated at max power for motor
const int MIN_BOTTOM_MOTOR_PWM = 0;                                                                   // Minumum PWM duty cycle value for bottom motor
const int MAX_BOTTOM_MOTOR_PWM = 4095 * (MAX_BOTTOM_MOTOR_POWER / MOTOR_VOLTAGE);                     // Maximum PWM duty cycle value for bottom motor, calculated at max power for motor
int topMotorPWM = 0;     // Current PWM value for top motor
int bottomMotorPWM = 0;  // Current PWM value for bottom motor

// PID Control Variables
float Kp = 400.0;   
float Ki = 0.5;    
float Kd = 1.0;    
float velocityIntegral = 0;
float lastVelocityError = 0;
unsigned long lastVelocityPIDTime = 0;
const float INTEGRAL_MAX = 1000.0;

// Encoder Variables
volatile uint16_t lastPosition = 0;                                                                     // Previous encoder position (0-4095)
volatile float velocities[SAMPLE_SIZE];                                                                 // Circular buffer of velocity measurements
volatile int velocityIndex = 0;                                                                         // Current index in velocity buffer
volatile uint32_t lastSampleTime = 0;                                                                   // Last sample time in microseconds
volatile float targetVelocity = 0;                                                                      // Target velocity for wire

// Wire loading and unloading variables 
bool wireLoadingMode = false;
uint16_t loadingModeLastPosition = 0;
const float LOADING_PWM_SCALE = 500.0;                                                                   // PWM per revolution scaling factor
const int WIRE_LOADING_BOTTOM_TOOL_PWM = MAX_BOTTOM_MOTOR_PWM;

// State Variables
String inputBuffer = "";
bool fatalErrorActive = false;
String errorMessage = "";
bool wireIsLoaded = false;
float targetCurrent = 0;                     // Target motor current for tension control
float actualCurrent = 0;                     // Measured motor current
int safetyCheckCounter = 0;                  // Counter for safety check timing

// Timer Structure
repeating_timer mainTimer;                   // 10ms timer for all periodic tasks

// Telemetry Data Structure
struct Telemetry {
    float targetVelocity;
    float actualVelocity;
    float velocityError;
    float targetCurrent;
    float actualTopCurrent;     
    float actualBottomCurrent;  
    float currentError;
    bool wireStatus;
    bool errorStatus;
    String errorMsg;
    int topPWM;
    int bottomPWM;
} telemetryData;

float readMotorCurrent(int sensorPin, float voltageOffset) {
    // Read raw voltage from current sensor (0-3.3V based on 12-bit ADC)
    float voltage = analogRead(sensorPin) * (3.3 / 4096.0);
    
    // Convert to mA using sensitivity and offset
    return abs(((voltage - voltageOffset) / CURRENT_SENSOR_SENSITIVITY) * 1000.0);
  }

float calculateAverageCurrent(float readings[], int numSamples) {
    float sum = 0;
    for(int i = 0; i < numSamples; i++) {
        sum += readings[i];
    }
    return sum / numSamples;
  }
void updateCurrentReadings() {
    // Read new current values
    topCurrentReadings[currentReadingIndex] = readMotorCurrent(TOP_MOTOR_ACS723, topVoltageOffset);
    bottomCurrentReadings[currentReadingIndex] = readMotorCurrent(BOTTOM_MOTOR_ACS723, bottomVoltageOffset);
    
    // Update circular buffer index
    currentReadingIndex = (currentReadingIndex + 1) % CURRENT_AVERAGE_SAMPLES;
  }
void calibrateCurrentSensors() {
    float topSum = 0;
    float bottomSum = 0;
    
    Serial.println("Calibrating current sensors...");
    
    // Ensure motors are off
    applyTopMotorPWM(0);
    applyBottomMotorPWM(0);
    
    // Small delay to ensure motors are stopped
    delay(1000);
    
    // Take multiple readings and average them
    for(int i = 0; i < CALIBRATION_SAMPLES; i++) {
        topSum += analogRead(TOP_MOTOR_ACS723) * (3.3 / 4096.0);
        bottomSum += analogRead(BOTTOM_MOTOR_ACS723) * (3.3 / 4096.0);
        delay(10);  // Short delay between readings
    }
    
    // Calculate average offsets
    topVoltageOffset = topSum / CALIBRATION_SAMPLES;
    bottomVoltageOffset = bottomSum / CALIBRATION_SAMPLES;
    
    //Serial.print("Top current sensor offset: ");
    //Serial.print(topVoltageOffset, 3);
    //Serial.println("V");
    //Serial.print("Bottom current sensor offset: ");
    //Serial.print(bottomVoltageOffset, 3);
    //Serial.println("V");
  }
void applyTopMotorPWM(int pwmValue) { // Function to apply PWM value to top motor

    // Constrain PWM value within safe limits
    int safePWM;
    
    if (pwmValue > 0) {
        // Forward direction
        safePWM = min(pwmValue, MAX_TOP_MOTOR_PWM);
        safePWM = max(safePWM, MIN_TOP_MOTOR_PWM);
        analogWrite(TOP_MOTOR_DRV8871_IN1, safePWM);
        analogWrite(TOP_MOTOR_DRV8871_IN2, 0);
        topMotorPWM = safePWM;
    } 
    else if (pwmValue < 0) {
        // Reverse direction
        safePWM = min(-pwmValue, MAX_TOP_MOTOR_PWM);
        safePWM = max(safePWM, MIN_TOP_MOTOR_PWM);
        analogWrite(TOP_MOTOR_DRV8871_IN1, 0);
        analogWrite(TOP_MOTOR_DRV8871_IN2, safePWM);
        topMotorPWM = -safePWM;
    }
    else {
        // Stop
        analogWrite(TOP_MOTOR_DRV8871_IN1, 0);
        analogWrite(TOP_MOTOR_DRV8871_IN2, 0);
        topMotorPWM = 0;
    }
  }
void applyBottomMotorPWM(int pwmValue) { // Function to apply PWM value to bottom motor

    // Constrain PWM value within safe limits
    int safePWM;
    
    if (pwmValue > 0) {
        // Forward direction
        safePWM = min(pwmValue, MAX_BOTTOM_MOTOR_PWM);
        safePWM = max(safePWM, MIN_BOTTOM_MOTOR_PWM);
        analogWrite(BOTTOM_MOTOR_DRV8871_IN1, safePWM);
        analogWrite(BOTTOM_MOTOR_DRV8871_IN2, 0);
        bottomMotorPWM = safePWM;
    }
    else if (pwmValue < 0) {
        // Reverse direction
        safePWM = min(-pwmValue, MAX_BOTTOM_MOTOR_PWM);
        safePWM = max(safePWM, MIN_BOTTOM_MOTOR_PWM);
        analogWrite(BOTTOM_MOTOR_DRV8871_IN1, 0);
        analogWrite(BOTTOM_MOTOR_DRV8871_IN2, safePWM);
        bottomMotorPWM = -safePWM;
    }
    else {
        // Stop
        analogWrite(BOTTOM_MOTOR_DRV8871_IN1, 0);
        analogWrite(BOTTOM_MOTOR_DRV8871_IN2, 0);
        bottomMotorPWM = 0;
    }
  }







uint16_t readRawAngle() { // Read raw angle from AS5600 encoder
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(REG_RAW_ANGLE_H);
    Wire.endTransmission(false);
    Wire.requestFrom(AS5600_ADDR, 2);
    
    if(Wire.available() <= 2) {
        uint16_t high = Wire.read();
        uint16_t low = Wire.read();
        return (high << 8) | low;
    }
    return 0;
  }

float calculateAverageVelocity() { // Calculate average velocity from samples
    float sum = 0;
    for(int i = 0; i < SAMPLE_SIZE; i++) {
        sum += velocities[i];
    }
    return sum / SAMPLE_SIZE;
  }

void triggerFatalError(String message) { // Handle fatal errors
  fatalErrorActive = true;
  errorMessage = message;
    
  // Stop all motors
  analogWrite(TOP_MOTOR_DRV8871_IN1, 0);
  analogWrite(TOP_MOTOR_DRV8871_IN2, 0);
  analogWrite(BOTTOM_MOTOR_DRV8871_IN1, 0);
  analogWrite(BOTTOM_MOTOR_DRV8871_IN2, 0);
    
  digitalWrite(STATUS_LED, LOW);
  Serial.print("FATAL ERROR: ");
  Serial.println(message);
  }

bool checkSafetyLimits() { // Check safety limits
    // Check if values are within allowed ranges
    if(telemetryData.targetVelocity > MAX_VELOCITY || telemetryData.targetVelocity < MIN_VELOCITY) {
        char errorMsg[50];
        snprintf(errorMsg, sizeof(errorMsg), "Target velocity out of range: %.2f mm/s", telemetryData.targetVelocity);
        triggerFatalError(errorMsg);
        return false;
    }
    
    if(telemetryData.targetCurrent > MAX_CURRENT || telemetryData.targetCurrent < MIN_CURRENT) {
        char errorMsg[50];
        snprintf(errorMsg, sizeof(errorMsg), "Target current out of range: %.2f mA", telemetryData.targetCurrent);
        triggerFatalError(errorMsg);
        return false;
    }
    
    // Check for excessive errors
    float velocityError = abs(telemetryData.velocityError);
    if(velocityError > MAX_VELOCITY_ERROR && telemetryData.targetVelocity != 0) {
        char errorMsg[50];
        snprintf(errorMsg, sizeof(errorMsg), "Velocity error too high: %.2f mm/s", velocityError);
        triggerFatalError(errorMsg);
        return false;
    }
    
    float currentError = abs(telemetryData.currentError);
    if(currentError > MAX_CURRENT_ERROR && telemetryData.targetCurrent != 0) {
        char errorMsg[50];
        snprintf(errorMsg, sizeof(errorMsg), "Current error too high: %.2f mA", currentError);
        triggerFatalError(errorMsg);
        return false;
    }
    
    return true;
  }

bool getTelemetryCallback(struct repeating_timer *t) {
   // Velocity telemetry
   telemetryData.targetVelocity = targetVelocity;
   telemetryData.actualVelocity = calculateAverageVelocity();
   telemetryData.velocityError = targetVelocity - telemetryData.actualVelocity;

   // Current telemetry
   telemetryData.targetCurrent = targetCurrent;
   
   // Read raw voltage from current sensors (0-3.3V based on 12-bit ADC)
   float topVoltage = analogRead(TOP_MOTOR_ACS723) * (3.3 / 4096.0);
   float bottomVoltage = analogRead(BOTTOM_MOTOR_ACS723) * (3.3 / 4096.0);


  // Get motor 
  telemetryData.actualTopCurrent = calculateAverageCurrent(topCurrentReadings, CURRENT_AVERAGE_SAMPLES);
  telemetryData.actualBottomCurrent = calculateAverageCurrent(bottomCurrentReadings, CURRENT_AVERAGE_SAMPLES);
  telemetryData.currentError = abs(targetCurrent - telemetryData.actualBottomCurrent);

   // Status telemetry
   telemetryData.wireStatus = wireIsLoaded;
   telemetryData.errorStatus = fatalErrorActive;
   telemetryData.errorMsg = errorMessage;
   
   // Motor PWM telemetry
   telemetryData.topPWM = topMotorPWM;
   telemetryData.bottomPWM = bottomMotorPWM;

  //Serial.print("Raw ADC: "); 
  //Serial.println(analogRead(BOTTOM_MOTOR_ACS723));
  //Serial.print("Voltage: ");
  //Serial.println(bottomVoltage, 4);
  //Serial.print("Offset: ");
  //Serial.println(bottomVoltageOffset, 4);

   return true;
  }

void outputTelemetry() {
    Serial.println("TELEMETRY_BEGIN");
    
    Serial.print("Target Velocity: ");
    Serial.print(telemetryData.targetVelocity, 2);
    Serial.println(" mm/s");
    Serial.print("Actual Velocity: ");
    Serial.print(telemetryData.actualVelocity, 2);
    Serial.println(" mm/s");
    Serial.print("Velocity Error: ");
    Serial.print(telemetryData.velocityError, 2);
    Serial.println(" mm/s");
    
    Serial.print("Target Current: ");
    Serial.print(telemetryData.targetCurrent, 2);
    Serial.println(" mA");
    Serial.print("Top Motor Current: ");       // Add this line
    Serial.print(telemetryData.actualTopCurrent, 2);  // Add this line
    Serial.println(" mA");                     // Add this line
    Serial.print("Bottom Motor Current: ");    // Modified line
    Serial.print(telemetryData.actualBottomCurrent, 2);
    Serial.println(" mA");
    Serial.print("Current Error: ");
    Serial.print(telemetryData.currentError, 2);
    Serial.println(" mA");
    
    Serial.print("Top Motor PWM: ");
    Serial.println(telemetryData.topPWM);
    Serial.print("Bottom Motor PWM: ");
    Serial.println(telemetryData.bottomPWM);
    
    Serial.print("Wire Status: ");
    Serial.println(telemetryData.wireStatus ? "LOADED" : "UNLOADED");
    Serial.print("Error Status: ");
    Serial.println(telemetryData.errorStatus ? telemetryData.errorMsg : "NO ERROR");
    
    Serial.println("TELEMETRY_END");
  }

bool mainTimerCallback(struct repeating_timer *t) { // Combined callback for all periodic tasks
    // Position sampling
    uint32_t currentTime = micros();
    uint16_t currentPosition = readRawAngle();
    
    int16_t deltaPos = currentPosition - lastPosition;
    if(deltaPos < -2048) deltaPos += 4096;  // Handle rollover in positive direction
    if(deltaPos > 2048) deltaPos -= 4096;   // Handle rollover in negative direction

    // Add debug prints
    //Serial.print("Position: ");
    //Serial.print(currentPosition);
    //Serial.print(" Delta: ");
    //Serial.println(deltaPos);
    
    
    if (abs(deltaPos) < 2) {  // Noise threshold
        velocities[velocityIndex] = 0;
        velocityIndex = (velocityIndex + 1) % SAMPLE_SIZE;
        lastPosition = currentPosition;
        lastSampleTime = currentTime;
    } else {
        float timeElapsed = (currentTime - lastSampleTime) / 1000000.0;  // Convert to seconds
        if (timeElapsed >= 0.001) {  // Minimum time threshold
            float rotationFraction = deltaPos / 4096.0;
            float linearDistance = rotationFraction * MM_PER_REVOLUTION;
            velocities[velocityIndex] = linearDistance / timeElapsed;
            velocityIndex = (velocityIndex + 1) % SAMPLE_SIZE;
            lastPosition = currentPosition;
            lastSampleTime = currentTime;
            
            // Add debug print
            //Serial.print("Calculated velocity: ");
            //Serial.println(velocities[velocityIndex]);
        }
    }
    // Update current readings
    updateCurrentReadings();
    // Telemetry collection
    getTelemetryCallback(t);
    // Will run if device is in wire loading mode
    updateLoadingMode();

    // Safety check every ERROR_CHECK_INTERVAL ms
    safetyCheckCounter++;
    if (safetyCheckCounter >= (ERROR_CHECK_INTERVAL / 10)) {
        safetyCheckCounter = 0;
        if (!fatalErrorActive) {
            checkSafetyLimits();
        }
    }

    return true;
  }
void updateLoadingMode() {
    if (!wireLoadingMode) return;
    
    // Get the averaged bottom motor current
    float bottomCurrent = calculateAverageCurrent(bottomCurrentReadings, CURRENT_AVERAGE_SAMPLES);
    
    Serial.println(bottomCurrent);

    if (bottomCurrent > 200.0) {
        wireLoadingMode = false;
        wireIsLoaded = true;  // Wire is now loaded
        applyTopMotorPWM(0);  // Stop top motor
        applyBottomMotorPWM(0); // Stop bottom motor
        Serial.println("Wire detected - exiting loading mode");
        return;
    }
    
    //enable bottom motor to catch wire
    applyBottomMotorPWM(WIRE_LOADING_BOTTOM_TOOL_PWM);
    
    uint16_t currentPosition = readRawAngle();
    int16_t deltaPos = currentPosition - loadingModeLastPosition;
    
    // Handle rollover
    if(deltaPos < -2048) deltaPos += 4096;
    if(deltaPos > 2048) deltaPos -= 4096;
    
    // Calculate motor PWM based on encoder movement
    if (abs(deltaPos) > 2) {  // Noise threshold
        int pwmValue = (int)(deltaPos * LOADING_PWM_SCALE);
        applyTopMotorPWM(pwmValue);
        loadingModeLastPosition = currentPosition;
    } else {
        applyTopMotorPWM(0);  // Stop if no significant movement
    }
  }


void processCommand(String command) { // Process serial commands
    Serial.print("Received command: '");
    Serial.print(command);
    Serial.println("'");

    if (command.startsWith(COMMAND_LOAD_WIRE)) {
      if(!fatalErrorActive) {
        wireLoadingMode = true;
        loadingModeLastPosition = readRawAngle();
        Serial.println("Entering wire loading mode - rotate encoder wheel to feed wire");
      } else {
        Serial.println("ERROR: Cannot load wire while in error state");
      }
      }
      else if (command.startsWith(COMMAND_EXIT_WIRE_LOADING)) {
        if(wireLoadingMode) {
        wireLoadingMode = false;
        applyTopMotorPWM(0);  // Stop top motor
        applyBottomMotorPWM(0); // Stop bottom motor
        Serial.println("Exiting wire loading mode");
        } else {
        Serial.println("Not in wire loading mode");
        }
    }  
    else if (command.startsWith(COMMAND_UNLOAD_WIRE)) {
        if(!fatalErrorActive) {
            // TODO: Implement wire unloading sequence
            Serial.println("Unloading wire...");
            wireIsLoaded = false;
        } else {
            Serial.println("ERROR: Cannot unload wire while in error state");
        }
    }
    else if (command.startsWith(COMMAND_SET_WIRE_VELOCITY)) {
        String parameter = command.substring(COMMAND_SET_WIRE_VELOCITY.length() + 1);
        parameter.trim();
        float velocity = parameter.toFloat();
        
        if(!fatalErrorActive) {
            if(velocity <= MAX_VELOCITY && velocity >= MIN_VELOCITY) {
                targetVelocity = velocity;
                Serial.print("Setting wire velocity to: ");
                Serial.println(velocity, 2);
            } else {
                Serial.println("ERROR: Velocity setpoint out of range");
            }
        } else {
            Serial.println("ERROR: Cannot set velocity while in error state");
        }
    }
    else if (command.startsWith(COMMAND_SET_WIRE_TENSION)) {
        String parameter = command.substring(COMMAND_SET_WIRE_TENSION.length() + 1);
        parameter.trim();
        float tension = parameter.toFloat();
        
        if(!fatalErrorActive) {
            if(tension <= MAX_CURRENT && tension >= MIN_CURRENT) {
                targetCurrent = tension;
                Serial.print("Setting wire tension to: ");
                Serial.println(tension, 2);
            } else {
                Serial.println("ERROR: Tension setpoint out of range");
            }
        } else {
            Serial.println("ERROR: Cannot set tension while in error state");
        }
    }
    else if (command.startsWith(COMMAND_SET_KP)) {
        if(!fatalErrorActive) {
            String parameter = command.substring(COMMAND_SET_KP.length() + 1);
            parameter.trim();
            float newKp = parameter.toFloat();
            if(newKp >= 0) {
                Kp = newKp;
                Serial.print("Setting Kp to: ");
                Serial.println(Kp);
            } else {
                Serial.println("ERROR: Kp must be non-negative");
            }
        } else {
            Serial.println("ERROR: Cannot set PID parameters while in error state");
        }
    }
    else if (command.startsWith(COMMAND_SET_KI)) {
        if(!fatalErrorActive) {
            String parameter = command.substring(COMMAND_SET_KI.length() + 1);
            parameter.trim();
            float newKi = parameter.toFloat();
            if(newKi >= 0) {
                Ki = newKi;
                Serial.print("Setting Ki to: ");
                Serial.println(Ki);
            } else {
                Serial.println("ERROR: Ki must be non-negative");
            }
        } else {
            Serial.println("ERROR: Cannot set PID parameters while in error state");
        }
    }
    else if (command.startsWith(COMMAND_SET_KD)) {
        if(!fatalErrorActive) {
            String parameter = command.substring(COMMAND_SET_KD.length() + 1);
            parameter.trim();
            float newKd = parameter.toFloat();
            if(newKd >= 0) {
                Kd = newKd;
                Serial.print("Setting Kd to: ");
                Serial.println(Kd);
            } else {
                Serial.println("ERROR: Kd must be non-negative");
            }
        } else {
            Serial.println("ERROR: Cannot set PID parameters while in error state");
        }
    }
    else if (command.startsWith(COMMAND_GET_PID)) {
        Serial.print("Kp=");
        Serial.print(Kp);
        Serial.print(" Ki=");
        Serial.print(Ki);
        Serial.print(" Kd=");
        Serial.println(Kd);
    }
    else if (command.startsWith(COMMAND_TEST_ERROR)) {
        String parameter = command.substring(COMMAND_TEST_ERROR.length() + 1);
        parameter.trim();
        if (parameter.length() > 0) {
            triggerFatalError(parameter); // Use custom error message
        } else {
            triggerFatalError("Test error triggered"); // Default message
        }
    }
    else if (command.startsWith(COMMAND_RESET)) {
      Serial.println("Executing reset command...");
  
      // Stop all motors
      analogWrite(TOP_MOTOR_DRV8871_IN1, 0);
      analogWrite(TOP_MOTOR_DRV8871_IN2, 0);
      analogWrite(BOTTOM_MOTOR_DRV8871_IN1, 0);
      analogWrite(BOTTOM_MOTOR_DRV8871_IN2, 0);
    
      // Reset state variables
      fatalErrorActive = false;
      errorMessage = "";
      wireIsLoaded = false;
      targetVelocity = 0;
      targetCurrent = 0;
    
      // Reset PID variables
      velocityIntegral = 0;
      lastVelocityError = 0;
      lastVelocityPIDTime = 0;
    
      // Reset velocity buffer
      for(int i = 0; i < SAMPLE_SIZE; i++) {
        velocities[i] = 0;
      }
    
      // Reset position tracking
      lastPosition = readRawAngle();
      lastSampleTime = micros();
    
      // Reset safety counter
      safetyCheckCounter = 0;

      // Recalibrate current sensors
      calibrateCurrentSensors();
    
      // Turn on status LED
      digitalWrite(STATUS_LED, HIGH);
    
      Serial.println("Reset complete");
  }
    else if (command.startsWith(COMMAND_GET_TELEMETRY)) {
        outputTelemetry();
    }
    else if (command.startsWith(COMMAND_SET_TOP_MOTOR_PWM)) {
      String parameter = command.substring(COMMAND_SET_TOP_MOTOR_PWM.length() + 1);
      parameter.trim();
      int pwmValue = parameter.toInt();
    
      if(!fatalErrorActive) {
        applyTopMotorPWM(pwmValue);
        Serial.print("Setting top motor PWM to: ");
        Serial.println(pwmValue);
      } else {
        Serial.println("ERROR: Cannot set motor PWM while in error state");
      }
    }
    else if (command.startsWith(COMMAND_SET_BOTTOM_MOTOR_PWM)) {
      String parameter = command.substring(COMMAND_SET_BOTTOM_MOTOR_PWM.length() + 1);
      parameter.trim();
      int pwmValue = parameter.toInt();
    
      if(!fatalErrorActive) {
        applyBottomMotorPWM(pwmValue);
        Serial.print("Setting bottom motor PWM to: ");
        Serial.println(pwmValue);
      } else {
        Serial.println("ERROR: Cannot set motor PWM while in error state");
      }
  }
    else {
        Serial.print("Unknown command: '");
        Serial.print(command);
        Serial.println("'");
    }
  }
void setup() {
    // Configure pins
    pinMode(TOP_MOTOR_DRV8871_IN1, OUTPUT);
    pinMode(TOP_MOTOR_DRV8871_IN2, OUTPUT);
    pinMode(BOTTOM_MOTOR_DRV8871_IN1, OUTPUT);
    pinMode(BOTTOM_MOTOR_DRV8871_IN2, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    pinMode(TOP_MOTOR_ACS723, INPUT);
    pinMode(BOTTOM_MOTOR_ACS723, INPUT);

    // Configure PWM and ADC
    analogWriteFreq(20000);
    analogWriteResolution(12);
    analogReadResolution(12);

    // Initialize communications
    Serial.begin(115200);
    Wire.setSDA(I2C_SDA);
    Wire.setSCL(I2C_SCL);
    Wire.begin();

    delay(1000); 
    // Test encoder communication
    Wire.beginTransmission(AS5600_ADDR);
    if (Wire.endTransmission() == 0) {
        Serial.println("AS5600 encoder found!");
    } else {
        Serial.println("ERROR: AS5600 encoder not found!");
        fatalErrorActive = true;
    }
    delay(1000);
    // Calibrate current sensors
    calibrateCurrentSensors();

    // Initialize position tracking
    lastPosition = readRawAngle();
    lastSampleTime = micros();
   
    // Initialize velocity buffer
    for(int i = 0; i < SAMPLE_SIZE; i++) {
      velocities[i] = 0;
    }

    // Start main timer
    add_repeating_timer_ms(10, mainTimerCallback, NULL, &mainTimer);

    // Signal ready
    digitalWrite(STATUS_LED, HIGH);
   
    Serial.println("Wire Tool Firmware - Initialization Complete");
}
void loop() {
   // Process serial commands
   while (Serial.available()) {
       char inChar = (char)Serial.read();
       if (inChar != '\n') {
           inputBuffer += inChar;
       }
       else {
           inputBuffer.trim();
           if (inputBuffer.length() > 0) {
               processCommand(inputBuffer);
           }
           inputBuffer = "";
       }
   }

   // Handle error recovery if reset command received
   if (fatalErrorActive) {
       digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));  // Blink LED
       delay(100);
   }
}