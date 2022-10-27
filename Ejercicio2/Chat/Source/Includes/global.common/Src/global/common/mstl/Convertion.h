#ifndef _CONVERTION_H_
#define _CONVERTION_H_

/**
 * Utility to have a mean to convert from a table in both directions for any
 * two types.
 */
template <class T, class M>
class Convertion
{
public:
  typedef struct { T t; M m; } ConvertionTable;

  Convertion(const ConvertionTable* table, const int size, T noTValue, M noMValue)
    : table(table), size(size), noTValue(noTValue), noMValue(noMValue)
  {}

  M directConvert(const T& t) const
  {
    M m = noMValue;
    for (int n=0; n < size; n++)
      if ( (table + n)->t == t)
      {
        m = (table + n)->m;
        break;
      }
    return m;
  }

  T reverseConvert(const M& m) const
  {
    T t = noTValue;
    for (int n=0; n < size; n++)
      if ( (table + n)->m == m)
      {
        t = (table + n)->t;
        break;
      }
    return t;
  }

private:
  const ConvertionTable* table;
  int size;

  T noTValue;
  M noMValue;
};

#endif /*_CONVERTION_H_*/
