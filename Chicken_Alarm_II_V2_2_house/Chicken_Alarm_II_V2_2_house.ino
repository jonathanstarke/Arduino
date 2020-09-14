// Special Project for Kevin Pope
// Jonathan Starke
// jonathan@entry.co.za
// Latest Change 12 September 2020

/*   Specification
4 inputs
Chicken house1
Chicken house2
Mains fail
reset


And 3 outputs
Chicken house1 alarm trigger
Chicken house2 alarm trigger
Mains fail alarm trigger
Global Output

All the inputs must be monitored at all times for a low going signal to ground with the same level of priority, low is 0 volt high is 12 volt

Debounce should be absolute minimal if any

If any input is triggered it must trigger the corresponding output with a low GND signal for a period of 5 seconds then high again (high being 12 volt)

The input once received as a low signal whether it stays low or goes high again, must triggered the corresponding output (low for 5 seconds) again every 3 minutes continuously until the reset has been pressed.

Should the rest be pressed but the fault (input still low) still exists the input the output must trigger again and follow the same 3 minute output trigger again until it has been reset manually and the input fault is no longer there.

This should apply to each input the same as explained above

If input 1 triggers and 1 minute later input 2 triggers then both should go into the 3 minute cycle but obviously a minute apart, if all three are triggered then they will all be in the 3 minute cycle until the reset is pressed and the faults 
are or have been resolved (all inputs being high).
The input voltages are either 0 'GND' when there is a fault or 12 volt when no fault.

New 19 February 2020
1. Addition of a LoadShedding Pause period
2. All alarm input to be monitored for the above period, and if still active, then the normal timing starts
3. Addition of a Global Alarm output, that goes HIGH when any of the above outputs are triggered.

*/

unsigned long BuzzerPeriod = 200;              // Period (Seconds) an Output is pulled LOW, for any Alarm Output
unsigned long SleepAlarmPeriod = 5;            // Period (Minutes) between sounding an alarm again, if the reset has not been pushed
int NumberOfSecondsInAMinute  = 60;            // Change this value for testing purposes, make it one, then there is one second in a minute etc.

// Changed 09 September 2020
unsigned long LoadSheddingPause;               // (Seconds) Extra Pause for Load Shedding to let the Generator Start (Seconds)

unsigned long LongLoadSheddingPause = 30;      // (Seconds) Load Shedding period when Loadshedding PIN 3 is Pulled LOW
unsigned long ShortLoadSheddingPause = 5;      // (Seconds) Load Shedding period when NO Loadshedding PIN 3 remains HIGH

unsigned long LoadSheddingTimerAlarm1 = 0;     // Load Shedding Timer for Generator 
unsigned long LoadSheddingTimerAlarm2 = 0;     // Load Shedding Timer for Generator
unsigned long LoadSheddingTimerAlarm3 = 0;     // Load Shedding Timer for Generator

unsigned long AlarmTimer1 = 0;                 // Timer for Alarm1
unsigned long AlarmTimer2 = 0;                 // Timer for Alarm2
unsigned long AlarmTimer3 = 0;                 // Timer for Alarm3


unsigned long TimeNow;                         // Current Time in MiliSeconds

// Input Pins
int Input1 = 5;                                 // Go Low to Trigger
int Input2 = 6;                                 // Go Low to Trigger
int Input3 = 7;                                 // Go Low to Trigger

// Reset Pin
int Reset = 11;                                 // Go LOW to Trigger

// Output Pins
int Output1 = 8;                                // Go LOW to Trigger
int Output2 = 9;                                // Go LOW to Trigger
int Output3 = 10;                               // Go LOW to Trigger

int GlobalOutput = 12;                          // Go HIGH to Trigger - Global Siren

int Alarm1Triggered = 0;                        // Has Alarm it been Triggered - may still be low!
int Alarm2Triggered = 0;                        // Has Alarm it been Triggered - may still be low!
int Alarm3Triggered = 0;                        // Has Alarm it been Triggered - may still be low!

