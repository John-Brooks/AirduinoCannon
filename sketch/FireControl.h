#pragma once
#include <stdint.h>
#include "ButtonInput.h"
#include "Runnable.h"

typedef bool (*GONOGOCallback_t)(void);
typedef bool (*AboutToFireCallback_t)(int);

#define MAX_GO_NOGO_CALLBACKS 5

class FireControl : public Runnable
{
public:
    FireControl();
    void Run(uint32_t current_time);
    void AddGONOGOCallback(GONOGOCallback_t funct);
    int GetSelectedBarrel() const { return mSelectedBarrel; };
    void SetDwellTime(int barrel, uint32_t micros);
    uint32_t GetDwellTime(int barrel);
    bool HasFault() const { return mFaultActive; }
    const char* GetFaultText() const { return mFaultText; }
    void ClearFault() { mFaultActive = false; }
    void SetAboutToFireCallback(AboutToFireCallback_t funct);

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

    AboutToFireCallback_t mAboutToFireCallback = nullptr;
};   

