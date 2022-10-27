/** Define clase utilitaria de despliegue de bloques de data.
 */
#ifndef _HEXLINE_H
#define _HEXLINE_H

#include <string>

/** Clase utilitaria que permite la generaci�n de l�neas en formato hexadecimal
 * y ASCII. Estas l�neas consideran tres �reas: el offset de la l�nea respecto
 * al bloque, los caracteres en formato hexadecimal y los caracteres en formato
 * ASCII (si son imprimibles).
 * Notar que esta clase no considera la salida misma, s�lo genera las l�neas.
 * Un ejemplo de utilizaci�n es el siguiente:
 *
 *  ...
 *  unsigned char* data = (unsigned char*)block;
 *  int addr = 0;
 *  int len = blklen;
 *  while (len > 0)
 *  {
 *    cout << CHexLine::formatHexLine16(addr, data, len) << endl;
 *    data += 16;
 *    addr += 16;
 *    len -= 16;
 *  }
 *  ...
 *
 */
class CHexLine
{
public:
  /** Genera una l�nea que considera los primeros 8 caracteres del buffer
   * indicado.
   */
  static std::string formatHexLine8(int addr, const void* block, int len);
  /** Genera una l�nea que considera los primeros 16 caracteres del buffer
   * indicado.
   */
  static std::string formatHexLine16(int addr, const void* block, int len);

  /** Genera una l�nea que considera los primeros 'bytesPerLine' caracteres del
   * buffer indicado.
   */
  static std::string formatHexLine(int bytesPerLine, int addr,
                                   const void* block, int len);

  /**
   * Retorna el largo que podr'ia tener una representaci'on hexadecimal.
   */
  static int hexStringLength(int len);

  /**
   * Retorna un string (sobre el buffer dado) que contiene una representaci�n
   * en hexa de los primeros len bytes de data.
   */
  static const char* hexString(char* buffer, int buflen,
                               const void* data, int len);

  /**
   * @return A std::string with the hexadecimal representation of the given data
   *         buffer, including spaces between 4, 8 and 16 bytes.
   */
  static const std::string hexString(const void* data, int len);
  /**
   * @return A plain std::string with the hexadecimal representation of the
   *         given data buffer.
   */
  static const std::string hexString_ws(const void* data, int len);
};


#endif
