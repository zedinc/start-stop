#define START 10
#define KILL 11
#define THROTTLE 9
#define CDI 8
#define MAX_SPEED 120
#define SPEED_THRESHOLD 2
#define THROTTLE_OPEN_DURATION 20
#define THROTTLE_CLOSED_DURATION 40
#define START_DURATION 30
#define KILL_DURATION 10
#define START_STATE "START"
#define KILL_STATE "KILL_"
#define ON_STATE "ON___"
#define OFF_STATE "OFF__"

double raw2vel( int signal ) {
  return ( 1 - signal / 1024.0 ) * MAX_SPEED;
}

unsigned long timeElapsed( unsigned long start ) {
	return millis() - start;
}

void setup() {
  Serial.begin(9600);
  pinMode(START, OUTPUT);
  pinMode(KILL, OUTPUT);
  pinMode(THROTTLE, INPUT);
  pinMode(CDI, INPUT);
  pinMode(A0, INPUT);
  digitalWrite(START, LOW);
  digitalWrite(KILL, LOW);
}

unsigned long startTimer = 0;
unsigned long throttleTimer = 0;
unsigned long killTimer = 0;
unsigned long prevThrottleState = LOW;
char * prev_state = OFF_STATE;
char * state = OFF_STATE;

void loop() {
  
  delay(3);

  // long speed = raw2vel( analogRead(A0) );
  // long throttle = raw2vel( analogRead(A1) );
  double speed = raw2vel( analogRead(A0) );
  // double speed = analogRead(A0) ;
  int throttle = HIGH - digitalRead(THROTTLE);
  int cdi = digitalRead(CDI);
  // char * state;
  
  // Serial.print( "Speed: " );
  // Serial.print( speed );
  // Serial.print( " ; CDI: " );
  // Serial.print( cdi );
  // Serial.print( " ; Throttle: ");
  // Serial.println( throttle );
  
  // Update timers
  
  if ( prevThrottleState != throttle ) {  // reset throttle state
	  
	  Serial.print("Throttle changed from ");
	  Serial.print( prevThrottleState );
	  Serial.print(" to ");
	  Serial.println( throttle );
	  prevThrottleState = throttle;
	  throttleTimer = millis();
  }
  
  // change state
  
  bool throttleOpenTimeElapsed = timeElapsed(throttleTimer) >= THROTTLE_OPEN_DURATION ;
  bool throttleClosedTimeElapsed = timeElapsed(throttleTimer) >= THROTTLE_CLOSED_DURATION ;
  bool startTimeElapsed = timeElapsed(startTimer) >= START_DURATION ;
  bool killTimeElapsed = timeElapsed(killTimer) >= KILL_DURATION ;
  
  if ( prev_state == OFF_STATE && speed < SPEED_THRESHOLD && throttle == HIGH && throttleOpenTimeElapsed ) {
	  state = START_STATE;
	  startTimer = millis();
  }
  
  else if ( prev_state == START_STATE && cdi == HIGH && startTimeElapsed ) {
	  state = ON_STATE;
  }
  
  else if ( prev_state == START_STATE && cdi == LOW && startTimeElapsed ) {
	  state = OFF_STATE;
  }
  
  else if ( prev_state == ON_STATE && speed < SPEED_THRESHOLD && throttle == LOW && throttleClosedTimeElapsed ) {
	  state = KILL_STATE;
	  killTimer = millis();
  }
  
  else if ( prev_state == KILL_STATE && killTimeElapsed ) {
	  state = OFF_STATE;
  }
  
  // execute state change
  
  if ( state == prev_state ) {
	  return;
  }
  
  Serial.print( "thrOpenTimer: " );
  Serial.print( throttleOpenTimeElapsed );
  Serial.print( " | thrClosedTimer: " );
  Serial.print( throttleClosedTimeElapsed );
  Serial.print( " | killTimer: " );
  Serial.print( killTimeElapsed );
  Serial.print( " | startTimer: " );
  Serial.println( startTimeElapsed );
  
  Serial.print("Shifting from ");
  Serial.print( prev_state );
  Serial.print( " to " );
  Serial.println( state );
  
  if ( state == OFF_STATE || state == ON_STATE ) {
      
	  digitalWrite( START, LOW );
	  digitalWrite( KILL, LOW );
  }
  
  else if ( state == START_STATE ) {
	  digitalWrite( START, HIGH );
	  digitalWrite( KILL, LOW );
  }
  
  else if ( state == KILL_STATE ) {
	  digitalWrite( START, LOW );
	  digitalWrite( KILL, HIGH );
  }
  
  prev_state = state;
  
  delay(3); // Delay a little bit to improve simulation performance
}