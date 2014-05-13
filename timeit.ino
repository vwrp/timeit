// button pin
int button = 7;
// led pin
int led = 13;


// earliest time for the led to be
// switched on after the signal for
// the measurement was received
unsigned long start = 1000;

// latest time for the led to be
// switched on after the signal for
// the measurement was received
unsigned long end = 5000;

// the measurment is aborted if
// timeout milliseconds have passed
// after the led was switched on
unsigned long timeout = 5000;

// rate in milliseconds at which the 
// button is checked for beeing pressed
unsigned long rate = 100;

// buffer for reaction time
unsigned long react_time = 0;

/**
 * \brief Function for measuring the reaction by button pressing 
 *        after an led was switched on at a random time
 *
 * \param react_time   Mesured reaction time
 *
 * \param start        Earliest time for the led to be
 *                     switched on after the signal for
 *                     the measurement was received
 *
 * \param end          Latest time for the led to be
 *                     switched on after the signal for
 *                     the measurement was received
 *
 * \param timeout      The measurment is aborted if
 *                     timeout milliseconds have passed
 *                     after the led was switched on
 * \param rate         Rate in milliseconds at which the
 *                     button is checked for beeing pressed
 *
 * @return             Returns true if the mesurement was
 *                     successfull, if the timeout is reached,
 *                     false is returned
 */
bool measure(unsigned long & react_time, unsigned long & start, unsigned long & end, unsigned long & timeout, unsigned long & rate) {

    // success of the measurement 
    bool success=false;
    // time when the led is switched on
    unsigned long start_time;
    // time when the button is pressed
    unsigned long end_time; 
    // passed time since the beginning of 
    // the mesurement
    unsigned long passed_time = 0;
    // bool for led on
    bool on=false;

    // delay for random time between start and end
    delay(start + random(end-start));
    // switch on led
    digitalWrite(led, HIGH);
    on=true;
    // save starting time
    start_time = millis();
    while(on && passed_time<timeout) {
        if (digitalRead(button)==true) {
            // save end time
            end_time = millis();
            // switch of led
            digitalWrite(led, LOW);
            on=false;
            // calculate reaction time
            react_time = end_time - start_time;
            success=true;
        }
        Serial.println(passed_time);
        delay(rate);
        passed_time = millis()-start_time;
    }
    // switch of led
    digitalWrite(led, LOW);
    return success;
}

/**
 * \brief Function which receives the 
 *        parameter for the reaction measurement
 *        from the device over a serial connection
 */
void receiveParameter(unsigned long & start, unsigned long & end, unsigned long & timeout, unsigned long & rate) {

    // wait until serial communication is received
	while (!Serial.available());
    
    // save last distance measurement
    int region=Serial.read() - '0';
    Serial.print("Start measurement. Button is ");
    if (digitalRead(button)==true) {
        Serial.println("ON");
    }
    else {
        Serial.println("OFF");
    }
}

// send reaction time to device
void sendReactTime(unsigned long react_time) {
    Serial.print("Result: ");
    Serial.print(react_time);
    Serial.println(" ms");
}

// send fail signal to device
void sendFail() {
    Serial.print("Result: ");
    Serial.println(" Fail");
}

void setup() {
    // set up serial communication
    Serial.begin(9600);
    // set the led pin as an output
    pinMode(led, OUTPUT);
    // set the button pin as an input
    pinMode(button, INPUT);
    // switch of led at begining
    digitalWrite(led, LOW);
}
 
void loop() {
    // wait for device to send start signal together with parameters
    receiveParameter(start, end, timeout, rate);
    // perform measurement
    bool success = measure(react_time, start, end, timeout, rate);
    // send reaction time or fail signal to device
    if (success) {
        sendReactTime(react_time);
    }
    else {
        sendFail();
    }
}
