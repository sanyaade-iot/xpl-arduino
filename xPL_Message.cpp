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
 
#include "xPL_Message.h"

xPL_Message::xPL_Message()
{
    command = NULL;
	command_count = 0;
}

xPL_Message::~xPL_Message()
{
	if(command != NULL)
	{
		free(command);
	}
}

void xPL_Message::SetSource(char * _vendorId, char * _deviceId, char * _instanceId)
{
	memcpy(source.vendor_id, _vendorId, XPL_VENDOR_ID_MAX);
	memcpy(source.device_id, _deviceId, XPL_DEVICE_ID_MAX);
	memcpy(source.instance_id, _instanceId, XPL_INSTANCE_ID_MAX);
}

void xPL::SetTarget(const PROGMEM char * _vendorId, const PROGMEM char * _deviceId, const PROGMEM char * _instanceId)
{
	memcpy_P(target.vendor_id, _vendorId, XPL_VENDOR_ID_MAX);
	if(_deviceId != NULL) memcpy_P(target.device_id, _deviceId, XPL_DEVICE_ID_MAX);
	if(_instanceId != NULL) memcpy_P(target.instance_id, _instanceId, XPL_INSTANCE_ID_MAX);
}


bool xPL_Message::AddCommand(char* _name, char* _value)
{
	// Maximun command reach
	// To avoid oom, we arbitrary accept only XPL_MESSAGE_COMMAND_MAX command
	if(command_count > XPL_MESSAGE_COMMAND_MAX)
		return false;
		
	command = (struct_command*)realloc ( command, (++command_count) * sizeof(struct_command) );

	struct_command newcmd;
	memcpy(newcmd.name, _name, 16);
	memcpy(newcmd.value, _value, 128);

	command[command_count-1] = newcmd;
	
	return true;
}

char *xPL_Message::toString()
{
  char message_buffer[XPL_MESSAGE_BUFFER_MAX];
  int pos;

  clearStr(message_buffer);

  switch(type)
  {
    case (XPL_CMND):
      pos = sprintf_P(message_buffer, PSTR("xpl-cmnd"));
      break;
    case (XPL_STAT):
      pos = sprintf_P(message_buffer, PSTR("xpl-stat"));
      break;
    case (XPL_TRIG):
      pos = sprintf_P(message_buffer, PSTR("xpl-trig"));
      break;
  }

  pos += sprintf_P(message_buffer + pos, PSTR("\n{\nhop=1\nsource=%s-%s.%s\ntarget="), source.vendor_id, source.device_id, source.instance_id);

  if(memcmp(target.vendor_id,"*", 1) == 0)  // check if broadcast message
  {
    pos += sprintf_P(message_buffer + pos, PSTR("*\n}\n"));
  }
  else
  {
	pos += sprintf_P(message_buffer + pos, PSTR("%s-%s.%s\n}\n"),target.vendor_id, target.device_id, target.instance_id);
  }

  pos += sprintf_P(message_buffer + pos, PSTR("%s.%s\n{\n"),schema.class_id, schema.type_id);

  for (byte i=0; i<command_count; i++)
  {
	pos += sprintf_P(message_buffer + pos, PSTR("%s=%s\n"), command[i].name, command[i].value);
  }

  sprintf_P(message_buffer + pos, PSTR("}\n"));

  return message_buffer;
}

bool xPL_Message::IsSchema(char* _classId, char* _typeId)
{
//  const char *classId = (const char PROGMEM *)_classId;
//  const char *typeId = (const char PROGMEM *)_typeId;

  if (memcmp(schema.class_id, _classId, 8) == 0)
  {
    if (memcmp(schema.type_id, _typeId, 8) == 0)
    {
      return true;
    }   
  }  

  return false;
}
