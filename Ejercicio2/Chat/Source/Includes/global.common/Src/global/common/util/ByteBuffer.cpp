#include "ByteBuffer.h"

#include "endian.h"

#include <stdexcept>

//#include <assert.h>
#include <stdlib.h>
#include <string.h>

namespace util
{

  /*
   * Las conversiones se ejecutan dependiendo del formato natural de la
   * plataforma en la que se est'a corriendo y del formato destino definido para
   * el ByteBuffer.
   * Este valor se obtiene a través de la unidad Endian.
   *
   * @deprecated 2008-10-16
   * Este valor no es conocido como obtenerlo en forma expedita, por lo que se
   * define artificialmente aqu'i.
   * Hasta el momento todas nuestras plataformas corren sobre Intel con formato
   * natural LittleEndian.
   *
  #define PC
   *
   */

  union RUglyConverter
  {
    short int si;
    int i;
    long li;
    long long lli;
    float f;
    double d;

    unsigned short int w;
    unsigned int ui;
    unsigned long int lw;

    unsigned char arr[64];

    void applyOrder(int size, Endian::endian endianness);
  };

  void RUglyConverter::applyOrder(int size, Endian::endian endianness)
  {
    /* Se hace swapping sólo si el endian definido para el ByteBuffer es
     * distinto al de la máquina, porque el endian de lo que viene siempre tiene
     * origen en un tipo nativo de la máquina.
     *
  #ifdef PC
    if (bigEndian)
    {
      for (int ndx=0; ndx < size / 2; ndx++)
      {
        unsigned char by = arr[ndx];
        arr[ndx] = arr[size - ndx - 1];
        arr[size - ndx - 1] = by;
      }
    }
  #endif
     *
     */
    if (endianness != Endian::localMachineEndian())
    {
      for (int ndx=0; ndx < size / 2; ndx++)
      {
        unsigned char by = arr[ndx];
        arr[ndx] = arr[size - ndx - 1];
        arr[size - ndx - 1] = by;
      }
    }
  }

  ByteBuffer::ByteBuffer(int mark, int pos, int lim, int cap,
                         unsigned char* hb, int offset, bool ownsMem)
    : Buffer(mark, pos, lim, cap), m_endian(Endian::bigEndian)
  {
    m_hb = hb;
    m_offset = offset;
    m_ownsMem = ownsMem;

//    m_endian = Endian::bigEndian;

    /*
     * Not a very good decision. Is better to use BIG ENDIAN because is more
     * popular.
     *
    m_endian = Endian::localMachineEndian();
     */
//    m_bigEndian = Endian::isBigEndian();
  }

  ByteBuffer* ByteBuffer::allocate(int capacity)
  {
    if (!(capacity > 0))
      throw std::invalid_argument("invalid argument");
    return new ByteBuffer(-1, 0, capacity, capacity,
                          new unsigned char[capacity],
//                          (unsigned char*)(::malloc(capacity)),
                          0, true);
  }

  ByteBuffer* ByteBuffer::allocate(const void* buffer, int size)
  {
    if (!(size >= 0))
      throw std::invalid_argument("invalid argument");
    return new ByteBuffer(-1, 0, size, size, (unsigned char*)buffer, 0, false);
  }

  ByteBuffer::~ByteBuffer()
  {
    if (m_ownsMem)
      delete [] m_hb;
//      ::free(m_hb);
  }

  unsigned char ByteBuffer::get()
  {
    if (!(remaining() > 0))
      throw std::underflow_error("underflow error");
    unsigned char result = m_hb[position()];
    setPosition(position() + 1);

    return result;
  }

  void ByteBuffer::put(unsigned char b)
  {
    if (!(remaining() > 0))
      throw std::overflow_error("overflow error");
    m_hb[position()] = b;
    setPosition(position() + 1);
  }

  int ByteBuffer::get(void* dst, int length)
  {
    int cpylen = length;
    if (remaining() < length)
      cpylen = remaining();
    memcpy(dst, m_hb + position(), cpylen);
    setPosition(position() + cpylen);

    return cpylen;
  }

  void ByteBuffer::skip(int length)
  {
    setPosition(position() + length);
  }

  void ByteBuffer::fill(int c, int length)
  {
    if (!(remaining() >= length))
      throw std::overflow_error("overflow error");
    memset(m_hb + position(), c, length);
    setPosition(position() + length);
  }

  void ByteBuffer::put(const void* src, int length, int offset)
  {
    if (!(remaining() >= length))
      throw std::overflow_error("overflow error");
    memcpy(m_hb + position(), (unsigned char*)src + offset, length);
    setPosition(position() + length);
  }

