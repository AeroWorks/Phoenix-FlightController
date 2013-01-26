// Main loop variables
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sensorPreviousTime = 0;
uint8_t frameCounter = 0;
uint32_t itterations = 0;

bool all_ready = false;
bool armed = false;
bool flightMode = false;
bool altitudeHoldBaro = false;
bool altitudeHoldSonar = false;

// Flight modes
#define RATE_MODE 0
#define ATTITUDE_MODE 1

// Blinking LED to indicate activity
#define LED_PIN 13
#define LED_ORIENTATION 14
bool Alive_LED_state = false;

// Modulo definitions (integer remainder)
#define TASK_50HZ 2
#define TASK_10HZ 10
#define TASK_1HZ 100

// Axis deifnitions
#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

// Kinematics variable defnitions
double kinematicsAngleX = 0.0;
double kinematicsAngleY = 0.0;
double kinematicsAngleZ = 0.0;

// FlightController commands definitions
double commandYaw, commandYawAttitude, commandPitch, commandRoll, commandThrottle;

// PID variables
double YawCommandPIDSpeed, PitchCommandPIDSpeed, RollCommandPIDSpeed;
double YawMotorSpeed, PitchMotorSpeed, RollMotorSpeed, AltitudeHoldMotorSpeed;
int16_t throttle = 1000;

// Custom definitions
//#define DATA_VISUALIZATION
//#define DISPLAY_ITTERATIONS