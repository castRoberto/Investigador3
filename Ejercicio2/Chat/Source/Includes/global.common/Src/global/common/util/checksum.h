#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

namespace util
{
  /**
   * Calcula y retorna el checksum del bloque de data indicado.
   */
  unsigned int checksum32(const void* data, int ofs, int len);

  /**
   * Calcula y retorna el checksum del bloque de data indicado.
   */
  unsigned int checksum16(const void* data, int ofs, int len);

  /**
   * Calcula y retorna el checksum del bloque de data indicado.
   */
  unsigned int checksum8(const void* data, int ofs, int len);
}

#endif /*_CHECKSUM_H_*/
