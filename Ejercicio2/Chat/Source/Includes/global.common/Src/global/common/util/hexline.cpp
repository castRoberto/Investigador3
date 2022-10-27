#include "hexline.h"
#include <stdio.h>
#include <ctype.h>

//#if defined(__QNXNTO__)
//#endif
#include <string.h>

#include <iostream>

std::string CHexLine::formatHexLine8(int addr, const void* block, int len)
{
  return formatHexLine(8, addr, block, len);
}

std::string CHexLine::formatHexLine16(int addr, const void* block, int len)
{
  return formatHexLine(16, addr, block, len);
}

std::string CHexLine::formatHexLine(int bytesPerLine, int addr, const void* block, int len)
{
  std::string outAddr, outHex, outChar;
  char buffer[256];
  const unsigned char* data = (const unsigned char*)block;

  // inicializa la sección con la dirección relativa
  sprintf(buffer, "%4.4X%c", addr, '\0');
  outAddr += buffer;

  // completa la línea del largo indicado sacando desde el bloque de data
  for(int i=0; i<bytesPerLine; i++)
  {
    if ( (i % 4) == 0)
      outHex += " ";
    if (i < len)
    {
      sprintf(buffer, "%2.2x%c", *data, '\0');
      outHex += buffer;
//      outChar += ( (*data < 256) && isprint(*data)) ? *data : '.';
      outChar += isprint(*data) ? *data : '.';
      ++data;
    }
    else
    {
      outHex += "  ";
      outChar += " ";
    }
  }
  return outAddr + " " + outHex + "  " + outChar;
}

int CHexLine::hexStringLength(int len)
{
  return len * 2 + (len / 4) + (len / 8) + (len / 16) + 4;
}

const char* INSUFFICIENT = "INSUFFICIENT SPACE";

const char* CHexLine::hexString(char* buffer, int buflen,
                                const void* data, int len)
{
//  int reqlen = len * 2 + (len / 4) + (len / 8) + (len / 16) + 3;
  if (buflen < hexStringLength(len))
    return INSUFFICIENT;

  const unsigned char* cdata = (const unsigned char*)data;
  memset(buffer, 0, buflen);
  for (int n=0; n<len; n++)
  {
    if ( (n % 4) == 0)
      strcat(buffer, " ");
    if ( (n % 8) == 0)
      strcat(buffer, " ");
    if ( (n % 16) == 0)
      strcat(buffer, " ");

    sprintf(buffer + strlen(buffer), "%2.2x", cdata[n]);
  }
  return buffer;
}

const std::string CHexLine::hexString(const void* data, int len)
{
//  std::cout << " ---- [hexString]: " << len << " bytes" << std::endl;
  std::string result;

  const unsigned char* cdata = (const unsigned char*)data;
  char buffer[8];

  for (int n=0; n<len; n++)
  {
    if (n && ( (n % 4) == 0))
      result = result + " ";
    if (n && ( (n % 8) == 0))
      result = result + " ";
    if (n && ( (n % 16) == 0))
      result = result + " ";

    sprintf(buffer, "%2.2x%c", cdata[n], 0x00);
    result = result + buffer;
  }

  return result;
}

const std::string CHexLine::hexString_ws(const void* data, int len)
{
  std::string result;

  const unsigned char* cdata = (const unsigned char*)data;
  char buffer[8];

  for (int n=0; n<len; n++)
  {
    sprintf(buffer, "%2.2x%c", cdata[n], 0x00);
    result = result + buffer;
  }

  return result;
}

/*
var
  i      : integer;
  outAddr,
  outHex,
  outChar: string;
begin { formatHexLine }
  outAddr := format('%4.4X', [addr]);
  outHex := '';
  outChar := '';
  for i := 0 to bpl-1 do
  begin
    if (i mod 4)=0 then
    begin
      outHex := outHex+' ';;
    end; { (i mod 4)=0 }
    if i<len then
    begin
      outHex := format('%s%2.2x', [outHex, TByteArray(line)[i]]);

      if (TByteArray(line)[i]>=ord(' ')) and (TByteArray(line)[i]<=ord(
                           '~')) then
      begin
        outChar := outChar+Chr(TByteArray(line)[i]);;
      end { (TByteArray(line)[i]>=ord(' ')) and (TByteArray(line)[i]<=ord( }
      else
      begin
        outChar := outChar+'.';;
      end;
      { not ((TByteArray(line)[i]>=ord(' ')) and (TByteArray(line)[i]<=ord( };
    end { i<len }
    else
    begin
      outHex := outHex+'  ';
      outChar := outChar+' ';;
    end; { not (i<len) };;
  end; { for i }
  Result := outAddr+' '+outHex+'  '+outChar;;
end; { formatHexLine }
*/
