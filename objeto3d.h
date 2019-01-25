//
// OBJETO3D.H
//
//   Fichero cabecera de la jerarqu�a de clases Objeto3D, base de la
//   que heredan todos los objetos geom�tricos del trazador. Adem�s
//   de Objeto3D, aqu� se definen las clases ListaObjetos o Primitiva.
//
//   ------------------------------------------------
//   |                                              |
//   |         Jos� Pascual Molina Mass�            |
//   |     Proyecto Fin de Carrera - 1999/2000      |
//   |   Escuela Polit�cnica Superior de Albacete   |
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

// Clase base virtual que representa la interfaz b�sica
// con la que el trazador de rayos se comunica con los objetos.
class Objeto3D {

public:

  // Funciones miembro virtuales

  // Devuelve el objeto visible m�s cercano en la direcci�n y sentido del
  // rayo, junto al valor del par�metro t calculado, que debe ser positivo
  // y menor que tmax. Si no interseca ning�n objeto devuelve un puntero nulo.
  virtual ObjetoVisible* ObjetoMasCercano(Rayo &r, float &t, float tmax) = 0;

  // Si un objeto visible intercepta el rayo de sombra que se pasa como
  // par�metro, calcula la atenuaci�n de la luz en funci�n del grado de
  // transparencia de su superficie. Si el objeto es transparente, se devuelve
  // un puntero nulo, pero si el rayo interseca un objeto opaco debe devolverse
  // entonces un puntero al mismo, para poder introducirlo en la "shadow cache".
  virtual ObjetoVisible* CalculaAtenuacion(Rayo &r, ColorRGB &aten, float tmax) = 0;

  // Devuelve true si el objeto y la caja alineada con lo ejes que se pasa
  // como par�metro se intersecan. En caso contrario devuelve false. Por defecto
  // determina si existe intersecci�n entre esa caja y la caja que envuelve
  // al objeto.
  virtual bool IntersecaCaja(CajaAlinEjes &c);

  // Transforma el objeto de acuerdo a una matriz de transformacion
  virtual void Transformar(const Matriz &m) = 0;

  // Devuelve la caja de menor tama�o que contiene al objeto. Por defecto
  // esta funci�n devuelve una caja infinita, por lo que los objetos finitos
  // deber�n redefinirla.
  virtual CajaAlinEjes CajaEnvolvente();

  // Dos alternativas a las tres siguientes funciones consiste, por un lado
  // en definir una funci�n Flags() como hace Wilt o bien usar dynamic_cast.
  // En el primer caso hay que incluir tambi�n varios #define que nos
  // permitan conocer los flags. En el segundo caso deben resolverse los
  // castings en tiempo de ejecuci�n. Lo ideal ser�a un tipo conjunto.

  // Nos dice si el objeto es un volumen envolvente. Por defecto devuelve false.
  virtual bool EsVolumenEnvolvente() { return false; }

  // Nos dice si el objeto es un volumen que implementa el recorrido
  // de jerarqu�a de vol�menes de Kay y Kajiya. Por defecto devuelve false.
  virtual bool EsVolumenJerarquia() { return false; }

  // Devuelve true si el objeto es una instancia de la clase CajaJerarquia.
  // Por defecto se devuelve false.
  virtual bool EsCajaJerarquia() { return false; }

  // Devuelve el n�mero de hijos de este objeto. Por defecto devuelve 0,
  // aunque los objetos contenedores devolver�n el n�mero de objetos
  // que incluyen en su lista.
  virtual int NumeroHijos() { return 0; }

  // Determina si un objeto es o no infinito. Para ello obtiene la caja
  // de menor tama�o que lo contiene, y comprueba que sus l�mites no sean
  // infinitos.
  virtual bool EsInfinito();

  // Para recuento estadistico. Por defecto no hace nada.
  virtual void Contar() { }

  // Traduce el objeto a una representaci�n m�s apropiada para su modelado
  virtual Modelo* CopiarParaModelar() { }

private:

  // Datos privados

  CacheRayo cacheRayo;  // En la t�cnica de subdivisi�n uniforme del espacio
                        // se utiliza para guardar el �ltimo rayo con el que
                        // un objeto calcul� intersecci�n, para evitar la
                        // repetici�n de ese c�lculo.

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

  // Funciones que esta clase a�ade al interfaz de la clase Objeto3D

  // Calcula el punto de intersecci�n m�s cercano entre el rayo y el objeto,
  // en la direcci�n y sentido del rayo. Devuelve el par�metro t correspondiente
  // a ese punto si es positivo y menor que tmax, en caso contrario devuelve 0.
  virtual float IntersecMasCercana(Rayo &r, float tmax) = 0;

  // Obtiene una lista con los par�metros t de todos los puntos del
  // objeto que atraviesa el rayo, ordenados de menor a mayor.
  virtual ListaOrdInts* ListaIntersecciones(Rayo &r) = 0;

  // Determina si el rayo interseca el objeto. Si es as� devuelve true, en
  // caso contrario devuelve false. Por defecto se limita a llamar a la
  // funci�n IntersecMasCercana.
  virtual bool IntersecaRayo(Rayo &r, float tmax);
};

// Determina si el rayo interseca el objeto. Si es as� devuelve true,
// en caso contrario devuelve false.
inline bool Primitiva::IntersecaRayo(Rayo &r, float tmax)
{
  if (IntersecMasCercana(r,tmax) > 0) return true;
  return false;
}

#endif  // !defined _OBJETO3D_H_
