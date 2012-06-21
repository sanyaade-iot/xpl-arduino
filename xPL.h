/*
 * xPL.Arduino v0.1, xPL Implementation for Arduino
 *
 * This code is parsing a xPL message stored in 'received' buffer
 * - isolate and store in 'line' buffer each part of the message -> detection of EOL character (DEC 10)
 * - analyse 'line', function of its number and store information in xpl_header memory
 * - check for each step if the message respect xPL protocol
 * - parse each command line
 *
 * Copyright (C) 2012 johan@pirlouit.ch, olivier.lebrun@gmail.com
 * Original version by Gromain59@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef xPL_h
#define xPL_h
 
#include "Arduino.h"
#include "xPL_utils.h"

#define XPL_CMND 1
#define XPL_STAT 2
#define XPL_TRIG 3

#define XPL_MESSAGE_BUFFER_MAX           256
#define XPL_DEFAULT_HEARTBEAT_INTERVAL   5

#define XPL_UDP_PORT 3865

#define XPL_PORT_L  0x19
#define XPL_PORT_H  0xF

// Heartbeat request class definition
#define XPL_HBEAT_REQUEST_CLASS_ID                "hbeat"
#define XPL_HBEAT_REQUEST_TYPE_ID                 "request"
#define XPL_HBEAT_ANSWER_CLASS_ID                 "hbeat"
#define XPL_HBEAT_ANSWER_TYPE_ID                 "basic"  //app, basic


class xPL_Message
{
    public:
        short type;			        // 1=cmnd, 2=stat, 3=trig
        short hop;				// Hop count
        struct_id source;			// source identification
        struct_id target;			// target identification
        struct_xpl_schema schema;
        struct_command *command;
        unsigned short command_count;

        void AddCommand(char*, char*);
        void AddCommand(char*, int);

        xPL_Message();
        ~xPL_Message();

        //void ClearData();

        void Parse(char *message, unsigned short len);
        char *toString();
        
        bool IsSchema(char *class_id, char *type_id);

    private:
      int AnalyseHeaderLine(char *buffer, int line);
      int AnalyseCommandLine(char *buffer, int line);

};


typedef enum {XPL_ACCEPT_ALL, XPL_ACCEPT_SELF, XPL_ACCEPT_SELF_ANY} xpl_accepted_type;
// XPL_ACCEPT_ALL = all xpl messages
// XPL_ACCEPT_SELF = only for me
// XPL_ACCEPT_SELF_ANY = only for me and any (*)

typedef void (*xPLSendExternal)(char*, int);
typedef void (*xPLAfterParseAction)(xPL_Message * message);

class xPL
{
  public:
    struct_id source;  // my source
    unsigned short udp_port;    // default 3865
    unsigned short hbeat_interval;  // default 5
    xpl_accepted_type xpl_accepted;
    
    xPL();
    void Begin(char *vendorid, char *deviceid, char *instanceid);  // define my source
    void Process();
    void ParseInputMessage(char *buffer);
    
    xPLSendExternal SendExternal;
    xPLAfterParseAction AfterParseAction;
    
    void SendMessage(char *buffer);
    void SendMessage(xPL_Message *message);
    
    bool TargetIsMe(xPL_Message * message);
    
    void SetSourceVendorId(char *vendorid);
    void SetSourceDeviceId(char *deviceid);
    void SetSourceInstanceId(char *instanceid);
    void SetHBeatInterval(unsigned short interval);
    void SetUdpPort(unsigned short udpport);
  
  private:
    //void ClearData();
    unsigned long last_heartbeat;  
    void SendHBeat();
    bool CheckHBeatRequest(xPL_Message * message);
};

#endif
