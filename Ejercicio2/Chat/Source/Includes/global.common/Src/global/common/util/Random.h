#ifndef _RANDOM_H_
#define _RANDOM_H_

/*
 * Feb 1, 2009 - ayachan
 */

#include <string>

namespace util
{

  /**
   * Just some utilities to generate random numbers.
   */
  class Random
  {
  public:
    static void initialize(unsigned int seed);

    /**
     * Generate a random number [mean-desv,mean+desv].
     */
    static int random(int mean, int desv);

    /**
     * Generate a random number [0,value).
     */
    static int random(int value);

    /**
     * Fill the given block with random bytes.
     */
    static void random(void* data, int offset, int len);

    /**
     * @return A random name with the given number of silabes.
     */
    static std::string randomName(int ns, bool inverse = false);

  private:
    static const std::string VOCALS;
    static const std::string CONSONANTS;

    static std::string randomSilabe(bool inverse);
  };

} // namespace

#endif /* _RANDOM_H_ */
