//Rack Robotics, Inc.
//Wire Tool Firmware
//For use with Rarduino-Pico Plugin by EarlePhilhower

//Dependencies: Wire.h (built-in)
//Hardware: RP2040, AS5600, DRV8871, ACS723

#include <Wire.h>

// Digital Pin Definitions
const int I2C_SDA                     = 4;    //I2C Data for AS5600 encoder
const int I2C_SCL                     = 5;    //I2C Clock for AS5600 encoder
const int TOP_MOTOR_DRV8871_IN1       = 19;   //Motor driver IN1 for top tool motor
const int TOP_MOTOR_DRV8871_IN2       = 18;   //Motor driver IN2 for top tool motor
const int BOTTOM_MOTOR_DRV8871_IN1    = 17;   //Motor driver IN1 for top tool motor
const int BOTTOM_MOTOR_DRV8871_IN2    = 16;   //Motor driver IN2 for top tool motor
const int STATUS_LED                  = 25;   //Status LED on Pico

// Analog Pin Definitions
const int TOP_MOTOR_ACS723            = 26;   //Analog output of current sensor for top tool motor
const int BOTTOM_MOTOR_ACS723         = 27;   //Analog output of current sensor for bottom tool motor

// AS5600 Encoder Constants
const uint8_t AS5600_ADDR             = 0x36; //I2C address of AS5600 magnetic encoder
const uint8_t REG_RAW_ANGLE_H         = 0x0C; //High byte register for raw angle
const uint8_t REG_RAW_ANGLE_L         = 0x0D; //Low byte register for raw angle
const float WHEEL_DIAMETER            = 30.0; //Diameter of measurement wheel in mm
const float MM_PER_REVOLUTION         = PI * WHEEL_DIAMETER; //Linear distance per revolution in mm
const int SAMPLE_SIZE                 = 50;   //Number of samples in moving average window

// Motor Control Constants
const int MIN_PWM = 0.10 * 4095;             //Minimum PWM value (10% of 4095)
const int MAX_PWM = 0.90 * 4095;             //Maximum PWM value (90% of 4095)

// PID Control Variables
float Kp = 400.0;   
float Ki = 0.5;    
float Kd = 1.0;    
float integral = 0;
float lastError = 0;
unsigned long lastPIDTime = 0;
const float INTEGRAL_MAX = 1000.0;  

// Encoder Variables
volatile uint16_t lastPosition = 0;          //Previous encoder position (0-4095)
volatile float velocities[SAMPLE_SIZE];      //Circular buffer of velocity measurements
volatile int velocityIndex = 0;             //Current index in velocity buffer
volatile uint32_t lastSampleTime = 0;       //Last sample time in microseconds
volatile float targetVelocity = 0;

// State Variables
String inputBuffer = "";
bool fatalErrorActive = false;
String errorMessage = "";
bool wireIsLoaded = false;
float targetCurrent = 0;                    //Target motor current for tension control
float actualCurrent = 0;                    //Measured motor current

// Serial Commands
String COMMAND_LOAD_WIRE                = "WIRETOOL: LOAD_WIRE";
String COMMAND_UNLOAD_WIRE              = "WIRETOOL: UNLOAD_WIRE";
String COMMAND_CALIBRATE_WIRE_TENSION   = "WIRETOOL: CALIBRATE_WIRE_TENSION";
String COMMAND_SET_WIRE_VELOCITY        = "WIRETOOL: SET_WIRE_VELOCITY";
String COMMAND_SET_WIRE_TENSION         = "WIRETOOL: SET_WIRE_TENSION";
String COMMAND_SET_KP                   = "WIRETOOL: SET_KP";
String COMMAND_SET_KI                   = "WIRETOOL: SET_KI";
String COMMAND_SET_KD                   = "WIRETOOL: SET_KD";
String COMMAND_GET_PID                  = "WIRETOOL: GET_PID";
String COMMAND_TEST_ERROR               = "WIRETOOL: TEST_ERROR";
String COMMAND_RESET                    = "WIRETOOL: RESET";
String COMMAND_GET_TELEMETRY            = "WIRETOOL: GET_TELEMETRY";

// Telemetry Data Structure
struct Telemetry {
    float targetVelocity;
    float actualVelocity;
    float velocityError;
    float targetCurrent;
    float actualCurrent;
    float currentError;
    bool wireStatus;
    bool errorStatus;
    String errorMsg;
} telemetryData;

