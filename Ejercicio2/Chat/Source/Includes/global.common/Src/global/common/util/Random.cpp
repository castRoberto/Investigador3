/*
 * Feb 1, 2009 - ayachan
 */

#include "Random.h"

#include <stdlib.h>

namespace util
{

  void Random::initialize(unsigned int seed)
  {
    srand(seed);
  }

  int Random::random(int mean, int desv)
  {
    double r = 1.0 * rand() / RAND_MAX;
    double v = 2.0 * desv * r - desv + 0.5;
    return (int)(mean + v);
  }

  int Random::random(int value)
  {
    double r = (1.0 * rand()) / (RAND_MAX);
    double v = 1.0 * value * r;
    return (int)v;
  }

  void Random::random(void* data, int offset, int len)
  {
    unsigned char* uchb = (unsigned char*)data;
    for (int n=0; n<len; n++)
      uchb[offset + n] = (unsigned char)(n % 256);
//    for (int n=0; n<len; n++)
//      uchb[offset + n] = (unsigned char)random(256);
  }

  const std::string Random::VOCALS = "AEIOU";
  const std::string Random::CONSONANTS = "BCDFGHJKLMNPQRSTVWXYZ";

  std::string Random::randomSilabe(bool inverse)
  {
    int ndxv = util::Random::random(VOCALS.length());
    int ndxc = util::Random::random(CONSONANTS.length());

    std::string result;
    if (inverse)
    {
      result.append(1, VOCALS.at(ndxv));
      result.append(1, CONSONANTS.at(ndxc));
    }
    else
    {
      result.append(1, CONSONANTS.at(ndxc));
      result.append(1, VOCALS.at(ndxv));
    }

    return result;
  }

  std::string Random::randomName(int ns, bool inverse)
  {
    std::string result;
    for (int n=0; n<ns; n++)
      result.append(Random::randomSilabe(inverse));
    return result;
  }
} // namespace
