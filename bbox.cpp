#include <algorithm>
#include "bbox.h"
#include "limits.h"
#include "object.h"
#include "objectlist.h"
#include "glm/vec3.hpp"
#include "glm/glm.hpp"

BBox::BBox(ObjectList *listaOrigen) {
	BBox *optr, *closeoptr, *boxptr;
	float closedist, newdist;
	ObjectList *listaArbol = new ObjectList();
	ObjectList *auxList = new ObjectList();
	ObjectList *listaObjetos = listaOrigen->Clone();
	int offset = 0;

	while (listaObjetos->Length() > 1) {
		optr = (BBox*)listaObjetos->Pop();
		listaArbol->Add(optr);
		optr = (BBox*)listaObjetos->Pop();

		while (optr != NULL)
		{
			closeoptr = NULL;
			boxptr = (BBox*)listaArbol->First();
			closedist = TFAR;

			while (boxptr != NULL) {
				newdist = distance(optr, boxptr);
				if (newdist <= closedist) {
					closedist = newdist;
					closeoptr = boxptr;
					
					if (closedist == 0)
						break;
				}

				boxptr = (BBox*)listaArbol->Next();
			}

			if (closeoptr == NULL || closedist > CLOSEDIST + offset) {
				listaArbol->Add(optr);
			}
			else {
				closeoptr->Add(optr);
			}

			optr = (BBox*) listaObjetos->Pop();
		}

		auxList = listaObjetos;
		listaObjetos = listaArbol;
		listaArbol = auxList;
		offset++;
	}

	optr = (BBox*) listaObjetos->First();
	children = optr->children;
	center = optr->center;
	box = optr->box;
	isLeaf = optr->isLeaf;
}

inline BBox::BBox(Object *child) {
	center = child->center;
	box = child->box;
	children = new ObjectList();
	children->Add(child);
	isLeaf = true;
}

void BBox::Add(Object *child) {
	if (isLeaf) {
		Object *root = children->First();

		children = new ObjectList();
		children->Add(new BBox(root));
		isLeaf = false;
	}

	children->Add(child);
	glm::vec3 max = center + box;
	glm::vec3 min = center - box;
	glm::vec3 newmax = child->center + child->box;
	glm::vec3 newmin = child->center - child->box;

	max = glm::vec3(std::max(max.x, newmax.x), std::max(max.y, newmax.y), std::max(max.z, newmax.z));
	min = glm::vec3(std::min(min.x, newmin.x), std::min(min.y, newmin.y), std::min(min.z, newmin.z));

	box = (max - min) / 2.0f;
	center = max - box;
}

float distance(Object *a, Object *b) {
	float dist = glm::distance(a->center, b->center);
	dist = dist - std::sqrt(a->box.x * a->box.x + a->box.y * a->box.y + a->box.z * a->box.z);
	dist = dist - std::sqrt(b->box.x * b->box.x + b->box.y * b->box.y + b->box.z * b->box.z);

	//return dist > 0 ? dist : 0;

	if (dist < 0)
		dist = 0;
	return dist;
}


float BBox::NearestInt(const glm::vec3& pos, const glm::vec3& dir) {
	float result = 0;

	if (dir.x != 0.0)
	{
		float tx1 = (this->center.x - this->box.x - pos.x) / dir.x;
		float tx2 = (this->center.x + this->box.x - pos.x) / dir.x;
		result = TMIN < tx1 || TMIN < tx2;
	}

	if (!result && dir.y != 0.0)
	{
		float ty1 = (this->center.y - this->box.y - pos.y) / dir.y;
		float ty2 = (this->center.y + this->box.y - pos.y) / dir.y;
		result = TMIN < ty1 || TMIN < ty2;
	}

	if (!result && dir.z != 0.0)
	{
		float tz1 = (this->center.z - this->box.z - pos.z) / dir.z;
		float tz2 = (this->center.z + this->box.z - pos.z) / dir.z;
		result = TMIN < tz1 || TMIN < tz2;
	}

	return result;
}

Object* BBox::NearestInt(const glm::vec3& pos, const glm::vec3& dir, float& tnear, float tmax) {
	Object *result = NULL, *optr, *auxo;
	float tmin = NearestInt(pos, dir);
	float aux = 0.0f;

	tnear = tmax;
	if (tmin > 0.0f && tmin < tmax) {
		optr = children->First();
		if (!isLeaf) {
			while (optr != NULL) {
				auxo = ((BBox*) optr)->NearestInt(pos, dir, aux, tnear);
				if (auxo != NULL && aux > TMIN) {
					result = auxo;
					tnear = aux;
				}

				optr = children->Next();
			}
		}
		else {
			tnear = optr->NearestInt(pos, dir);
			if (tnear > 0.0f && tnear < tmax) {
				result = optr;
			}
		}
	}

	if (tnear >= tmax) {
		tnear = 0;
	}

	return result;
}

/* Returns the color of the sphere at the intersection point with the ray */
glm::vec3 BBox::Shade(ShadingInfo &shadInfo) {
	glm::vec3 color;
	return color;
}

/* Draw the object with OpenGL and GLUT */
void BBox::Draw() {}