#pragma once
#include <stdint.h>

typedef bool (*GONOGOCallback_t)(void);

class FireControl
{
public:
    bool Loop();
    void AddGONOGOCallback(GONOGOCallback_t funct);

private:
    void Fire();
    bool ReadyToFire();
    bool FireButtonPressed();

    enum class FireControlState{
        WaitingForButtonPress,
        Firing,
        Repressurization,
        
    } mState;


};   

