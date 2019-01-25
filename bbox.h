#if !defined _BBOX_H
#define _BBOX_H

#include "glm/vec3.hpp"
#include "object.h"
#include "objectlist.h"

class BBox : public Object {
public:
	BBox(ObjectList *listaObjetos);
	BBox(Object *child);

	/* Inherited functions that are implemented in this class */
	// Calcular intersección con bounding box
	virtual float NearestInt(const glm::vec3& pos, const glm::vec3& dir);
	virtual glm::vec3 Shade(ShadingInfo &shadInfo);
	virtual void Draw();

	// Gets the nearest object
	// Recorrer el árbol
	virtual Object* NearestInt(const glm::vec3& pos, const glm::vec3& dir, float& tnear, float tmax);
private:
	ObjectList* children;
	bool isLeaf;

	void Add(Object *child);
	//Object** OLtoArray(ObjectList* listaObjetos);
	//int partition(Object **arr, int low, int high);
	//void quickSort(Object **arr, int low, int high);
	//void Ordenar(Object** arrayObjetos, int length, glm::vec3 *min, glm::vec3 *max);
};

float distance(Object *a, Object *b);

#endif