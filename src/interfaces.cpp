#include <interfaces.h>
//==============================================================================
//==============================================================================
RemoteInterface::RemoteInterface(int recvpin)
                :IRrecv(recvpin)
{
    enableIRIn(); // Start the IR receiver
    prevAct = NONE;
}

//------------------------------------------------------------------------------
ACTION RemoteInterface::getAction( PAGE page ){
/*
The Apple remote returns the following codes:
The value for the third byte when we discard the least significant bit is:
    HEX:
    Repaet -   FFFFFFFF / 4294967295
    UP -       77E1D06C / 2011287660
    Down -     77E1B06C / 2011279468
    Right -    77E1E06C / 2011291756
    left -     77E1106C / 2011238508
    middle -   77E1BA6C or 77E1206C / 2011282028 or 2011242604
    menue -    77E1406C / 2011250796
    play -     77E17A6C or 77E1206C / 2011265644 or 2011242604
*/

  if ( !decode( &results ) )
    return NONE;


   LOG( "Remote:" + String( results.value ) );

    switch (results.value){
      case 4294967295: // REPEAT
        //reapeat if volume change, holding the vol button
        if ( prevAct == VOLUME_DOWN || prevAct == VOLUME_UP ){
          action = prevAct;
        }else{
          action = NONE;
        }
      break;
      case 2011287696: // UP
      case 2011255020:
        action = VOLUME_UP; //action = page == MAIN_MENU? VOLUME_UP : MOVE_UP;
      break;
      case 2011279504: // DOWN
      case 2011246828 :
        action = VOLUME_DOWN; //action = page == MAIN_MENU? VOLUME_DOWN : MOVE_DOWN;
      break;
      case 2011291792: // RIGHT
      case 2011259116:
        action = CHANNEL_RIGHT; //action = page == MAIN_MENU? CHANNEL_RIGHT : NONE;
      break;
      case 2011238544: // LEFT
      case 2011271404:
        action = CHANNEL_LEFT; //action = page == MAIN_MENU? CHANNEL_LEFT : NONE;
      break;
      case 2011282064: // MIDDLE  or 77E1206C
      case 2011275500:
        action = ENTER; //action = page == MAIN_MENU? ENTER : CHANGE_SETTINGS;
      break;
      case 2011250832:  // MENU
      case 2011283692:
        action = MENU;
      break;
      case 2011265680: // PLAY or 77E1206C
      case 2011265644:
        action = NONE;
      break;
      default:
        action = NONE;
      break;
      }

    LOG("Remote action: " + String(action) );

    resume();
    prevAct = action;    // Remember the key in case we want to use the repeat code

return action;
}




//==============================================================================
//==============================================================================
TouchInterface::TouchInterface(byte tclk, byte tcs, byte tdin, byte dout, byte irq)
               :URTouch(tclk, tcs, tdin, dout, irq)
{

  InitTouch(LANDSCAPE); 
  setPrecision(PREC_MEDIUM);

  x = 0;
  y = 0;

}

//------------------------------------------------------------------------------
ACTION TouchInterface::getAction( PAGE page ){

  if (!dataAvailable()){
    return NONE;
  }

  read();
  x = getX();
  y = getY();

  //LOG("x: "); LOG(x); LOG(" / y: "); LOG(y); LOG("\n");

  switch ( page ){
    case MAIN_MENU:
      return actionFromMAIN( x, y );
    case SETTINGS_MENU:
      return actionFromSETTINGS( x, y );
  }

return NONE;
}

