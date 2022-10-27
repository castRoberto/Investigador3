#ifndef SSERIALIZABLE_H_
#define SSERIALIZABLE_H_

#include "ByteBuffer.h"

/**
 * Interfaz Secure para serializacion de objetos, a diferencia de 
 * Serializable, SSerializable en su metodo deserialize retorna false 
 * en caso de no poder deserializar
 */
namespace util
{

  class SSerializable
  {
    public:
      virtual ~SSerializable();
  
      /**
       * @return el largo de la forma serializada del objeto
       */
      virtual int serializedLen() const = 0;
  
      /**
       * Serializa el objeto en el buffer indicado
       *
       * @param bb El buffer donde se serializara el objeto
       */
      virtual ByteBuffer* serialize() const = 0;
  
      /**
       * Deserializa un objeto del buffer dado.
       *
       * @return false si no es posible deserializar el objeto y true si es posible
       * @param bb El buffer de donde se obtiene la data del objeto.
       */
      virtual bool deserialize(ByteBuffer* bb) = 0;
  };

} // namespace

#endif /*SSERIALIZABLE_H_*/
