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

#include <stdio.h>
#include "netdevice.h"

string devKeyWord[] = { "HWaddr ", "inet addr:", "Bcast:", "Mask:" };
int nrKeyWords  = (sizeof(devKeyWord) / sizeof(devKeyWord[0]));
enum { HWADDR=0, INETADDR, BCAST, MASK};

NetDevice::NetDevice()
{
   m_devInfo = "";
   m_devName = "";
   
   m_inetAddr = "... ... ... ...";
   m_bcastAddr = "... ... ... ...";
   m_mask = "... ... ... ...";
   m_hwAddr = "00:00:00:00:00:00";
   
}
//-----------------------------------------------------------------------------
NetDevice::NetDevice( const char* devName )
{
    m_devInfo = "";
    m_devName = devName;
    
    m_inetAddr = "... ... ... ...";
    m_bcastAddr = "... ... ... ...";
    m_mask = "... ... ... ...";
    m_hwAddr = "00:00:00:00:00:00";
}
//-----------------------------------------------------------------------------
void NetDevice::setDevInfo( string devInfo )
{
   m_devInfo = devInfo;
   string::size_type startPos, endPos, length;
   string subStr;
   
   for(int i = 0; i < nrKeyWords; ++i )
   {
       startPos = devInfo.find( devKeyWord[i] );
              
       if( startPos != string::npos )
       {
	  length = devKeyWord[i].length();
	  endPos = devInfo.find( " ", startPos+length );
	  subStr = devInfo.substr( startPos+length, 
				   endPos-startPos-length );

	  switch( i )
	  {
	    case HWADDR: { m_hwAddr = subStr; }
	    break;

	    case INETADDR: { m_inetAddr = subStr; }
	    break;

	    case BCAST: { m_bcastAddr = subStr; }
	    break;
	    
	    case MASK: { m_mask = subStr; }
	    break;
	    default:
	      cout << "Error: No such case!" << endl;
	      break;
	  } //switch
       } // if(..)
   } // for(..)
}
//-----------------------------------------------------------------------------
void NetDevice::setInetAddrInt( vector<int> inetAddr )
{
   m_inetAddr = convIntToString( inetAddr, "." );
}
//-----------------------------------------------------------------------------
void NetDevice::setBcastAddrInt( vector<int> bcastAddr )
{
   m_bcastAddr = convIntToString( bcastAddr, "." );
}
//-----------------------------------------------------------------------------
void NetDevice::setMaskInt( vector<int> mask )
{
   m_mask = convIntToString( mask, "." );
}
//-----------------------------------------------------------------------------
void NetDevice::setHwAddrInt( vector<int> hwAddr )
{
   m_hwAddr = convIntToString( hwAddr, ":" );
}
//-----------------------------------------------------------------------------
uint NetDevice::getInetAddrInt( vector<int> &inetTokens )
{
   uint length = convStringToInt( m_inetAddr, inetTokens, "." );
   return length;
}
//-----------------------------------------------------------------------------
uint NetDevice::getBcastAddrInt( vector<int> &bcastTokens )
{
   uint length = convStringToInt( m_bcastAddr, bcastTokens, "." );
   return length;
}
//-----------------------------------------------------------------------------
uint NetDevice::getMaskInt( vector<int> &maskTokens )
{
   uint length = convStringToInt( m_mask, maskTokens, "." );
   return length;
}
//-----------------------------------------------------------------------------
uint NetDevice::getHwAddrInt( vector<int> &hwTokens )
{
   uint length = convStringToInt( m_hwAddr, hwTokens, ":", 16 );
   return length;
}
//-----------------------------------------------------------------------------
string NetDevice::convIntToString( vector<int> valTok, const char *delim )
{
   string strVal;
   char conVal[100];

   vector<int>::iterator itr;
   
   for( itr = valTok.begin(); itr != valTok.end(); ++itr )
   {
      sprintf( conVal, "%d", *itr );
      strVal += conVal;
      if( itr < (valTok.end()-1) )
	strVal += delim;
   }
   return strVal; 
}
//-----------------------------------------------------------------------------
uint NetDevice::convStringToInt( string &scrStr, vector<int> &tokens,
				 const char *delim, int base )
{
   int length=0;
   string subStr;
   
   // Skip delimiters at beginning
   string::size_type lastPos = scrStr.find_first_not_of( delim, 0 );

   // Find first "non-delimiter"
   string::size_type pos = scrStr.find_first_of( delim, lastPos );


   while(  string::npos != lastPos )
   {
      ++length;
      
      // Found a token, add it to the vector as int
      subStr = scrStr.substr( lastPos, pos -lastPos );
      tokens.push_back( strtol( subStr.c_str(), NULL, base ) );
     
      // Skip delimiters. Note the "not_of"
      lastPos = scrStr.find_first_not_of( delim, pos );

      // Find next "non-delimiter"
      pos = scrStr.find_first_of( delim, lastPos );
   }
   
   return length; 
}
//-----------------------------------------------------------------------------