  void ByteBuffer::put(ByteBuffer* src, int len)
  {
    if (!(src != this))
      throw std::invalid_argument("invalid argument");
    if (len == 0)
      len = src->remaining();
    if (!(remaining() >= len))
      throw std::overflow_error("overflow error");

    src->get(m_hb + position(), len);
    setPosition(position() + len);
  }

  void ByteBuffer::sequenceRead(RUglyConverter* cnv, int length)
  {
    if (!(position() + length <= limit()))
      throw std::underflow_error("underflow error");
    get(cnv->arr, length);
  }

  void ByteBuffer::sequenceWrite(const RUglyConverter* cnv, int length)
  {
    if (!(remaining() >= length))
      throw std::overflow_error("overflow error");
    put(cnv->arr, length);
  }


  short int ByteBuffer::getShort()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.si));
    cnv.applyOrder(sizeof(cnv.si), m_endian);

    return cnv.si;
  }
  void ByteBuffer::putShort(short int value)
  {
    RUglyConverter cnv;
    cnv.si = value;
    cnv.applyOrder(sizeof(cnv.si), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.si));
  }

  int ByteBuffer::getInt()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.i));
    cnv.applyOrder(sizeof(cnv.i), m_endian);

    return cnv.i;
  }
  void ByteBuffer::putInt(int value)
  {
    RUglyConverter cnv;
    cnv.i = value;
    cnv.applyOrder(sizeof(cnv.i), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.i));
  }

  long int ByteBuffer::getLong()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.li));
    cnv.applyOrder(sizeof(cnv.li), m_endian);

    return cnv.li;
  }
  void ByteBuffer::putLong(long int value)
  {
    RUglyConverter cnv;
    cnv.li = value;
    cnv.applyOrder(sizeof(cnv.li), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.li));
  }

  long long ByteBuffer::getLongLong()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.lli));
    cnv.applyOrder(sizeof(cnv.lli), m_endian);

    return cnv.lli;
  }
  void ByteBuffer::putLongLong(long long value)
  {
    RUglyConverter cnv;
    cnv.lli = value;
    cnv.applyOrder(sizeof(cnv.lli), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.lli));
  }

  unsigned short int ByteBuffer::getWord()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.w));
    cnv.applyOrder(sizeof(cnv.w), m_endian);

    return cnv.w;
  }
  void ByteBuffer::putWord(unsigned short int value)
  {
    RUglyConverter cnv;
    cnv.w = value;
    cnv.applyOrder(sizeof(cnv.w), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.w));
  }

  unsigned int ByteBuffer::getUnsigned()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.ui));
    cnv.applyOrder(sizeof(cnv.ui), m_endian);

    return cnv.ui;
  }
  void ByteBuffer::putUnsigned(unsigned int value)
  {
    RUglyConverter cnv;
    cnv.ui = value;
    cnv.applyOrder(sizeof(cnv.ui), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.ui));
  }

  unsigned long int ByteBuffer::getLongWord()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.lw));
    cnv.applyOrder(sizeof(cnv.lw), m_endian);

    return cnv.lw;
  }
  void ByteBuffer::putLongWord(unsigned long int value)
  {
    RUglyConverter cnv;
    cnv.lw = value;
    cnv.applyOrder(sizeof(cnv.lw), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.lw));
  }

  float ByteBuffer::getFloat()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.f));
    cnv.applyOrder(sizeof(cnv.f), m_endian);

    return cnv.f;
  }
  void ByteBuffer::putFloat(float value)
  {
    RUglyConverter cnv;
    cnv.f = value;
    cnv.applyOrder(sizeof(cnv.f), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.f));
  }

  double ByteBuffer::getDouble()
  {
    RUglyConverter cnv;
    sequenceRead(&cnv, sizeof(cnv.d));
    cnv.applyOrder(sizeof(cnv.d), m_endian);

    return cnv.d;
  }
  void ByteBuffer::putDouble(double value)
  {
    RUglyConverter cnv;
    cnv.d = value;
    cnv.applyOrder(sizeof(cnv.d), m_endian);
    sequenceWrite(&cnv, sizeof(cnv.d));
  }

  char* ByteBuffer::getString()
  {
    int len = getShort();
    if (len == 0)
      return 0;

//    char* str = (char*)(std::malloc(len + 1));
    char* str = new char[len + 1];
    get(str, len);
    *(str + len) = 0;

    return str;
  }

  void ByteBuffer::putString(const char* value)
  {
    if (value != 0)
    {
      putShort(strlen(value));
      put(value, strlen(value));
    }
    else
      putShort(0);
  }

}
