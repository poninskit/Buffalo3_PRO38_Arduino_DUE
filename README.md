The software is based on TPA firmware, it behaves the same way: power up, reset, communication, configurateion, etc... is all based on TPA firmware.
The software gives you at this stage basic interaction, like volume change and input change, the rest of the configuration is read from the switches on the BIII board.
This software is easily scalable, you can replace any of the classes with your own or modify existing.
I have added some "interfaces" for editing, like eeprom or touch (didnt test the touch, but its easy to figure out how it works and correct it).
I also prepared the settings page for the tft screen so one can add it later on.


Main function is literaly 20 lines of code, it just takes the action from interface and apply to the DAC class.


As I said, the heart of the DAC Class is TPA Software.
The configuration file is the copy of TPA, ES9028_38.h file.
DAC class constructor is build of TPA functions, slightly modified for Arduino needs ( i use Wire.h for I2C communication ), but the funtions:
powerDACup();
initializeDAC();
configureDAC();
are practically the same you get with TPA firmware, those work the same way and are called the way TPA is doing it.


Simplified soft looks like this:
//--------------------------------------
setup:

dac = new DAC();
remoteInterface = new RemoteInterface();
touchInterface = new TouchInterface();
tftGraphics = new TFTGraphics();

//---------------------
loop:

action = remoteInterface->getAction();
if( action == NONE )
action = touchInterface->getAction( MAIN_MENU );


if( action == NONE ){
dac->readSwitchStates();
}


switch ( action ){
case NONE:
break;
case CHANNEL_LEFT:
tftGraphics->printChannel( dac->decreaseInput() );
break;
case CHANNEL_RIGHT:
tftGraphics->printChannel( dac->increaseInput() );
break;
case VOLUME_UP:
tftGraphics->printVolume( dac->increaseVolume() );
break;
case VOLUME_DOWN:
tftGraphics->printVolume( dac->decreaseVolume() );
break;
case ENTER:
tftGraphics->printVolume( dac->muteVolume() );
break;
default:
break;
}
action = NONE;

//-------------------------


All you need is Atom + Platform IO + ( Arduino Due, remote, TFT) and the repository:


see for more details:
https://www.diyaudio.com/community/threads/c-easy-and-scalable-software-for-biii38pro.337580/
