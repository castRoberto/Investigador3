# -*- coding: utf-8 -*-
"""
Created on Wed Oct 19 10:39:23 2022

@author: rober
"""


gasolina_prom = {
    'ANTIOQUIA': 8246,
    'ATLANTICO': 8252,
    'BOGOTA D.C.': 8418,
    'CAQUETA': 7283,
    'CESAR': 8313,
    'CHOCO': 7196,
    'CUNDINAMARCA': 8543,
    'MAGDALENA': 7626,
    'QUINDIO': 8079,
    'SUCRE': 8258
    }

gasolina_desv = {
    'ANTIOQUIA': 495,
    'ATLANTICO': 421,
    'BOGOTA D.C.': 304,
    'CAQUETA': 465,
    'CESAR': 371,
    'CHOCO': 682,
    'CUNDINAMARCA': 419,
    'MAGDALENA': 679,
    'QUINDIO': 362,
    'SUCRE': 269
}

EDS1 = [
    'BOMBA UNICA LA PISTA',  # nombre estación
    'AMAZONAS',              # departamento
    14000                    # precio gasolina
]

EDS2 = [
    'CENTRO DE SERVICIOS SAN FRANCISCO',  # nombre estación
    'MAGDALENA',                            # departamento
    8322                                    # precio gasolina
]

# No modifiques el nombre de la función
def clasificar_EDS ( edslist, promdict, desvdict) -> str:
    # edsdict es la lista con la información de la estación de servicio
    # promdict es el diccionario con los precios promedio de la gasolina
    # desvdict es el diccionario con la desviación estándar del precio de la gasolina
    raise NotImplementedError(clasificar_EDS.__name__)
    import numpy as np
    dep = edslist[1]
    cl = 'NoData'
    
    if dep in (promdict.keys() and desvdict.keys()):
        clasificacion = ['----', '---', '--', '-', 'o', '+', '++', '+++', '++++']
        vals = [(promdict[dep] + i*desvdict[dep]) for i in np.arange(-2, 2.5, 0.5) if i != 0]
        cl = [clasificacion[vals.index(val)] for val in vals if edslist[2] <= val]
        
        cl = cl[0] if len(cl) != 0 else clasificacion[len(clasificacion)-1]
    
    print(cl)
    return cl

assert clasificar_EDS(EDS1, gasolina_desv, gasolina_prom) == 'NoData', "Ej. 1 - La salida de la función clasificar_EDS no está bien definida para el caso en el que el Departamento no está entre los datos."
assert clasificar_EDS(EDS2, gasolina_desv, gasolina_prom) == '++', "Ej. 1 - La salida de la función clasificar_EDS no está bien definido."
assert clasificar_EDS(['EDS1', 'MAGDALENA', 7600], gasolina_desv, gasolina_prom) == '+', "Ej. 1 - La salida de la función clasificar_EDS no está bien definido."
assert clasificar_EDS(['EDS2', 'MAGDALENA',  5000], gasolina_prom, gasolina_desv) == '----', "Ej. 1 - La salida de la función clasificar_EDS no está bien definido."
print("¡El Ejercicio 1 luce bien!")



