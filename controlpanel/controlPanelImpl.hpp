//////////////////////////////////////////////////////////////////////////////
//
// File         :  controlpanelImpl.hpp
// Class        :  ControlPanelImpl
// Description  :  This class is inherent from ControPanel. All function from 
//                 controlPanel.fl are here implemented
// Author       :  Juergen P. Messerer, juergen.messerer@freesurf.ch
// Project      :  Configuration Tool for SIMpad
// Release Date :  25th October 2002
// Revised on   :
// License      :  Gnu Public License (GPL)
//
// Copyright(c) Juergen P. Messerer 2002.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ControlPanelImpl_hpp
#define ControlPanelImpl_hpp

#include <string>
#include "controlPanel.h"
#include "netdevice.h"
#include "wifiDev.h"


class ControlPanelImpl: public ControlPanel
{
public:
  ControlPanelImpl();
  void doOkay();
  void doApply();
  void doCancel();
  void changeTab();
  void changeVol();
  void changeMic();
  void changeLight();
  void volMute();
  void micMute();
  void lightMute();
  void doCalibration();
  void show( int argc, char** argv);
  void changeMonth();
  void changeYear();
  void plusTime();
  void minusTime();
  void openCommand();
  void openSoundFile();
  void enableCommand();
  void enableSoundFile();
  void changeNetDevice();
  void showDevInfoDlg();
  void changeWifiDevice();
  void showWifiInfoDlg();
  void changeApplyButton();
  void doInfoOkay();

private:
  void initCurrentTab();
  
  void updateAvTab();
  void updateTimeTab();
  void updatePowerTab();
  void updateNetworkTab();
  void updateWirelessTab();
  void updateCS3Tab();
  void updateInfoTab(); // Its neccessary!

  void setTimeValue();
  void setPowerValue();
  void setNetworkValue();
  void setWirelessValue();
  void setCS3Value();

  string executeCommand( const char *command );
  string getValueFromString( const string scrString, const string keyword, 
			     int mode );
  int getCurrentTab();
  
  int getVolume();
  void setVolume( int value );
  int getMic();
  void setMic( int value );
  int getBackLight();
  void setBackLight( int value );
  void muteSlider( Fl_Slider *slider );
  int scanDevice( char * command );

  long cs3Value;
  NetDevice netDev[10];
  int nrOfNetDevs;
  WifiDev   wifiDev[10];
  int nrOfWifiDevs;
  
};

#endif //ControlPanelImpl_hpp
