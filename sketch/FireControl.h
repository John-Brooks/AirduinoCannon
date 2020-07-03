#pragma once
#include <stdint.h>
#include "ButtonInput.h"

typedef bool (*GONOGOCallback_t)(void);

class FireControl
{
public:
    FireControl();
    bool Loop(uint32_t current_time);
    void AddGONOGOCallback(GONOGOCallback_t funct);
    void GetSelectedBarrel(int barrel) const;
    void SetDwellTime(int barrel, uint32_t micros);
    uint32_t GetDwellTime(int barrel);

private:
    void Fire(uint32_t current_time);
    bool ReadyToFire();
    bool FireButtonIsPressed(uint32_t current_time);
    bool BarrelSelectButtonPressed(uint32_t current_time);
    void SelectNextBarrel();
    void WaitValveDwellTime();

    enum class FireControlState{
        WaitingForButtonPress,
        Repressurization,
    } mState;

    int mSelectedBarrel = 1;
    ButtonInput mFireBtn;
    ButtonInput mBarrelSelectBtn;

    uint32_t mDwellTimes[3];
    uint32_t mRepressurizationDoneTime;
};   

