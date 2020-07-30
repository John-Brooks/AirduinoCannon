#include "FireControl.h"
#include "Arduino.h"
#include "ArduinoPins.h"
#include "TimeDifference.h"
#include <cstring>

#if defined (__linux__) || (__WIN32)
    #include <cassert>
#else
    #include <assert.h>
#endif

#define FIRE_DEBOUNCE_TIME 250
#define BARREL_SEL_DEBOUNCE_TIME 250

FireControl::FireControl() : 
    mFireBtn(FIRE_BUTTON_PIN, FIRE_DEBOUNCE_TIME), 
    mBarrelSelectBtn(BARREL_SELECT_BUTTON_PIN, BARREL_SEL_DEBOUNCE_TIME)
{
    mDwellTimes[0] = 250000;
    mDwellTimes[1] = 250000;
    mDwellTimes[2] = 250000;
}

bool FireControl::ReadyToFire()
{
    static bool fire_button_fault_lockout = true;

    for (int i = 0; i < mNumCallbacks; i++)
    {
        if (!mGONOGOCallbacks[i]()) 
        {
            fire_button_fault_lockout = digitalRead(FIRE_BUTTON_PIN);
            return false; 
        }
    }

    if (fire_button_fault_lockout)
    {
        if (digitalRead(FIRE_BUTTON_PIN))
        {
            mFaultActive = true;
            const char* fault_text = "FAULT:  FIRE    "
                                     "CONTROL STUCK   ";
            memcpy(mFaultText, fault_text, sizeof(mFaultText));
            return false;
        }
        else
            fire_button_fault_lockout = false;
    }

    return true;
}

void FireControl::AddGONOGOCallback(GONOGOCallback_t funct)
{
    assert(mNumCallbacks+1 <= MAX_GO_NOGO_CALLBACKS);
    mGONOGOCallbacks[mNumCallbacks++] = funct;
}

void FireControl::SetAboutToFireCallback(AboutToFireCallback_t funct)
{
    mAboutToFireCallback = funct;
}

void FireControl::SelectNextBarrel()
{
    if (mSelectedBarrel >= 3)
        mSelectedBarrel = 1;
    else
        mSelectedBarrel++;
}

void FireControl::Run(uint32_t current_time)
{
    if(mBarrelSelectBtn.IsPressed(current_time))
        SelectNextBarrel();

    bool ready_to_fire = ReadyToFire();
    
    if (ready_to_fire)
        digitalWrite(SAFETY_LIGHT_PIN, SAFETY_LIGHT_ON); 
    else
        digitalWrite(SAFETY_LIGHT_PIN, SAFETY_LIGHT_OFF);
    
    

    if(mFireBtn.IsPressed(current_time))
    {
        if (ready_to_fire)
            Fire(current_time);
    }
}

void FireControl::Fire(uint32_t current_time)
{
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

    //Last Chance
    if(mAboutToFireCallback && !mAboutToFireCallback(mSelectedBarrel))
        return;

    uint32_t now;
    digitalWrite(pin, SOLENOID_OPEN);
    WaitValveDwellTime();
    digitalWrite(pin, SOLENOID_CLOSED);
}
void FireControl::WaitValveDwellTime()
{
    uint32_t dwell = mDwellTimes[mSelectedBarrel-1];
    
    //Per the Arduino documentation delayMicroseconds is only accurate to ~16000 us. 
    //If we need to wait longer than that, wait using delay (in ms), then
    //wait using delayMicroseconds.
    if(dwell > 15000)
    {
        int dwell_partial = (dwell - 15000) / 1000;
        delay(dwell_partial);
        delayMicroseconds(dwell - (dwell_partial * 1000));
    }
    else
        delayMicroseconds(dwell);
}
void FireControl::SetDwellTime(int barrel, uint32_t micros)
{
    mDwellTimes[barrel] = micros;
}