// Timer Structures
repeating_timer positionTimer;              //10ms timer for velocity sampling
repeating_timer telemetryCollectionTimer;   //10ms timer for data collection

//Read raw angle from AS5600 encoder
uint16_t readRawAngle() {
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

//Calculate average velocity from samples
float calculateAverageVelocity() {
    float sum = 0;
    for(int i = 0; i < SAMPLE_SIZE; i++) {
        sum += velocities[i];
    }
    return sum / SAMPLE_SIZE;
}

//Collect telemetry data every 10ms
bool getTelemetryCallback(struct repeating_timer *t) {
    telemetryData.targetVelocity = targetVelocity;
    telemetryData.actualVelocity = calculateAverageVelocity();
    telemetryData.velocityError = targetVelocity - telemetryData.actualVelocity;

    telemetryData.targetCurrent = targetCurrent;
    telemetryData.actualCurrent = analogRead(BOTTOM_MOTOR_ACS723) * (3.3 / 4096.0);
    telemetryData.currentError = targetCurrent - telemetryData.actualCurrent;

    telemetryData.wireStatus = wireIsLoaded;
    telemetryData.errorStatus = fatalErrorActive;
    telemetryData.errorMsg = errorMessage;

    return true;
}

//Output telemetry data over serial
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
    Serial.print("Actual Current: ");
    Serial.print(telemetryData.actualCurrent, 2);
    Serial.println(" mA");
    Serial.print("Current Error: ");
    Serial.print(telemetryData.currentError, 2);
    Serial.println(" mA");
    
    Serial.print("Wire Status: ");
    Serial.println(telemetryData.wireStatus ? "LOADED" : "UNLOADED");
    Serial.print("Error Status: ");
    Serial.println(telemetryData.errorStatus ? telemetryData.errorMsg : "NO ERROR");
    
    Serial.println("TELEMETRY_END");
}

//Position sampling callback (10ms)
bool positionSampleCallback(struct repeating_timer *t) {
    uint32_t currentTime = micros();
    uint16_t currentPosition = readRawAngle();
    
    int16_t deltaPos = currentPosition - lastPosition;
    if(deltaPos < -2048) deltaPos += 4096;
    if(deltaPos > 2048) deltaPos -= 4096;
    
    if (abs(deltaPos) < 2) {
        velocities[velocityIndex] = 0;
        velocityIndex = (velocityIndex + 1) % SAMPLE_SIZE;
        lastPosition = currentPosition;
        lastSampleTime = currentTime;
        return true;
    }
    
    float timeElapsed = (currentTime - lastSampleTime) / 1000000.0;
    if (timeElapsed < 0.001) return true;
    float rotationFraction = deltaPos / 4096.0;
    float linearDistance = rotationFraction * MM_PER_REVOLUTION;
    
    velocities[velocityIndex] = linearDistance / timeElapsed;
    velocityIndex = (velocityIndex + 1) % SAMPLE_SIZE;
    
    lastPosition = currentPosition;
    lastSampleTime = currentTime;
    return true;
}

//Handle fatal errors
void triggerFatalError(String message) {
    fatalErrorActive = true;
    errorMessage = message;
    
    analogWrite(TOP_MOTOR_DRV8871_IN1, 0);
    analogWrite(TOP_MOTOR_DRV8871_IN2, 0);
    analogWrite(BOTTOM_MOTOR_DRV8871_IN1, 0);
    analogWrite(BOTTOM_MOTOR_DRV8871_IN2, 0);
    
    digitalWrite(STATUS_LED, LOW);
    Serial.print("FATAL ERROR: ");
    Serial.println(message);
    
    while (fatalErrorActive) {
        digitalWrite(STATUS_LED, HIGH);
        delay(100);
        digitalWrite(STATUS_LED, LOW);
        delay(100);
        
        while (Serial.available()) {
            String command = Serial.readStringUntil('\n');
            command.trim();
            
            if (command == COMMAND_RESET) {
                Serial.println("Reset command received. Restarting device...");
                fatalErrorActive = false;
                digitalWrite(STATUS_LED, HIGH);
                return;
            }
        }
    }
}

