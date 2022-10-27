#ifndef _MAPTABLE_H_
#define _MAPTABLE_H_

#include <map>

/**
 * Esta clase define un factory que asocia llaves con objetos los cuales pueden
 * ser posteriormente rescatados.
 * <p>Hace poco m'as que la definici'on del mapa mismo, excepto que lo enmascara
 * en el contexto de un factory y de que asume inmediatamente que el objeto
 * almacenado es un puntero.</p>
 * <p>Oportunidad de Uso:<br />
 * Esta facilidad puede utilizarse en m'ultiples casos en reemplazo de
 * secuencias de if-then-else o switch's.</p>
 * <p>Notar que normalmente el objeto almacenado ser'a directamente aquel donde
 * se desea ejecutar la acci'on que corresponda (tal vez un <i>process</i>) o
 * algo as'i. El ciclo principal - que busca al objeto - conocer'a qu'e m'etodo
 * invocar en el objeto y con qu'e par'ametros.</p>
 * <p>En los casos en los cuales el objeto asociado a la llave hay que
 * <i>construirlo</i> (como en el caso de reconocimiento de data serializada),
 * entonces el objeto almacenado en la tabla normalmente es un <b>constructor</b>
 * del objeto (al cual se le invoca un <i>create</i> o algo as'i).</p>
 *
 * @date Oct 18, 2008
 * @author ayachan
 */
template <typename K, typename O>
class MapTable
{
public:
  /**
   * Constructor, instancia la tabla.
   */
  MapTable() : table() {}
  /**
   * Destructor, dispone los objetos puntero.
   */
  ~MapTable()
  {
    for (
      typename std::map<K, O*>::iterator p = table.begin();
      p != table.end(); ++p)
    {
      delete p->second;
    }
    table.clear();
  }

  /**
   * Agrega una asociaci'on a la tabla.
   */
  void add(K key, O* object) { table.insert(std::make_pair(key, object)); }

  /**
   * Obtiene el objeto asociado a una llave.
   * @param key La llave a buscar en la tabla.
   * @return El objeto asociado o 0 (null) si no existe.
   */
  O* get(K key)
  {
    O* object = 0;
    typename std::map<K, O*>::iterator it = table.find(key);
    if (it != table.end())
      object = it->second;
    return object;
  }

  int size() { return table.size(); }

  typename std::map<K, O*>::iterator begin() { return table.begin(); }
  typename std::map<K, O*>::iterator end() { return table.end(); }

private:
  std::map<K, O*> table;
};

#endif /*_MAPTABLE_H_*/
