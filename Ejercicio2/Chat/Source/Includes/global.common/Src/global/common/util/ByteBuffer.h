#ifndef _BYTEBUFFER_H
#define _BYTEBUFFER_H

/*
 * Implementaci'on libre de la clase ByteBuffer de Java para C++.
 *
 * 2008/10/16 aya
 * Esta clase est'a orientada a ser utilizada en la conversi'on desde y hacia
 * datos serializados.
 *
 * 2008/10/15 aya
 * Agrega asignaci'on autom'atica del endian de la m'aquina por defecto en la
 * creaci'on.
 *
 * 2008/10/24 aya
 * Agrega m'etodo skip para saltar bytes en la lectura
 */

#include "Buffer.h"
#include "endian.h"

namespace util
{

  /**
   * Estructura conversora auxiliar a prop'osito de la eventual conversi'on que
   * depende del orden (Big Endian o Little Endian) definido para el buffer.
   */
  union RUglyConverter;

  /**
   * Implementaci'on C++ de ByteBuffer al modo del de Java.
   * Como el de Java no tiene constructores directos si no s'olo a trav'es del
   * resultado de llamadas est'aticas de la clase.
   */
  class ByteBuffer : public Buffer
  {
  private:

    unsigned char* m_hb;
    int m_offset;
    bool m_ownsMem;

    /**
     * El endian asignado a este <code>ByteBuffer</code>. Este endian no es
     * necesariamente el nativo de la m'aquina, que es una de las gracias de
     * esta clase.
     */
    Endian::endian m_endian;
//    bool m_bigEndian;

    /**
     * Constructor privado de esta clase.
     */
    ByteBuffer(int mark, int pos, int lim, int cap,
               unsigned char* hb = 0, int offset = 0, bool ownsMem = false);

  public:

    /**
     * Crea un ByteBuffer de la capacidad indicada. La memoria es pose'ida y, por
     * ende, liberada por la clase.
     */
    static ByteBuffer* allocate(int capacity);

    /**
     * Crea un ByteBuffer utilizando la memoria indicada. Esta memoria NO es
     * liberada al destruir el objeto.
     */
    static ByteBuffer* allocate(const void* buffer, int size);

    ~ByteBuffer();

    /*
     * Define el orden que se utilizar'a al escribir valors enteros en el buffer.
     */
//    void setOrderBigEndian() { m_bigEndian = true; }
//    void setOrderLittleEndian() { m_bigEndian = false; }
    void setOrderBigEndian() { m_endian = Endian::bigEndian; }
    void setOrderLittleEndian() { m_endian = Endian::littleEndian; }

    void setEndian(Endian::endian endian) { m_endian = endian; }

    /**
     * Lee un byte del buffer.
     */
    unsigned char get();
    /**
     * Escribe un byte al buffer.
     */
    void put(unsigned char b);

    /**
     * Lee una cantidad de memoria cualquiera del buffer.
     */
    int get(void* dst, int length);
    /**
     * Escribe una cantidad de memoria cualquiera al buffer.
     */
    void put(const void* src, int length, int offset = 0);
    /**
     * Escribe el contenido actual de un ByteBuffer al buffer.
     */
    void put(ByteBuffer* src, int len = 0);

    /**
     * Salta el largo indicado en la lectura.
     */
    void skip(int length);
    /**
     * Escribe el caracter dado el n'umero de veces indicado.
     */
    void fill(int c, int length);

    /**
     * Retorna verdadero si posee un arreglo.
     * TODO: explicar
     */
    bool hasArray() { return (m_hb != 0); }

    /**
     * Return the buffer where the bytebuffer works.
     * THIS IS NOT necesarily where the bytebuffer will make its next get
     * operation.
     *
     * @deprecated use storage to get the buffer and actualData to get a pointer
     *             to where the next get operation will act.
     */
    void* array() const { return m_hb; }

    const void* storage() const { return m_hb; }

    /**
     * Added as a "safe" way to recover the data where the bytebuffer actually
     * is pointed out.
     */
    const void* actualData() const { return m_hb + position(); }

  protected:

    void sequenceRead(RUglyConverter* cnv, int length);
    void sequenceWrite(const RUglyConverter* cnv, int length);

  public:

    /*
     * Todos estos son m'etodos varios para leer y escribir tipos conocidos de
     * datos.
     */

    short int getShort();
    void putShort(short int value);

    int getInt();
    void putInt(int value);

    long int getLong();
    void putLong(long int value);

    long long getLongLong();
    void putLongLong(long long value);

    unsigned short int getWord();
    void putWord(unsigned short int value);

    unsigned int getUnsigned();
    void putUnsigned(unsigned int value);

    unsigned long int getLongWord();
    void putLongWord(unsigned long int value);

    float getFloat();
    void putFloat(float value);

    double getDouble();
    void putDouble(double value);

  public:

    /*
     * El siguiente set de conversores NO ES EST'ANDAR
     */

    /**
     * Read and return a string.
     * The result MUST be freed by the caller; use something like the following:
     *
     *   ...
     *   char* s = bb->getstring();
     *   // use s
     *   delete [] s;
     *   ...
     */
    char* getString();
    void putString(const char* value);

  };

}

#endif
