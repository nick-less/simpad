//////////////////////////////////////////////////////////////////////////////
//
// File         :  wifiDev.cpp
// Class        :  WifiDev
// Description  :  This class handels the information of a wireless device like
//                 name, essid, mode,  freq, etc.
// Author       :  Juergen P. Messerer, juergen.messerer@freesurf.ch
// Project      :  Configuration Tool
// Release Date :  21th October 2002
// Revised on   :
// License      :  Gnu Public License (GPL)
//
// Copyright(c) Juergen P. Messerer 2002.
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "wifiDev.h"

string wifiKeyWord[] = { "ESSID:", "Mode:", "Frequency:", "Signal level:" };
int nrWifiWords  = (sizeof(wifiKeyWord) / sizeof(wifiKeyWord[0]));
enum { ESSID=0, MODE, FREQ, SENS };

WifiDev::WifiDev()
{
   m_devInfo = "";
   m_devName = "";
}
//-----------------------------------------------------------------------------
WifiDev::WifiDev( const char* devName )
{
    m_devInfo = "";
    m_devName = devName;
}
//-----------------------------------------------------------------------------
void WifiDev::setDevInfo( string devInfo )
{
   m_devInfo = devInfo;
   string::size_type startPos, endPos, length;
   string subStr;
   
   for(int i = 0; i < nrWifiWords; ++i )
   {
       startPos = devInfo.find( wifiKeyWord[i] );
              
       if( startPos != string::npos )
       {
	  length = wifiKeyWord[i].length();
	  endPos = devInfo.find( " ", startPos+length );
	  subStr = devInfo.substr( startPos+length, 
				   endPos-startPos-length );

	  switch( i )
	  {
	    case ESSID: { m_essId = subStr; }
	    break;

	    case MODE: { m_mode = subStr; }
	    break;

	    case FREQ: { m_freq = subStr; }
	    break;
	    
	    case SENS: { m_sens = subStr; }
	    break;
	    default:
	      cout << "Error: No such case!" << endl;
	      break;
	  } //switch
       } // if(..)
   } // for(..)
}
//-----------------------------------------------------------------------------
void WifiDev::setFreqInt( uint freq )
{
   m_freq = convIntToString( freq );
}
//-----------------------------------------------------------------------------
void WifiDev::setChannelInt( uint channel )
{
   m_channel = convIntToString( channel );
}
//-----------------------------------------------------------------------------
void WifiDev::setRateInt( uint rate )
{
   m_rate = convIntToString( rate );
}
//-----------------------------------------------------------------------------
void WifiDev::setSensInt( int sens )
{
   m_sens = convIntToString( sens );
}
//-----------------------------------------------------------------------------
uint WifiDev::getFreqInt()
{
   uint freq = convStringToInt( m_freq );
   return freq;
}
//-----------------------------------------------------------------------------
uint WifiDev::getChannelInt()
{
   uint channel = convStringToInt( m_channel );
   return channel;
}
//-----------------------------------------------------------------------------
uint WifiDev::getRateInt()
{
   uint rate = convStringToInt( m_rate );
   return rate;
}
//-----------------------------------------------------------------------------
uint WifiDev::getSensInt()
{
   uint sens = convStringToInt( m_sens );
   return sens;
}
//-----------------------------------------------------------------------------
string WifiDev::convIntToString( int value )
{
   string strVal;
   char conVal[100];
   
   sprintf( conVal, "%d", value );
   strVal += conVal;
   
   return strVal; 
}
//-----------------------------------------------------------------------------
int WifiDev::convStringToInt( string &scrStr )
{
   int length=0;
   
   
   return length; 
}
//-----------------------------------------------------------------------------