// Set a few things up
void setup() {
  
  // Convert Seconds to MilliSeconds
  BuzzerPeriod = BuzzerPeriod * 1000;
  
  // Convert Minutes to MiliSeconds
  SleepAlarmPeriod = SleepAlarmPeriod * 1000 * NumberOfSecondsInAMinute;
  
  // Enable Serial output
 Serial.begin(9600);
  
  Serial.println("Waiting 4 Seconds");
  // delay(4000);
  
  // Setup the Input Pins
  pinMode(Input1,INPUT_PULLUP);
  digitalWrite(Input1,HIGH);
  pinMode(Input2,INPUT_PULLUP);
  digitalWrite(Input2,HIGH);
  pinMode(Input3,INPUT_PULLUP);
  digitalWrite(Input3,HIGH);
  
  // Setup the Reset Pin
  pinMode(Reset,INPUT_PULLUP);
  digitalWrite(Reset,HIGH);
  
  // Setup the Output Pins
  pinMode(Output1,OUTPUT);
  digitalWrite(Output1,HIGH);
  pinMode(Output2,OUTPUT);
  digitalWrite(Output2,HIGH);
  pinMode(Output3,OUTPUT);
  digitalWrite(Output3,HIGH);
  
  pinMode(GlobalOutput,OUTPUT);
  digitalWrite(GlobalOutput,LOW);    // Goes High when triggered
  
  Serial.println("Waiting 4 Seconds");
  // delay(4000);
  
} // end of Setup

// Main Loop of the Program Starts here
void loop() {
  
  TimeNow = millis();

    // Changed 09 September 2020 - Now dependant on weather Alarm 3 has already been triggered or not - just the 
    // duration of the Loadshedding Pause is reduced 
    // Alarm3 Triggered = 1 then use the LONG Load Shedding Pause
    // Alarm3 NOT Triggered the use the SHORT Load Shedding Pause.
    if(digitalRead(Input3) == LOW) {
        LoadSheddingPause = LongLoadSheddingPause;
        globalTest(Input1,LoadSheddingTimerAlarm1,Alarm1Triggered,Output1,AlarmTimer1);    
        globalTest(Input2,LoadSheddingTimerAlarm2,Alarm2Triggered,Output2,AlarmTimer2);            
    }
    else {
        LoadSheddingPause = ShortLoadSheddingPause;
        globalTest(Input1,LoadSheddingTimerAlarm1,Alarm1Triggered,Output1,AlarmTimer1);    
        globalTest(Input2,LoadSheddingTimerAlarm2,Alarm2Triggered,Output2,AlarmTimer2);            
    }
    
    // Input Alarm Trigger 3 Always behaves with the Long Load Shedding Pause.
    LoadSheddingPause = LongLoadSheddingPause;
    globalTest(Input3,LoadSheddingTimerAlarm3,Alarm3Triggered,Output3,AlarmTimer3); 
       
  // Check if Reset has been pressed
  if (digitalRead(Reset) == LOW) {
    
    // Reset all Timers
    AlarmTimer1 = 0;
    AlarmTimer2 = 0;
    AlarmTimer3 = 0;
    
    // Reset all the Alarm Trigger indicators
    Alarm1Triggered = 0;
    Alarm2Triggered = 0;
    Alarm3Triggered = 0;
    
    // Reset all the LoadShedding Timers
    LoadSheddingTimerAlarm1 = TimeNow;  
    LoadSheddingTimerAlarm2 = TimeNow;  
    LoadSheddingTimerAlarm3 = TimeNow;  
      
    // Switch off All outputs
    digitalWrite(Output1,HIGH);
    digitalWrite(Output2,HIGH);
    digitalWrite(Output3,HIGH);
     
    digitalWrite(GlobalOutput,LOW);        // Global Output for all alarms  
    
    // Switch all the inputs to HIGH again
    digitalWrite(Input1,HIGH);
    digitalWrite(Input2,HIGH);
    digitalWrite(Input3,HIGH);
    
    // Switch the Rest to HIGH again
    digitalWrite(Reset,HIGH);
  }
  
   DisplayData();                 // Comment this line out for no serial output
  
} // end of loop

// FUNCTIONS START HERE

