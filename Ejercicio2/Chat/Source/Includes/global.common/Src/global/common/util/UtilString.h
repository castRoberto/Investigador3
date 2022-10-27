/**_____________________                                            
 *Informacion General   \-------------------------------------------
 *==================================================================
 * Proyecto             : SC1633 - LINK-CO                          
 * Modulo               : UTIL                      
 * Autor                : Alejandro Yachan                      
 * Email                : ayachan@sisdef.cl                        
 * Copyright (C) 2016     SISDEF Ltda/COTECMAR    Quintero, Chile   
 *________________                                                  
 *Funcionalidad   \-------------------------------------------------
 *==================================================================
 * Descripcion         :                   
 * Algunas utilidaes para strings
 *_____________________                                             
 *Control de Cambios   \--------------------------------------------
 *==================================================================
 * Autor  : <JAC>       Fecha  : <03/03/2016>                       
 * Descripcion         : 
 * Reformateo y traduccion                
 *==================================================================
 */

#ifndef _UTILSTRING_H_
#define _UTILSTRING_H_

/*
 * Aug 5, 2009 - ayachan
 */

#include <string>
#include <vector>

namespace util
{

  /**
   * Just some string utilities.
   */
  struct UtilString
  {

    /**
     * Divide el "string" dado en "substrings" segun los delimitadores especificados.
     * Los "substrings" se retornan en el vector dado (que sera reseteado).
     */
    static int split(const std::string& text, std::vector<std::string>& tokens);
    static int split(const std::string& text, std::vector<std::string>& tokens,
                     const std::string& delimiters);
    //Divide el string en substrings segun el delimitador dado (la totalidad
    //del delimitador). 
    static int smartSplit(const std::string& text, std::vector<std::string>& tokens,
                           const std::string& delimiter);

    static const std::string trim(const std::string& s);

    static const int MAX_LENGTH_TEXT = 4096;
    static const std::string format(const char* fmt, ...);
    
    static const unsigned char toUpperCaseChar(const unsigned char Char);
    static const unsigned char toLowerCaseChar(const unsigned char Char);
    
    static const std::string toUpperCase(const std::string& s);
    static const std::string toLowerCase(const std::string& s);

    static const std::string nltrim(const std::string& s);

    /**
     * Return a copy of the string in upper case.
     */
    static const std::string toupper(const std::string& s);
    
    /**
     * Calcula el checksum sobre un std::string
     */
    static char checkSum(std::string s);
    
    /**
     * valida que cumpla con el formato de una IPv4
     */
    static bool isValidIpV4(std::string ip);

	/*
	 * Jul 13, 2016 - jisler
	 */
    /**
     * Verifica que el std::string es numérico.
	 * Actualmente considera solo separador de decimales
     */
    static bool isNumeric(const std::string& s, const char& decimalSeparator);

    //verifica si el string contiene un numero entero positivo
    //retorna un booleano
    static bool isStringNumeric(std::string aValString);

	/**
	 * convierte una representacion de dos caracteres Hex a un char
	 * @param data representacion caracter
	 * @return char equivalente
	 */
	static char hexToChar(std::string data);
	static char valueDigit(char digit);


  };

} // namespace

#endif /* _UTILSTRING_H_ */
