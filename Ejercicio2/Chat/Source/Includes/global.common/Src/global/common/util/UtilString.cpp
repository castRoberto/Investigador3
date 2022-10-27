/*
 * Aug 5, 2009 - ayachan
 */

#include "UtilString.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <algorithm>

namespace util
{
  int UtilString::split(const std::string& text, std::vector<std::string>& tokens)
  {
    return split(text, tokens, " \t");
  }
  
  int UtilString::split(const std::string& text, std::vector<std::string>& tokens,
                        const std::string& delimiters)
  {
    tokens.clear();

    unsigned start = text.find_first_not_of(delimiters);
    unsigned end = text.find_first_of(delimiters, start);

    while (end <= text.length())
    {
      std::string token = text.substr(start, end - start);
      tokens.push_back(token);
      start = text.find_first_not_of(delimiters, end);
      end = text.find_first_of(delimiters, start);

    }
    if (start != text.length())
    {
      std::string token = text.substr(start);
      tokens.push_back(token);
    }

    return tokens.size();
  }

  const std::string UtilString::trim(const std::string& s)
  {
    std::string result;

    std::string delimiters = " \t";
    unsigned start = s.find_first_not_of(delimiters);
    unsigned end   = s.find_last_not_of(delimiters);
    if (start < s.length())
      result = s.substr(start, end - start + 1);
    return result;
  }

  const std::string UtilString::format(const char* fmt, ...)
  {
    // el formato del texto
    char text[MAX_LENGTH_TEXT];
    memset(text, 0, sizeof(text));

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(text, fmt, argptr);
    va_end(argptr);

    std::string result = text;
    return result;
  }
  
  int UtilString::smartSplit(const std::string& text, 
                              std::vector<std::string>& tokens,
                              const std::string& delimiter)
  {
    tokens.clear();
    size_t pos = 0;
    std::string token;
    int count = 0;
    std::string auxText(text);
    while ((pos = auxText.find(delimiter)) != std::string::npos)
    {
      token = auxText.substr(0, pos);
      tokens.push_back(token);
      auxText.erase(0, pos + delimiter.length());
      count++;
    }
    tokens.push_back(auxText);
    count++;
    return count;
  }
  
  const unsigned char UtilString::toUpperCaseChar(const unsigned char Char) 
  {
    if (Char >= 97 && Char <= 122)
    {
      return (unsigned char) (Char - 32);
    }
    else
    {
      return Char;
    }
  }
  
  const unsigned char UtilString::toLowerCaseChar(const unsigned char Char) 
  {
    if (Char >= 65 && Char <= 90)
    {
      return (unsigned char) (Char + 32);
    }
    else
    {
      return Char;
    }
  }
  
  const std::string UtilString::toUpperCase(const std::string& s)
  {
    std::string ret;
    int len = s.length();
    const char * data = s.c_str();
    char * data_converted = new char[len];
    for(int i = 0;i<len;i++)
    {
      data_converted[i] = toUpperCaseChar((unsigned char)data[i]);
    }
    ret.assign(data_converted,(unsigned int)len);
    delete [] data_converted;
    return ret;
  }
  
  const std::string UtilString::toLowerCase(const std::string& s)
  {
    std::string ret;
    int len = s.length();
    const char * data = s.c_str();
    char * data_converted = new char[len];
    for(int i = 0;i<len;i++)
    {
      data_converted[i] = toLowerCaseChar((unsigned char)data[i]);
    }
    ret.assign(data_converted,(unsigned int)len);
    delete [] data_converted;
    return ret;
  }

