//Rack Robotics, Inc.
//Wire Tool Firmware
//For use with Rarduino-Pico Plugin by EarlePhilhower

#include <Wire.h>

// Digital Pins Definitions
const int I2C_SDA                     = 4;                //I2C Data for AS5600 encoder
const int I2C_SCL                     = 5;                //I2C Clock for AS5600 encoder
const int TOP_MOTOR_DRV8871_IN1       = 19;               //Motor driver IN1 for top tool motor
const int TOP_MOTOR_DRV8871_IN2       = 18;               //Motor driver IN2 for top tool motor
const int BOTTOM_MOTOR_DRV8871_IN1    = 17;               //Motor driver IN1 for top tool motor
const int BOTTOM_MOTOR_DRV8871_IN2    = 16;               //Motor driver IN2 for top tool motor
const int STATUS_LED                  = 25;               //Status LED on Pico

// Analog Pin Definitions
const int TOP_MOTOR_ACS723            = 26;               //Analog output of current sensor for top tool motor
const int BOTTOM_MOTOR_ACS723         = 27;               //Analog output of current sensor for bottom tool motor

// Variables
const uint8_t AS5600_ADDR = 0x36;                         // I2C address of AS5600 magnetic encoder
const uint8_t REG_RAW_ANGLE_H = 0x0C;                     // High byte register for raw angle
const uint8_t REG_RAW_ANGLE_L = 0x0D;                     // Low byte register for raw angle
const float WHEEL_DIAMETER = 30.0;                        // Diameter of measurement wheel in mm
const float MM_PER_REVOLUTION = PI * WHEEL_DIAMETER;      // Linear distance per revolution in mm
const int SAMPLE_SIZE = 50;                               // Number of samples in moving average window
volatile uint16_t lastPosition = 0;                       // Previous encoder position (0-4095)
volatile float velocities[SAMPLE_SIZE];                   // Circular buffer of velocity measurements
volatile int velocityIndex = 0;                           // Current index in velocity buffer
volatile uint32_t lastSampleTime = 0;                     // Last sample time in microseconds

// Serial Messages
String READY_MESSAGE                          = "WIRETOOL:READY";

uint16_t readRawAngle() { // Reads raw angle from AS5600 encoder over I2C. Returns: 12-bit angle value (0-4095) or 0 if read fails
   Wire.beginTransmission(AS5600_ADDR);     // Start I2C transmission
   Wire.write(REG_RAW_ANGLE_H);             // Request high byte
   Wire.endTransmission(false);             // Repeated start
   Wire.requestFrom(AS5600_ADDR, 2);        // Request 2 bytes
   if(Wire.available() <= 2) {              // Check data available
       uint16_t high = Wire.read();         // Read high byte
       uint16_t low = Wire.read();          // Read low byte
       return (high << 8) | low;            // Combine bytes
   }
   return 0;                                // Return 0 if read fails
}
float calculateAverageVelocity() { // Calculate average velocity from the circular buffer of velocity samples 
  float sum = 0;                            // Initialize sum
  for(int i = 0; i < SAMPLE_SIZE; i++) {    // Sum all samples
      sum += velocities[i];
  }
  return sum / SAMPLE_SIZE;                 // Return average
}
bool positionSampleCallback(struct repeating_timer *t) {  // Timer callback for position sampling and velocity calculation (runs every 10ms)
   uint32_t currentTime = micros();                       // Get current timestamp
   uint16_t currentPosition = readRawAngle();             // Read encoder position
   
   int16_t deltaPos = currentPosition - lastPosition;     // Calculate position change
   if(deltaPos < -2048) deltaPos += 4096;                 // Handle wraparound at 0
   if(deltaPos > 2048) deltaPos -= 4096;                  // Handle wraparound at 4095
   
    if (abs(deltaPos) < 2) {    // If movement is below threshold, zero velocity and update timestamps
      velocities[velocityIndex] = 0;
      velocityIndex = (velocityIndex + 1) % SAMPLE_SIZE;
      lastPosition = currentPosition;
      lastSampleTime = currentTime;
      return true;
   }
   
   float timeElapsed = (currentTime - lastSampleTime) / 1000000.0;  // Convert to seconds
   if (timeElapsed < 0.001) return true;                            // Skip if time delta too small
   float rotationFraction = deltaPos / 4096.0;                      // Convert counts to revolutions
   float linearDistance = rotationFraction * MM_PER_REVOLUTION;     // Convert to mm
   
   velocities[velocityIndex] = linearDistance / timeElapsed;        // Calculate mm/s
   velocityIndex = (velocityIndex + 1) % SAMPLE_SIZE;               // Update buffer index
   
   lastPosition = currentPosition;                                  // Store values for next sample
   lastSampleTime = currentTime;
   return true;                                                     // Keep timer running
}
bool telemetryCallback(struct repeating_timer *t) {
  uint16_t currentPos = readRawAngle();
  Serial.print("Raw Angle: ");
  Serial.print(currentPos);
  Serial.print(" WIRE_VELOCITY: ");
  Serial.print(calculateAverageVelocity(), 2);
  Serial.println(" mm/s");
  return true;  // Not Return
}

// Timer structures for position sampling and telemetry
repeating_timer positionTimer;           // 10ms timer for velocity sampling
repeating_timer telemetryTimer;          // 500ms timer for serial output

void setup() {
   pinMode(STATUS_LED, OUTPUT);         // Configure LED pin for output

   // Configure PWM and ADC
   analogWriteFreq(10000);             // Set PWM frequency
   analogWriteResolution(12);          // 12-bit PWM resolution
   delay(1000);                        // Stabilization delay
   analogReadResolution(12);           // 12-bit ADC resolution

   // Initialize communications
   Serial.begin(115200);               // Start serial port
   Wire.setSDA(4);                     // Configure I2C pins
   Wire.setSCL(5);
   Wire.begin();                       // Start I2C bus

   // Initialize position tracking
   lastPosition = readRawAngle();      // Get initial encoder reading
   lastSampleTime = micros();          // Get initial timestamp

   // Start timer callbacks
   add_repeating_timer_ms(10, positionSampleCallback, NULL, &positionTimer);   // Velocity sampling
   add_repeating_timer_ms(500, telemetryCallback, NULL, &telemetryTimer);      // Serial reporting

   // Signal ready state
   digitalWrite(STATUS_LED, HIGH);     // Turn on status LED
   Serial.println(READY_MESSAGE);      // Send ready message
}

void loop() {
}