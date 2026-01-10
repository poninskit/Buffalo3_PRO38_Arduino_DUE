#ifndef INTERFACES_H
#define INTERFACES_H


#include <globals.h>
#include <IRremote2.h> // Remote
#include <URTouch.h> // For Touchscreen



//==============================================================================

class RemoteInterface:IRrecv{
  public:
    RemoteInterface( int recvpin = 8 );
    ACTION getAction( PAGE page = MAIN_MENU );

  private:
    decode_results results;
    unsigned long lastRemoteMillis = 0; //Prevente Debounce in Remote,  it keeps track of time between clicks

    ACTION action;  // actual action
    ACTION prevAct; // last action taken for repeat

};

//==============================================================================
class TouchInterface:URTouch{
  public:
    TouchInterface(byte tclk = 6, byte tcs = 5, byte tdin = 4, byte dout = 3, byte irq = 2);
    ACTION getAction( PAGE page );
    bool detectHold( ACTION& act, ACTION lastAct );

  private:
    ACTION action;
    int x;
    int y;

    ACTION actionFromMAIN( int x, int y );
    ACTION actionFromSETTINGS( int x, int y );

    //detect button hold
    int act_after = 1000;
    int wait_after_act = act_after + 300;

    unsigned long pressTime = 0;
    unsigned long lastInterruptTime = 0;
};
#endif /* INTERFACES_H */