  const std::string UtilString::nltrim(const std::string& s)
  {
    static const unsigned int CR = 0x0d;
    static const unsigned int LF = 0x0a;
    static const unsigned int TAB = 0x09;

    std::string result;
    for (unsigned int n=0; n<s.length(); n++)
    {
      if ( (unsigned int)s.at(n) == CR)
      {
        result.append("<cr>");
      }
      else if ( (unsigned int)s.at(n) == LF)
      {  
        result.append("<lf>");
      }
      else if ( (unsigned int)s.at(n) == TAB)
      {
        result.append("<tab>");
      }
      else if ( ( (unsigned int)s.at(n) < 32) || ( (unsigned int)s.at(n) > 127))
      {
        result.append(UtilString::format("<%2.2x>", (unsigned int)s.at(n)));
      }
      else
      {
        result.append(1, s.at(n));
      }
    }
    return result;
  }

  struct util_string_upper
  {
    int operator()(int c)
    {
      return std::toupper( (unsigned char)c);
    }
  };

  const std::string UtilString::toupper(const std::string& s)
  {
    std::string s_copy(s);
    std::transform(s_copy.begin(), s_copy.end(), s_copy.begin(), util_string_upper());
    return s_copy;
  }
  
  char UtilString::checkSum(std::string s)
  {
    //  Metodo que calcula checksum sobre una cadena std::string
    char check = s.at(0);
    for (unsigned c = 1; c < s.length(); c++)
    {
      char tc = s.at(c);
      check = char(check ^ tc);
    }
    return check;
  }
  
  bool UtilString::isNumeric(const std::string& s, const char& decimalSeparator)
  {
    bool val = false;
    bool isCommaChecked = false;
    std::string::const_iterator it = s.begin();
    while(it != s.end())
    {
      bool isComma = (char)*it == decimalSeparator;
      val = std::isdigit(*it) || (isComma && isCommaChecked == false);
      if(isComma && isCommaChecked == true)
      {
        break;
      }
      else if(isComma && isCommaChecked == false)
      {
        isCommaChecked = isComma;
      }
      ++it;
    }
    return val;
  }

  /**
   * @brief Metodo que permite validar si una cadena string contiene solo valores enteros positivos
   * @param aValString : valor del string a validar
   * @return val: valor booleano
   */
  bool UtilString::isStringNumeric(std::string aValString)
  {
    bool val = false;
	int sizeValNum = aValString.size();

	for(int i = 0; i < sizeValNum; i++)
	{
	  if(aValString[i] >= 48 && aValString[i] <= 57)
	  {
	    val = true;
	  }
	  else
	  {
		val = false;
		break;
	  }
	}

	return val;
  }


  bool UtilString::isValidIpV4(std::string ip)
  {
    std::string partIp;
    partIp.clear();
    bool result = true;
    int countDot = 0;
    for (unsigned i = 0; i < ip.length() && result == true; i++)
    {
      char ipChar = ip[i];
      if (ipChar >= '0' && ipChar <= '9')
      {
        partIp.append(1,ipChar);
      }else if (ipChar == '.')
      {
        countDot++;
        if (partIp.length() <= 0)
        {
          result = false;
        }
        else
        {
          int val = atoi(partIp.c_str());
          if (val <0 || val > 255)
          {
            result = false;
          }
        }
        partIp.clear();
      }else
      {
        result = false;
      }
    }

    if (countDot != 3)
    {
      result = false;
    }

    if (partIp.length() <= 0)
    {
      result = false;
    }
    else
    {
      int val = atoi(partIp.c_str());
      if (val <0 || val > 255)
      {
        result = false;
      }
    }
    return result;
  }


  char UtilString::hexToChar(std::string data)
  {
  	char result = 0;
  	int lowValue = valueDigit(data[1]);
  	int highValue = valueDigit(data[0]);
  	result = (char)(lowValue + ( 16 * highValue));
  	return result;
  }

  char UtilString::valueDigit(char digit) {
    int result = 0;
    if (digit >= 48 && digit <= 57) { // 0 - 9
      //result = (48 - digit);
      result = (digit - 48);
    } else if (digit >= 65 && digit <= 70) { // A - F
      int asciiOffset = 55;
      result = (digit - asciiOffset);
    } else if (digit >= 97 && digit <= 102) { //a -f
      int asciiOffset = 87;
      result = (digit - asciiOffset);
    }
    return result;
  }


} // namespace

