
#include "checksum.h"

namespace util
{

  unsigned int checksum32(const void* data, int ofs, int len)
  {
    unsigned int chksum = 0;
    const unsigned char* ucdata = (unsigned char*)data;
    for (int n=0; n<len; n++)
      chksum += ucdata[ofs + n];
    return chksum;
  }

  unsigned int checksum16(const void* data, int ofs, int len)
  {
    unsigned short chksum = 0;
    const unsigned char* ucdata = (unsigned char*)data;
    for (int n=0; n<len; n++)
      chksum += ucdata[ofs + n];
    return chksum;
  }

  unsigned int checksum8(const void* data, int ofs, int len)
  {
    unsigned char chksum = 0;
    const unsigned char* ucdata = (unsigned char*)data;
    for (int n=0; n<len; n++)
      chksum += ucdata[ofs + n];
    return chksum;
  }

}