//------------------------------------------------------------------------------
ACTION TouchInterface::actionFromMAIN( int x, int y ){

/* 4.3", 480 X 272
CAL_X 0x03E9804BUL
CAL_Y 0x03AC412EUL
CAL_S 0x801DF10FUL

x = 0 ->
y = 0 v
-------------------------------------------
||       1      |           |  6  ||  5  ||
||______________|           |_____||_____||
|                                         |
|                            _____________|
|________________           |            ||
||  2  |  |  3  |           |     4      ||
||     |  |     |           |            ||
------------------------------------------- x = 470, y = 270
*/

  action = NONE;

  Rect channel  = { { 40,  20}, {220,  80} }; //1
  Rect vol_down = { { 40, 185}, {100, 250} }; //2
  Rect vol_up   = { {160, 185}, {220, 250} }; //3
  Rect mute     = { {300, 140}, {450, 250} }; //4
  Rect settings = { {390,  20}, {450,  80} }; //5
  //Rect power_on = { {300,  20}, {360,  80} }; //6


  if ( x >= channel.p1.x && x <= channel.p2.x ){        // Left column

      if ( y >= channel.p1.y && y <= channel.p2.y ){    // Button: Input
        action = CHANNEL_RIGHT;
        LOG("CHANNEL_RIGHT\n");
      }else if ( y >= vol_down.p1.y && y <= vol_down.p2.y ) {// Button: vol_down.y == vol_up.y

        if ( x >= vol_down.p1.x && x <= vol_down.p2.x ){ //Vol down 
          action = VOLUME_DOWN; 
          LOG("VOLUME_DOWN\n");
        } else if ( x >= vol_up.p1.x && x <= vol_up.p2.x ){  //vol up
          action = VOLUME_UP; 
          LOG("VOLUME_UP\n");
        }

      }

  }else if ( x >= mute.p1.x && x <= mute.p2.x ){ //Right Column

      if ( y >= mute.p1.y && y <= mute.p2.y ){          // Button: Mute
        action = ENTER;
        LOG("ENTER\n");
      } else if ( y >= settings.p1.y && y <= settings.p2.y ){  // Button: settings.y == power_on.y
  
        if ( x >= settings.p1.x && x <= settings.p2.x ){  // Button: MENU
          action = MENU; 
          LOG("MENU\n");
        } 
        // else if ( x >= power_on.p1.x && x <= power_on.p2.x ){ // Button: POWER 
        //   action = POWER_ON;
        //   LOG("POWER_ON\n");
        // } 

      }
        
  }

return action;
}

//------------------------------------------------------------------------------
ACTION TouchInterface::actionFromSETTINGS( int x, int y ){
  
/*
x = 0 ->
y = 0 v

  int x = 30;
  int y = 20; 
  int h = 50;
  int l = 320;
  int jump = 60; 

-------------------------------------------
|   |         1          |         |  5  ||
|   |____________________|         |_____||
|   |         2          |                |
|   |____________________|                |
|   |         3          |                |
|   |____________________|                |
|   |         4          |                |
------------------------------------------- x = 470, y = 270

NOTE:
Use only two buttons shifted in the middle for now
  int y = 40;
  int jump = 80;

*/

  action = NONE;

  Rect fir_filter = { {30,  20},  {350,  70} }; //1
  Rect iir_bandw  = { {30,  80},  {350, 130} }; //2
  Rect dpll_band  = { {30,  140}, {350, 190} }; //3
  Rect jitter_el  = { {30,  200}, {350, 250} }; //4

  Rect settings = { {390,  20}, {450,  80} }; //5

  if ( x >= fir_filter.p1.x && x <= fir_filter.p2.x ){  // Settings Buttons, all the same as FIR

    if ( y >= fir_filter.p1.y && y <= fir_filter.p2.y ){  // Button: FIR
      action = SET_FIR_FILTER; 
      LOG("FIR\n");
    } else if ( y >= iir_bandw.p1.y && y <= iir_bandw.p2.y ){  // Button: IIR
      action = SET_IIR_BANDWIDTH; 
      LOG("IIR\n");
    }else if ( y >= dpll_band.p1.y && y <= dpll_band.p2.y ){  // Button: DPLL
      action = SET_DPLL; 
      LOG("DPLL\n");
    } else if ( y >= jitter_el.p1.y && y <= jitter_el.p2.y ){  // Button: Jitter eliminator
      action = TOGGLE_JE; 
      LOG("TOGLE JE\n");
    }

  }else if ( x >= settings.p1.x && x <= settings.p2.x ){  // Button: MENU
    if ( y >= settings.p1.y && y <= settings.p2.y ){  // Button: MENU
      action = MENU; 
      LOG("MENU\n");
    }  
  }
        
return action;
}

//------------------------------------------------------------------------------
bool TouchInterface::detectHold( ACTION& act, ACTION lastAct ){

        int interval = millis() - pressTime;
        if( lastAct != act || interval > wait_after_act ){ //interval wait to start new holding
          pressTime = lastInterruptTime = millis();
          LOG("----------first touch----------\n");
        }else{ //hold
          //First holding
          int no_touch = millis() - lastInterruptTime;
          if( no_touch > 200 ){ //reset as holding interrupt detected
            act = RESET; //this will set all new
            pressTime = lastInterruptTime = millis();
            LOG("Interruption, start new counter (reset)\n");
            return false;
          }else{
            lastInterruptTime = millis(); //update interrupt time
          }
          interval = millis() - pressTime;
          LOG( interval );
          if( interval > act_after ){ // holding
            /*
            This is where the touch screen hold action should be placed
            */
            LOG("\nhold button: " + String (action) );
            act = RESET; //this will set all new 
            return true;

            pressTime = millis(); //update press time
        }else if ( interval < 300 ){
          //tapped_only;
        }
      }
  return false;
}

//==============================================================================
