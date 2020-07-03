#pragma once
#include <stdint.h>
#include "ButtonInput.h"

typedef bool (*GONOGOCallback_t)(void);

#define MAX_GO_NOGO_CALLBACKS 5

class FireControl
{
public:
    FireControl();
    bool Loop(uint32_t current_time);
    void AddGONOGOCallback(GONOGOCallback_t funct);
    int GetSelectedBarrel() const { return mSelectedBarrel; };
    void SetDwellTime(int barrel, uint32_t micros);
    uint32_t GetDwellTime(int barrel);
    bool HasFault() const { return mFaultActive; }
    const char* GetFaultText() const { return mFaultText; }
    void ClearFault() { mFaultActive = false; }

private:
    void Fire(uint32_t current_time);
    bool ReadyToFire();
    bool FireButtonIsPressed(uint32_t current_time);
    bool BarrelSelectButtonPressed(uint32_t current_time);
    void SelectNextBarrel();
    void WaitValveDwellTime();

    int mSelectedBarrel = 1;
    ButtonInput mFireBtn;
    ButtonInput mBarrelSelectBtn;

    uint32_t mDwellTimes[3];
    uint32_t mRepressurizationDoneTime;

    int mNumCallbacks = 0;
    GONOGOCallback_t mGONOGOCallbacks[MAX_GO_NOGO_CALLBACKS] = { nullptr };

    bool mFaultActive = false;
    char mFaultText[33];
};   

