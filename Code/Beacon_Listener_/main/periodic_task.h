#include <Arduino.h>
#define TIME_INTERRUPT 30 //SECONDS
volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;
hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timer_mqtt;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer()
{
    // Increment the counter and set the time of ISR
    portENTER_CRITICAL_ISR(&timerMux);
    isrCounter++;
    lastIsrAt = millis();
    portEXIT_CRITICAL_ISR(&timerMux);
    // Give a semaphore that we can check in the loop
    xSemaphoreGiveFromISR(timer_mqtt, NULL);
    // It is safe to use digitalRead/Write here if you want to toggle an output
}
void config_timer()
{
    // Create mqtt timer to reconnect broker
    timer_mqtt = xSemaphoreCreateBinary();
    // Use 1st timer of 4 (counted from zero).
    // Set 80 divider for prescaler
    timer = timerBegin(0, 80, true);
    // Attach onTimer function to our timer.
    timerAttachInterrupt(timer, &onTimer, true);
    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(timer, TIME_INTERRUPT * 1000000, true); //30 seconds
    // Start an alarm
    timerAlarmEnable(timer);
}