// globalTest  for all input and outputs
void globalTest(int &Input, unsigned long &LoadSheddingTimerAlarm, int &AlarmTriggered, int &Output, unsigned long &AlarmTimer) {

  // Check for an Alarm Condition
  if ((digitalRead(Input) == LOW) & (LoadSheddingTimerAlarm == TimeNow))  {    
    
    // Start Load Shedding Timer if the Alarm has been triggered   
    LoadSheddingTimerAlarm = millis();     
  }

  // If Alarm Condition gone away, reset the LoadSheddingTimer 
  if ((digitalRead(Input) == HIGH)) {    
    
    // Stop Load Shedding Timer if the Alarm has been reset
    LoadSheddingTimerAlarm = TimeNow;     
  }    

  if ((((TimeNow - LoadSheddingTimerAlarm) / 1000) >= LoadSheddingPause) & (AlarmTriggered == 0)) {
    
    // Start the Timer
    AlarmTimer = millis();     // Start the timer for this alarm
    AlarmTriggered = 1;        // Set the Alarm Trigger Flag
  }
  
  // Now to Sound the Alarm, if it was triggered, and it is less than 5 (BuzzerPeriod) Seconds ago
  if ((AlarmTimer > 0) & ((TimeNow - AlarmTimer) <= BuzzerPeriod) & (AlarmTriggered == 1)) {
    
    // Make the Output go Low
    digitalWrite(Output,LOW);
    digitalWrite(GlobalOutput,HIGH);        
  }
  else {
    digitalWrite(Output,HIGH);
  }
  
  // Has three minutes passed, and NO reset
  if((AlarmTimer > 0) & ((TimeNow - AlarmTimer) > SleepAlarmPeriod) & (AlarmTriggered == 1)) {
    // Set the Timer back to as though the reading with alarm just took place
    AlarmTimer = millis();
  }
}


// Debug facility
void DisplayData() {
/*
    Serial.print("BuzzerPeriod:");
        Serial.print(BuzzerPeriod);
            Serial.print("     ");
    Serial.print("SleepAlarmPeriod:");
        Serial.print(SleepAlarmPeriod);
            Serial.print("     ");
                Serial.print("AlarmTimer1:");
                    Serial.print(AlarmTimer1);
                        Serial.print("     ");
                Serial.print("AlarmTimer2:");
                    Serial.print(AlarmTimer2);
                        Serial.print("     ");
                Serial.print("AlarmTimer3:");
                    Serial.print(AlarmTimer3);
                        Serial.print("     ");
                Serial.print("AlarmTimer4:");
                    Serial.print(AlarmTimer4);
                        Serial.print("     ");
                Serial.print("AlarmTimer5:");
                    Serial.print(AlarmTimer5);
                        Serial.print("     ");        
                Serial.print("Time Now:");
                    Serial.print(TimeNow);
    
  Serial.print("     Alarm 1 Triggered:");
  Serial.print(Alarm1Triggered);
  Serial.print("     Alarm 2 Triggered:");
  Serial.print(Alarm2Triggered);
  Serial.print("     Alarm 3 Triggered:");
  Serial.print(Alarm3Triggered);
  Serial.print("     Alarm 4 Triggered:");
  Serial.print(Alarm4Triggered);
  
    Serial.print("     TimeNow:");
    Serial.print(TimeNow);        
*/    
    Serial.print("Input 1: ");
    Serial.print(digitalRead(Input1));    
    Serial.print("  LSE: ");
    Serial.print((TimeNow - LoadSheddingTimerAlarm1) / 1000);        
    Serial.print("  Trigg: ");
    Serial.print(Alarm1Triggered);

    Serial.print(" Input 2: ");
    Serial.print(digitalRead(Input2));    
    Serial.print("  LSE: ");
    Serial.print((TimeNow - LoadSheddingTimerAlarm2) / 1000);        
    Serial.print("  Trigg: ");
    Serial.print(Alarm2Triggered);

    Serial.print(" Input 3: ");
    Serial.print(digitalRead(Input3));    
    Serial.print("  LSE: ");
    Serial.print((TimeNow - LoadSheddingTimerAlarm3) / 1000);        
    Serial.print("  Trigg: ");
    Serial.print(Alarm3Triggered);    
      
/*    
    Serial.print("     LoadSheddingTimerAlarm5:");
    Serial.print(LoadSheddingTimerAlarm5);    
*/    

  Serial.println("  ");
}
