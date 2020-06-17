#include "FireControl.h"
#if defined (__linux__) || (__WIN32)
    #include <cassert>
#else
    #include <assert.h>
#endif

#define MAX_CALLBACKS 5
static GONOGOCallback_t GONOGOCallbacks[MAX_CALLBACKS] { nullptr };
int NUM_CALLBACKS = 0;

bool FireControl::FireButtonPressed()
{
    if(!ReadyToFire())
        return false;

    return true;
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

bool FireControl::Loop()
{
    switch (mState)
    {
        case FireControlState::WaitingForButtonPress:
            if(FireButtonPressed())
            {
                if (ReadyToFire())
                    Fire();
                else
                    return false;
            }
            break;
        case FireControlState::Firing:
            break;
        case FireControlState::Repressurization:
            break;
        default:
            return false;
    }
    return true;
}

void FireControl::Fire()
{

}
