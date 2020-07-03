#include "FireControl.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "TimeDifference.h"

#if defined (__linux__) || (__WIN32)
    #include <cassert>
#else
    #include <assert.h>
#endif

#define MAX_CALLBACKS 5
#define FIRE_DEBOUNCE_TIME 250
#define BARREL_SEL_DEBOUNCE_TIME 250
#define REPRESSURIZATION_TIME 5000

static GONOGOCallback_t GONOGOCallbacks[MAX_CALLBACKS] { nullptr };
int NUM_CALLBACKS = 0;

FireControl::FireControl() : 
    mFireBtn(FIRE_BUTTON_PIN, FIRE_DEBOUNCE_TIME), 
    mBarrelSelectBtn(BARREL_SELECT_BUTTON_PIN, BARREL_SEL_DEBOUNCE_TIME)
{
}

bool FireControl::ReadyToFire()
{
    for (int i = 0; i < NUM_CALLBACKS; i++)
    {
        if (!GONOGOCallbacks[i]()) 
            return false; 
    }
    return true;
}

void FireControl::AddGONOGOCallback(GONOGOCallback_t funct)
{
    assert(NUM_CALLBACKS+1 <= MAX_CALLBACKS);
    GONOGOCallbacks[NUM_CALLBACKS++] = funct;
}

void FireControl::SelectNextBarrel()
{
    if (mSelectedBarrel >= 3)
        mSelectedBarrel = 1;
    else
        mSelectedBarrel++;
}

bool FireControl::Loop(uint32_t current_time)
{
    if(mBarrelSelectBtn.IsPressed(current_time))
        SelectNextBarrel();
    
    switch (mState)
    {
        case FireControlState::WaitingForButtonPress:
            if(mFireBtn.IsPressed(current_time))
            {
                if (ReadyToFire())
                    Fire(current_time);
            }
            break;
        case FireControlState::Repressurization:
            if (millis() < mRepressurizationDoneTime)
                mState = FireControlState::WaitingForButtonPress;
            break;
        default:
            return false;
    }
    return true;
}

void FireControl::Fire(uint32_t current_time)
{
    mRepressurizationDoneTime = millis() + REPRESSURIZATION_TIME; 
    int pin;
    switch(mSelectedBarrel)
    {
        case 1:
            pin = BARREL_1_SOLENOID_PIN;
            break;
        case 2:
            pin = BARREL_2_SOLENOID_PIN;
            break;
        case 3:
            pin = BARREL_3_SOLENOID_PIN;
            break;
        default:
            return;
    }

    uint32_t now;
    digitalWrite(pin, SOLENOID_OPEN);
    WaitValveDwellTime();
    digitalWrite(pin, SOLENOID_CLOSED);

    mState = FireControlState::Repressurization;
}
void FireControl::WaitValveDwellTime()
{
    uint32_t dwell = mDwellTimes[mSelectedBarrel-1];
    
    //Per the Arduino documentation delayMicroseconds is only accurate to ~16000 us. 
    //If we need to wait longer than that, wait using delay (in ms), then
    //wait using delayMicroseconds.
    if(dwell > 16000)
    {
        int dwell_partial = (dwell - 16000) / 1000;
        delay(dwell_partial);
        delayMicroseconds(dwell - dwell_partial);
    }
    else
        delayMicroseconds(dwell);
}