//////////////////////////////////////////////////////////////////////////////
//
// File         :  netdevice.h
// Class        :  NetDevice
// Description  :  This class handels the information of a network device like
//                 name, address, mask etc.
// Author       :  Juergen P. Messerer, juergen.messerer@freesurf.ch
// Project      :  Configuration Tool
// Release Date :  21th September 2002
// Revised on   :
// License      :  Gnu Public License (GPL)
//
// Copyright(c) Juergen P. Messerer 2002.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef NETDEVICE_H_
#define NETDEVICE_H_

#include <string>
#include <vector>


class NetDevice
{
  public:
    NetDevice();
    NetDevice( const char *devName);
 
    void setDevInfo( string devInfo );
    void setDevName( string devName )       { m_devName = devName;    }
    void setInetAddrStr( string inetAddr )  { m_inetAddr = inetAddr;  }
    void setBcastAddrStr( string bcastAddr ){ m_bcastAddr = bcastAddr;}
    void setMaskStr( string mask )          { m_mask = mask;          }
    void setHwAddrStr( string hwAddr )      { m_hwAddr = hwAddr;      }

    void setInetAddrInt( vector<int> inetAddr );  
    void setBcastAddrInt( vector<int> bcastAddr );
    void setMaskInt( vector<int> mask );   
    void setHwAddrInt( vector<int> hwAddr );  

    string getDevInfo()  { return m_devInfo;   }
    string getDevName()  { return m_devName;   }
    string getInetAddrStr() { return m_inetAddr;  }
    string getBcastAddrStr(){ return m_bcastAddr; }
    string getMaskStr()     { return m_mask;      }
    string getHwAddrStr()   { return m_hwAddr;    }
    
    uint getInetAddrInt( vector<int> &inetTokens ); 
    uint getBcastAddrInt( vector<int> &bcastTokens );
    uint getMaskInt( vector<int> &maskTokens );     
    uint getHwAddrInt( vector<int> &hwTokens );

 private:
    string convIntToString( vector<int> valTok, const char *delim = "." );
    uint convStringToInt( string &scrStr, vector<int> &tokens,
			  const char *delim = ".", int base = 10 );
    
    string m_devInfo;
    string m_devName;
    string m_inetAddr;
    string m_bcastAddr;
    string m_mask;
    string m_hwAddr;
};
#endif //NETDEVICE_H_
