//////////////////////////////////////////////////////////////////////////////
//
// File         :  wifiDev.h
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

#ifndef WIFIDEVICE_H_
#define WIFIDEVICE_H_

#include <string>
#include <vector>


class WifiDev
{
  public:
    WifiDev();
    WifiDev( const char *devName);
 
    void setDevInfo( string devInfo );
    void setDevName( string devName )   { m_devName = devName; }
    void setESSID( string essId )       { m_essId = essId;     }
    void setMode( string mode )         { m_mode = mode;       }
    
    void setFreqStr( string freq )      { m_freq = freq;       }
    void setFreqInt( uint freq ); 
    void setChannelStr( string channel ){ m_channel = channel;}
    void setChannelInt( uint channel );
    void setRateStr( string rate )      { m_rate = rate;       }
    void setRateInt( uint rate );
    void setSensStr( string sens )      { m_sens = sens;       }
    void setSensInt( int sens );

    string getDevInfo()    { return m_devInfo; }
    string getDevName()    { return m_devName; }
    string getESSID()      { return m_essId;   }
    string getMode()       { return m_mode;    }
    string getFreqStr()    { return m_freq;    }
    uint getFreqInt();
    string getChannelStr() { return m_channel; }
    uint getChannelInt();
    string getRateStr()    { return m_rate;    }
    uint getRateInt(); 
    string getSensStr()    { return m_sens;    }
    uint getSensInt();    

 private:
    int   convStringToInt( string &scrStr );
    string convIntToString( int value );

    string m_devInfo;
    string m_devName;
    string m_essId;
    string m_mode;
    string m_freq;
    string m_channel;
    string m_rate;
    string m_sens;
};
#endif //WIFIDEVICE_H_
