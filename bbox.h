#if !defined _BBOX_H
#define _BBOX_H

#include "glm/vec3.hpp"
#include "object.h"
#include "objectlist.h"

class BBox : public Object {
public:
	BBox(ObjectList *listaObjetos);
	BBox(Object *child);

	virtual float NearestInt(const glm::vec3& pos, const glm::vec3& dir);
	virtual glm::vec3 Shade(ShadingInfo &shadInfo);
	virtual void Draw();

	virtual Object* NearestInt(const glm::vec3& pos, const glm::vec3& dir, float& tnear, float tmax);
private:
	ObjectList* children;
	bool isLeaf;

	void Add(Object *child);
};

float distance(Object *a, Object *b);

#endif