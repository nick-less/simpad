//////////////////////////////////////////////////////////////////////////////
//
// File         :  controlpanelImpl.cpp
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
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "controlPanelImpl.hpp"

enum { AV=0, TIME, POWER, NETWORK, WIRELESS, CS3, INFO };
   char *tabNames[]= { "A / V", "Time", "Power", "Network", 
		       "Wireless", "CS3", "Info" };
int MAXLINE = 1024;
#define STDOUT 1
#define TRUE 1
#define FALSE 0
#define COMMAND "ifconfig -a"
#define COMMAND2 "iwconfig"


ControlPanelImpl::ControlPanelImpl():
  ControlPanel()
{
   nrOfNetDevs = 0;
   nrOfWifiDevs = 0;
   cs3Value = 0;
   applyButton->deactivate();

   rateInput->deactivate();
   wifiChannelChoice->deactivate();

   initCurrentTab();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::initCurrentTab()
{
   changeTab();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updateAvTab()
{
  lightSlider->value( getBackLight() );
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updateTimeTab()
{
  hourInput->value( sysClock->hour() ); 
  minInput->value( sysClock->minute() ); 
  secInput->value( sysClock->second() );
  
    
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updatePowerTab()
{
    enableCommand();
    enableSoundFile();

    string vBattStr = executeCommand( "cat /proc/ucb1x00/vbatt" );
    string vChargerStr = executeCommand( "cat /proc/ucb1x00/vcharger" );

    long vBatt = strtol( vBattStr.c_str(), NULL, 10 );
    long vCharger = strtol( vChargerStr.c_str(), NULL, 10 );
    

    batteryState->maximum( 8200 );
    batteryState->minimum( 6500 );

    if( vCharger < 12000 )
    {
        if( vBatt < 8200 && vBatt > 7400 )
	    batteryState->color( FL_GREEN );
	else
	{
	    if( vBatt < 7400 && vBatt > 6600 )
	        batteryState->color(  FL_RED );
	    else
	        batteryState->color(  FL_DARK_RED );
	}

        batteryState->value( vBatt );
    }
    else
    {
        batteryState->color( FL_MAGENTA );
        batteryState->value( 8200 );
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updateNetworkTab()
{
    nrOfNetDevs = scanDevice( "ifconfig -a" );
    
    if( nrOfNetDevs > 0 )
    {
        for( int i = 0; i < nrOfNetDevs; ++i )
	{
	  netDeviceChoice->add(netDev[i].getDevName().c_str());	   
	}

	netDeviceChoice->value(0);
	changeNetDevice();
    }
    
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updateWirelessTab()
{
    nrOfWifiDevs = scanDevice( "iwconfig" );

    if( nrOfWifiDevs > 0 )
    {
        wifiDeviceChoice->activate();
	wifiModeChoice->activate();
	essidInput->activate();
	freqInput->activate();

        for( int i = 0; i < nrOfWifiDevs; ++i )
	{
	    wifiDeviceChoice->add(wifiDev[i].getDevName().c_str());	
	}
	wifiDeviceChoice->value(0);
	changeWifiDevice();
    }
    else
    {
        wifiDeviceChoice->deactivate();
	wifiModeChoice->deactivate();
	essidInput->deactivate();
	freqInput->deactivate();
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updateCS3Tab()
{
    const int DECT_POWER_ON = 0x0100;
    const int IRDA_SD       = 0x0200;
    const int RS232_ON      = 0x0400;
    const int LED2_ON       = 0x1000;
    const int IRDA_MODE     = 0x2000;

    string cs3String = executeCommand( "cat /proc/CS3" );
    cs3String = getValueFromString( cs3String, "Chipselect3", 2 );
    cs3Value = strtol( cs3String.c_str(), NULL, 16);

    if( cs3Value & DECT_POWER_ON )
      dectCheckBox->set();
    else
      dectCheckBox->clear();
    
    if( cs3Value & IRDA_SD )
       enIrdaCheckBox->clear();
    else
       enIrdaCheckBox->set();

    if( cs3Value & RS232_ON )
      rs232CheckBox->set();
    else
      rs232CheckBox->clear();
    
    if( cs3Value & LED2_ON )
      greeLedCheckBox->set();
    else
      greeLedCheckBox->clear();

    if( cs3Value & IRDA_MODE )
      irdaModeCheckBox->set();
    else
      irdaModeCheckBox->clear();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::updateInfoTab()
{
   string versionStr = executeCommand( "cat /proc/version" );
   string cpuInfoStr = executeCommand( "cat /proc/cpuinfo" );
   
   string kernelVersionStr = getValueFromString( versionStr, 
						 "version", 1 );
   string gccVersionStr    = getValueFromString( versionStr, 
						 "gcc version", 1 );
   gccVersionStr = "Build with " + gccVersionStr; 

   cpuInfoStr = getValueFromString( cpuInfoStr, "Processor", 2 );
   
   cpuInfoBox->label( cpuInfoStr.c_str() );
   kernelBox->label( kernelVersionStr.c_str() );
   glibcBox->label( gccVersionStr.c_str() );
}
//-----------------------------------------------------------------------------
string ControlPanelImpl::executeCommand( const char * command )
{
   int n;
   char line[MAXLINE];
   FILE *fp;
   string versionStr;

   if((fp = popen(command, "r"))==NULL)
      printf("popen error");

   while( (fgets(line, MAXLINE, fp)) != NULL)
   {
      n = strlen(line);

#ifdef DEBUG
      if(write(1, line, n) !=n )
         printf("data write error");
#endif
      versionStr += line;
   }

   if(ferror(fp))
      printf("fgets error");

   pclose(fp);

   return versionStr;
}
//-----------------------------------------------------------------------------
string ControlPanelImpl::getValueFromString( const string scrString,
					     const string keyword, int mode )
{
   string::size_type pos, startPos=0, endPos=0, length;
   string subStr="";
   
   pos = scrString.find( keyword );
   
   if( startPos != string::npos )
   {
       if( mode == 1)
       {
	   length = keyword.length();
	   startPos =pos + length +1; // +1 for the first space
	   
	   endPos = scrString.find( " ", startPos );
	   subStr = scrString.substr( startPos,
				      endPos-startPos );
       }
       else
       {
	   length = keyword.length();
	   startPos = scrString.find( ": ", pos+length );
	   length = string(": ").length();
	   endPos = scrString.find( "\n", startPos+length );
	   subStr = scrString.substr( startPos+length,
				      endPos-startPos-length );
       }
       
   }
  
   return subStr;
}
//-----------------------------------------------------------------------------
int ControlPanelImpl::getCurrentTab( )
{
   int i = 0, choose = -1;
   const char* tabLabel = controlTab->value()->label();

   while( choose < 0 )
   {
     if( !strcmp(tabLabel, tabNames[i]) )
       choose = i;
     ++i;
   }
   return choose;
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::doOkay() 
{
   if( applyButton->active() )
       doApply();
   exit(0);
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::doApply() 
{
   int choose = getCurrentTab();
   
   switch( choose )
   {
   case TIME: 
     setTimeValue();
     break;
   case POWER: 
     setPowerValue();
     break;
   case NETWORK: 
     setNetworkValue();
     break;
   case WIRELESS: 
     setWirelessValue();
     break;
   case CS3: 
     setCS3Value();
     break;
   default :
     cout << "No such tab!\n";
     break;
   }

   if( applyButton->active() )
      applyButton->deactivate();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setTimeValue()
{
    sysClock->value( hourInput->value(), 
		     minInput->value(), 
		     secInput->value() );
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setPowerValue()
{

}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setNetworkValue()
{
    vector<int> inetAddr;
    vector<int> bcastAddr;
    vector<int> netMask;

    char writeCommand[100];
    int idx = netDeviceChoice->value();

    if( nrOfNetDevs > 0 )
    {
        inetAddr.push_back( inetAddr0->value() );
	inetAddr.push_back( inetAddr1->value() );
	inetAddr.push_back( inetAddr2->value() );
	inetAddr.push_back( inetAddr3->value() );
	
	bcastAddr.push_back( bcastAddr0->value() );
	bcastAddr.push_back( bcastAddr1->value() );
	bcastAddr.push_back( bcastAddr2->value() );
	bcastAddr.push_back( bcastAddr3->value() );

	netMask.push_back( netMask0->value() );
	netMask.push_back( netMask1->value() );
	netMask.push_back( netMask2->value() );
	netMask.push_back( netMask3->value() );

	netDev[idx].setInetAddrInt( inetAddr );
	netDev[idx].setBcastAddrInt( bcastAddr );
	netDev[idx].setMaskInt( netMask );
	
	sprintf(writeCommand, "ifconfig %s  %s broadcast %s netmask %s up", 
		netDev[idx].getDevName().c_str(),
		netDev[idx].getInetAddrStr().c_str(),
		netDev[idx].getBcastAddrStr().c_str(),
		netDev[idx].getMaskStr().c_str() );
	system(writeCommand);

	nrOfNetDevs = scanDevice( "ifconfig -a" );
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setWirelessValue()
{
    char writeCommand[100];
    int idx = wifiDeviceChoice->value();

    if( nrOfWifiDevs > 0 )
    {
        wifiDev[idx].setESSID( essidInput->value() );
	wifiDev[idx].setFreqStr( freqInput->value() );
	wifiDev[idx].setMode( wifiModeChoice->text() );

        sprintf(writeCommand, "iwconfig %s  essid %s", 
		wifiDev[idx].getDevName().c_str(),
		wifiDev[idx].getESSID().c_str() );
	system(writeCommand);

	sprintf(writeCommand, "iwconfig %s freq %s", 
		wifiDev[idx].getDevName().c_str(),
		wifiDev[idx].getFreqStr().c_str()  );
	system(writeCommand);

	sprintf(writeCommand, "iwconfig %s mode %s", 
		wifiDev[idx].getDevName().c_str(),
		wifiDev[idx].getMode().c_str() );
	system(writeCommand);
    }

    nrOfWifiDevs = scanDevice( "iwconfig" );
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setCS3Value()
{
   char writeCommand[100];
   const int DECT_POWER_ON = 0x0100;
   const int IRDA_SD       = 0x0200;
   const int RS232_ON      = 0x0400;
   const int IRDA_MODE     = 0x2000;

   if( dectCheckBox->value() )
      cs3Value |= DECT_POWER_ON;
   else
      cs3Value &= ~DECT_POWER_ON;
    
   if( enIrdaCheckBox->value() )
      cs3Value &= ~IRDA_SD;
   else
      cs3Value |= IRDA_SD;
   
   if( rs232CheckBox->value() )
      cs3Value |= RS232_ON;
   else
      cs3Value &= ~RS232_ON;
    
    if( irdaModeCheckBox->value() )
      cs3Value |= IRDA_MODE;
    else
      cs3Value &= ~IRDA_MODE;
     
    sprintf(writeCommand, "echo 0x%x > /proc/CS3", (uint)cs3Value);
    system(writeCommand);
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::doCancel() 
{
  exit(0);
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeTab() 
{
   int choose = getCurrentTab();
   
    switch( choose )
    {
    case AV: 
      updateAvTab();
      break;
    case TIME: 
      updateTimeTab();
      break;
    case POWER: 
      updatePowerTab();
      break;
    case NETWORK: 
      updateNetworkTab();
      break;
    case WIRELESS: 
      updateWirelessTab();
      break;
    case CS3: 
      updateCS3Tab();
      break;
    case INFO:
      updateInfoTab();
      break;
    default :
      cout << "No such tab!\n";
      break;
    }
   if( applyButton->active() )
      applyButton->deactivate();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeVol() 
{
  printf(":changeVol()!\n");
}
//-----------------------------------------------------------------------------
int ControlPanelImpl::getVolume()
{
  return 0;
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setVolume( int value )
{

}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeMic() 
{
  printf(":changeMic()!\n");
}
//-----------------------------------------------------------------------------
int ControlPanelImpl::getMic()
{
   return 0;
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setMic( int value )
{

}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeLight() 
{
  setBackLight( lightSlider->value()) ;
}
//-----------------------------------------------------------------------------
int ControlPanelImpl::getBackLight()
{
   int value=0;
   string backLightStr = 
     executeCommand( "cat /proc/driver/mq200/registers/PWM_CONTROL" );
   value = strtol( backLightStr.c_str(), NULL, 16);
   value = value & 0xff00;
   value = value >> 8;
   
   return value;
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::setBackLight( int value )
{
   char writeCommand[100];
   int mask=0x00a10044;
   value = value << 8;
   value += mask;

   sprintf(writeCommand, "echo 0x%x > /proc/driver/mq200/registers/PWM_CONTROL",
	   value);
   system(writeCommand);
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::volMute() 
{
   muteSlider( volSlider );
   printf(":volMute()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::micMute() 
{
   muteSlider( micSlider );
   printf(":micMute()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::lightMute() 
{
   if( lightSlider->active() )
      setBackLight( 0xff );
   else
     setBackLight( 0x6c );
   muteSlider( lightSlider );
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::muteSlider( Fl_Slider *slider ) 
{
   if( slider->active() )
   {
      slider->value( 0xff );
      slider->deactivate();
   }
   else
   {
      slider->value( 0x6c );
      slider->activate();
   }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::doCalibration() 
{
     executeCommand( "xcalibrate");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::show( int argc, char** argv) 
{
     controlWin->show();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeMonth() 
{
    changeApplyButton();
    printf(":changeMonth()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeYear() 
{
    changeApplyButton();
    printf(":changeYear()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::plusTime() 
{
    Fl_Widget *currentWidget = Fl::focus();
    int hourVal, minVal, secVal;

    if( currentWidget == hourInput )
    {
        hourVal = (int)hourInput->value()+1;
	if( hourVal >= 24)
	    hourVal = 0;
	hourInput->value( hourVal );
    }  

    if( currentWidget == minInput )
    {
        minVal = (int)minInput->value()+1;
	if( minVal >= 59 )
	   minVal = 0;
	minInput->value( minVal );
    }  
    
    if( currentWidget == secInput )
    {
        secVal = (int)secInput->value()+1;
	if( secVal >= 59 )
	    secVal = 0;
	secInput->value( secVal );
    }  

    printf(":plusTime()! %s\n", currentWidget->label() );
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::minusTime() 
{
  Fl_Widget *currentWidget = Fl::focus();
    int hourVal, minVal, secVal;

    if( currentWidget == hourInput )
    {
        hourVal = (int)hourInput->value();
	if( hourVal == 0)
	    hourVal = 23;
	else
	    --hourVal;
	hourInput->value( hourVal );
    }  

    if( currentWidget == minInput )
    {
        minVal = (int)minInput->value();
	if( minVal == 0)
	   minVal = 59;
	else
	    --minVal;
	minInput->value( minVal );
    }  
    
    if( currentWidget == secInput )
    {
        secVal = (int)secInput->value();
	if( secVal == 0)
	    secVal = 59;
	else
	    --secVal;
	secInput->value( secVal );
    }  
    
    printf(":minusTime()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::openCommand() 
{
  printf(":openCommand()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::openSoundFile() 
{
  printf(":openSoundFile()!\n");
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::enableCommand() 
{
    if( runCommandCheck->value() )
    {
        commandInput->activate();
	commandButton->activate();
    }
    else
    {
        commandInput->deactivate();
	commandButton->deactivate();
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::enableSoundFile() 
{
    if( playSoundCheck->value() )
    {
        soundFileInput->activate();
	soundFileButton->activate();
    }
    else
    {
        soundFileInput->deactivate();
	soundFileButton->deactivate();
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeNetDevice() 
{
     vector<int> inetAddr;
     vector<int> bcastAddr;
     vector<int> netMask;

     int idx = netDeviceChoice->value();
     
     hwAddrBox->label( netDev[idx].getHwAddrStr().c_str() );
     hwAddrBox->redraw();

     netDev[idx].getInetAddrInt( inetAddr );
     netDev[idx].getBcastAddrInt( bcastAddr );
     netDev[idx].getMaskInt( netMask );
     
     vector<int>::iterator itr;
     itr = inetAddr.begin();
     inetAddr0->value(*itr);	
     ++itr;
     inetAddr1->value(*itr);	
     ++itr;
     inetAddr2->value(*itr);	
     ++itr;
     inetAddr3->value(*itr);	
     
     itr = bcastAddr.begin();
     bcastAddr0->value(*itr);	
     ++itr;
     bcastAddr1->value(*itr);	
     ++itr;
     bcastAddr2->value(*itr);	
     ++itr;
     bcastAddr3->value(*itr);	
     
     itr = netMask.begin();
     netMask0->value(*itr);	
     ++itr;
     netMask1->value(*itr);	
     ++itr;
     netMask2->value(*itr);	
     ++itr;
     netMask3->value(*itr);	
     
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::showDevInfoDlg() 
{
    if( nrOfNetDevs > 0 )
    {
        int idx = netDeviceChoice->value();
	infoDevOutput->textsize(12);
	infoDevOutput->value( netDev[idx].getDevInfo().c_str() );
	infoDlgWin->show();
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeWifiDevice() 
{
    int idx = netDeviceChoice->value();
    essidInput->insert( wifiDev[idx].getESSID().c_str() );
    freqInput->insert( wifiDev[idx].getFreqStr().c_str() );
    
    for(int i=0; i < wifiModeChoice->size(); ++i)
    {
        if( !strcmp( wifiDev[idx].getMode().c_str(), 
		     wifiModeChoice->text(i) ) )
	{
	    wifiModeChoice->value(i);
	    i = wifiModeChoice->size();
	}
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::showWifiInfoDlg() 
{
    if( nrOfWifiDevs > 0 )
    {
        int idx = netDeviceChoice->value();
	infoDevOutput->textsize(12);
	infoDevOutput->value( wifiDev[idx].getDevInfo().c_str() );
	infoDlgWin->show();
    }
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::changeApplyButton()
{
  if( !applyButton->active() )
    applyButton->activate();

  if( rs232CheckBox->value() )
    baudRateChoice->activate();
  else
    baudRateChoice->deactivate();
}
//-----------------------------------------------------------------------------
void ControlPanelImpl::doInfoOkay()
{
    infoDlgWin->hide();
}
//-----------------------------------------------------------------------------
int ControlPanelImpl::scanDevice( char * command )
{
   int nrKeyWords  = 0, whichDev;
   char line[MAXLINE];
   FILE *fp;
   int n, countDevs = 0;
   bool foundDev = FALSE;
   string tmpStr, devInfo;
   string *keyWords;
   string devKeyWord[] = { "eth0" , "eth1", "eth2", "wlan0", "wlan1", "wlan2",
                          "usbf", "irda0", "irda1", "lo" };

   string wifiKeyWord[] = { "eth0" , "eth1", "eth2",
                            "wlan0", "wlan1", "wlan2" };

   if ( !strcmp( command, COMMAND) )
   {
      keyWords = devKeyWord;
      nrKeyWords  = (sizeof(devKeyWord) / sizeof(devKeyWord[0]));
      whichDev = 1;
   }
   else
   {
     keyWords = devKeyWord;
     nrKeyWords  = (sizeof(wifiKeyWord) / sizeof(wifiKeyWord[0]));
     whichDev = 0;
   }

   if((fp = popen( command, "r" ))==NULL)
      printf("popen error");

   while( ( fgets(line, MAXLINE, fp) ) != NULL)
   {
      n = strlen( line );

#ifdef DEBUG
      if( write( STDOUT, line, n ) !=n )
        printf( "data write error" );
#endif

      tmpStr = line;

      if( foundDev != TRUE )
      {
         for(int i = 0; i < nrKeyWords; ++i )
         {
	   
            string::size_type pos = tmpStr.find( keyWords[i] );

            if( pos >= 0  && pos <= tmpStr.length() )
            {
                if( whichDev == 1)
                    netDev[countDevs].setDevName( keyWords[i]);
                else
                    wifiDev[countDevs].setDevName( keyWords[i]);

                foundDev = TRUE;
                devInfo = "";
                devInfo += tmpStr;
            }
         }
      }
      else
      {
         if(strlen(line) <= 5 )
         {
            if( whichDev == 1)
                netDev[countDevs].setDevInfo( devInfo );
            else
                wifiDev[countDevs].setDevInfo( devInfo );
             ++countDevs;
            foundDev = FALSE;
         }
         else
         {
            devInfo += tmpStr;
         }
      }
   }

   if( ferror(fp) )
      printf("fgets error");

   pclose(fp);

   return countDevs;
}
