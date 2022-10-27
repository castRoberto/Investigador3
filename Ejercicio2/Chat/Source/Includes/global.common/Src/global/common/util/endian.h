#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <iostream>

namespace util
{

  class Endian
  {
  public:
    enum endian { endianUnknown, bigEndian, littleEndian };
  
    static endian localMachineEndian()
    {
      // (asumimos enteros de cuatro bytes - aunque en realidad no importa porque
      //  se preguna s'olo por el primer byte)
      union
      {
        unsigned int ui;
        unsigned char m[16];
      } u;
    
      u.ui = 0x12345678;
      endian e = endianUnknown;
      if ( (u.m[0] == 0x12) && (u.m[sizeof(u.ui)-1] == 0x78))
        e = bigEndian;
      else if ( (u.m[0] == 0x78) && (u.m[sizeof(u.ui)-1] == 0x12))
        e = littleEndian;
      else
        std::cerr << "Can't deduce local machine endian" << std::endl;
  
      return e;
    }
  
    static bool isBigEndian() { return (localMachineEndian() == bigEndian); }
    static bool isLittleEndian() { return (localMachineEndian() == littleEndian); }
  
  };

}

#endif /*_ENDIAN_H_*/
