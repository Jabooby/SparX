#include <Arduino.h>
#include <LibRobus.h>
#include <robot_sparX.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
}

void loop() {
  // timer de 50ms, le code dans le if est executé au 50 ms 
  // changer TIMER_TIMER dans robot_sparX.h pour changer le temps
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    sparx.startTimer += TIMER_TIME;

  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}