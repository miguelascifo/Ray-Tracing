//
// OBJETO3D.H
//
//   Fichero cabecera de la jerarquía de clases Objeto3D, base de la
//   que heredan todos los objetos geométricos del trazador. Además
//   de Objeto3D, aquí se definen las clases ListaObjetos o Primitiva.
//
//   ------------------------------------------------
//   |                                              |
//   |         José Pascual Molina Massó            |
//   |     Proyecto Fin de Carrera - 1999/2000      |
//   |   Escuela Politécnica Superior de Albacete   |
//   |      Universidad de Castilla-La Mancha       |
//   |                                              |
//   ------------------------------------------------
//

#if !defined _OBJETO3D_H_
#define _OBJETO3D_H_

// Declaraciones adelantadas

class ObjetoVisible;
class CajaAlinEjes;
class Modelo;

////////////////////
// Clase Objeto3D //
////////////////////

// Clase base virtual que representa la interfaz básica
// con la que el trazador de rayos se comunica con los objetos.
class Objeto3D {

public:

  // Funciones miembro virtuales

  // Devuelve el objeto visible más cercano en la dirección y sentido del
  // rayo, junto al valor del parámetro t calculado, que debe ser positivo
  // y menor que tmax. Si no interseca ningún objeto devuelve un puntero nulo.
  virtual ObjetoVisible* ObjetoMasCercano(Rayo &r, float &t, float tmax) = 0;

  // Si un objeto visible intercepta el rayo de sombra que se pasa como
  // parámetro, calcula la atenuación de la luz en función del grado de
  // transparencia de su superficie. Si el objeto es transparente, se devuelve
  // un puntero nulo, pero si el rayo interseca un objeto opaco debe devolverse
  // entonces un puntero al mismo, para poder introducirlo en la "shadow cache".
  virtual ObjetoVisible* CalculaAtenuacion(Rayo &r, ColorRGB &aten, float tmax) = 0;

  // Devuelve true si el objeto y la caja alineada con lo ejes que se pasa
  // como parámetro se intersecan. En caso contrario devuelve false. Por defecto
  // determina si existe intersección entre esa caja y la caja que envuelve
  // al objeto.
  virtual bool IntersecaCaja(CajaAlinEjes &c);

  // Transforma el objeto de acuerdo a una matriz de transformacion
  virtual void Transformar(const Matriz &m) = 0;

  // Devuelve la caja de menor tamaño que contiene al objeto. Por defecto
  // esta función devuelve una caja infinita, por lo que los objetos finitos
  // deberán redefinirla.
  virtual CajaAlinEjes CajaEnvolvente();

  // Dos alternativas a las tres siguientes funciones consiste, por un lado
  // en definir una función Flags() como hace Wilt o bien usar dynamic_cast.
  // En el primer caso hay que incluir también varios #define que nos
  // permitan conocer los flags. En el segundo caso deben resolverse los
  // castings en tiempo de ejecución. Lo ideal sería un tipo conjunto.

  // Nos dice si el objeto es un volumen envolvente. Por defecto devuelve false.
  virtual bool EsVolumenEnvolvente() { return false; }

  // Nos dice si el objeto es un volumen que implementa el recorrido
  // de jerarquía de volúmenes de Kay y Kajiya. Por defecto devuelve false.
  virtual bool EsVolumenJerarquia() { return false; }

  // Devuelve true si el objeto es una instancia de la clase CajaJerarquia.
  // Por defecto se devuelve false.
  virtual bool EsCajaJerarquia() { return false; }

  // Devuelve el número de hijos de este objeto. Por defecto devuelve 0,
  // aunque los objetos contenedores devolverán el número de objetos
  // que incluyen en su lista.
  virtual int NumeroHijos() { return 0; }

  // Determina si un objeto es o no infinito. Para ello obtiene la caja
  // de menor tamaño que lo contiene, y comprueba que sus límites no sean
  // infinitos.
  virtual bool EsInfinito();

  // Para recuento estadistico. Por defecto no hace nada.
  virtual void Contar() { }

  // Traduce el objeto a una representación más apropiada para su modelado
  virtual Modelo* CopiarParaModelar() { }

private:

  // Datos privados

  CacheRayo cacheRayo;  // En la técnica de subdivisión uniforme del espacio
                        // se utiliza para guardar el último rayo con el que
                        // un objeto calculó intersección, para evitar la
                        // repetición de ese cálculo.

  friend class MatrizVoxels;
};


////////////////////////
// Clase ListaObjetos //
////////////////////////

// Lista de punteros a instancias de la clase Objeto3D
class ListaObjetos : public Objeto3D, public Lista<Objeto3D*> {

public:

  // Funciones que hereda de Objeto3D y que esta clase redefine

  virtual ObjetoVisible* ObjetoMasCercano(Rayo &r, float &t, float tmax);
  virtual ObjetoVisible* CalculaAtenuacion(Rayo &r, ColorRGB &aten, float tmax);
  virtual bool IntersecaCaja(CajaAlinEjes &c);
  virtual void Transformar(const Matriz &m);
  virtual CajaAlinEjes CajaEnvolvente();
  virtual int NumeroHijos() { return Longitud(); }
  virtual void Contar();
};


/////////////////////
// Clase Primitiva //
/////////////////////

class Primitiva : public Objeto3D {

public:

  // Funciones que esta clase añade al interfaz de la clase Objeto3D

  // Calcula el punto de intersección más cercano entre el rayo y el objeto,
  // en la dirección y sentido del rayo. Devuelve el parámetro t correspondiente
  // a ese punto si es positivo y menor que tmax, en caso contrario devuelve 0.
  virtual float IntersecMasCercana(Rayo &r, float tmax) = 0;

  // Obtiene una lista con los parámetros t de todos los puntos del
  // objeto que atraviesa el rayo, ordenados de menor a mayor.
  virtual ListaOrdInts* ListaIntersecciones(Rayo &r) = 0;

  // Determina si el rayo interseca el objeto. Si es así devuelve true, en
  // caso contrario devuelve false. Por defecto se limita a llamar a la
  // función IntersecMasCercana.
  virtual bool IntersecaRayo(Rayo &r, float tmax);
};

// Determina si el rayo interseca el objeto. Si es así devuelve true,
// en caso contrario devuelve false.
inline bool Primitiva::IntersecaRayo(Rayo &r, float tmax)
{
  if (IntersecMasCercana(r,tmax) > 0) return true;
  return false;
}

#endif  // !defined _OBJETO3D_H_