//Process serial commands
void processCommand(String command) {
    Serial.print("Received command: '");
    Serial.print(command);
    Serial.println("'");

    if (command.startsWith(COMMAND_LOAD_WIRE)) {
        // TODO: Implement wire loading sequence
        Serial.println("Loading wire...");
        wireIsLoaded = true;
    }
    else if (command.startsWith(COMMAND_UNLOAD_WIRE)) {
        // TODO: Implement wire unloading sequence
        Serial.println("Unloading wire...");
        wireIsLoaded = false;
    }
    else if (command.startsWith(COMMAND_SET_WIRE_VELOCITY)) {
        String parameter = command.substring(COMMAND_SET_WIRE_VELOCITY.length() + 1);
        parameter.trim();
        float velocity = parameter.toFloat();
        targetVelocity = velocity;
        Serial.print("Setting wire velocity to: ");
        Serial.println(velocity, 2);
    }
    else if (command.startsWith(COMMAND_SET_WIRE_TENSION)) {
        String parameter = command.substring(COMMAND_SET_WIRE_TENSION.length() + 1);
        parameter.trim();
        float tension = parameter.toFloat();
        targetCurrent = tension;  // Convert tension to current setpoint
        Serial.print("Setting wire tension to: ");
        Serial.println(tension, 2);
    }
    else if (command.startsWith(COMMAND_CALIBRATE_WIRE_TENSION)) {
        // TODO: Implement tension calibration sequence
        Serial.println("Starting wire tension calibration...");
    }
    else if (command.startsWith(COMMAND_SET_KP)) {
        String parameter = command.substring(COMMAND_SET_KP.length() + 1);
        parameter.trim();
        Kp = parameter.toFloat();
        Serial.print("Setting Kp to: ");
        Serial.println(Kp);
    }
    else if (command.startsWith(COMMAND_SET_KI)) {
        String parameter = command.substring(COMMAND_SET_KI.length() + 1);
        parameter.trim();
        Ki = parameter.toFloat();
        Serial.print("Setting Ki to: ");
        Serial.println(Ki);
    }
    else if (command.startsWith(COMMAND_SET_KD)) {
        String parameter = command.substring(COMMAND_SET_KD.length() + 1);
        parameter.trim();
        Kd = parameter.toFloat();
        Serial.print("Setting Kd to: ");
        Serial.println(Kd);
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
            triggerFatalError(parameter);  // Use custom error message
        } else {
            triggerFatalError("Test error triggered");  // Default message
        }
    }
    else if (command.startsWith(COMMAND_RESET)) {
        if (fatalErrorActive) {
            Serial.println("Resetting device from error state...");
            fatalErrorActive = false;
            errorMessage = "";
            digitalWrite(STATUS_LED, HIGH);
            // TODO: Implement any additional reset procedures
        } else {
            Serial.println("Device is not in error state");
        }
    }
    else if (command.startsWith(COMMAND_GET_TELEMETRY)) {
        outputTelemetry();
    }
    else {
        Serial.print("Unknown command: '");
        Serial.print(command);
        Serial.println("'");
    }
}

void setup() {
    //Configure pins
    pinMode(TOP_MOTOR_DRV8871_IN1, OUTPUT);
    pinMode(TOP_MOTOR_DRV8871_IN2, OUTPUT);
    pinMode(BOTTOM_MOTOR_DRV8871_IN1, OUTPUT);
    pinMode(BOTTOM_MOTOR_DRV8871_IN2, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    pinMode(TOP_MOTOR_ACS723, INPUT);
    pinMode(BOTTOM_MOTOR_ACS723, INPUT);

    //Configure PWM and ADC
    analogWriteFreq(10000);
    analogWriteResolution(12);
    analogReadResolution(12);

    //Initialize communications
    Serial.begin(115200);
    Wire.setSDA(4);
    Wire.setSCL(5);
    Wire.begin();

    //Initialize position tracking
    lastPosition = readRawAngle();
    lastSampleTime = micros();

    //Start timers
    add_repeating_timer_ms(10, positionSampleCallback, NULL, &positionTimer);
    add_repeating_timer_ms(10, getTelemetryCallback, NULL, &telemetryCollectionTimer);

    //Signal ready
    digitalWrite(STATUS_LED, HIGH);
}

void loop() {
    //Process serial commands
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
}