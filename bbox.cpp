
#include <algorithm>
#include "bbox.h"
#include "limits.h"
#include "object.h"
#include "objectlist.h"
#include "glm/vec3.hpp"
#include "glm/glm.hpp"

BBox::BBox(ObjectList *listaObjetos) {
	BBox *optr, *closeoptr, *boxptr;
	float closedist, newdist;
	ObjectList *listaArbol = new ObjectList();
	ObjectList *auxList = new ObjectList();
	int offset = 0;

	while (listaObjetos->Length() > 1) {
		optr = (BBox*)listaObjetos->Pop();
		//listaArbol->Add(optr);

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
		children->Add(new BBox(child));
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
	dist = dist - glm::distance(a->center, a->box);
	dist = dist - glm::distance(b->center, b->box);

	//return dist > 0 ? dist : 0;

	if (dist < 0)
		dist = 0;
	return dist;
}


float BBox::NearestInt(const glm::vec3& pos, const glm::vec3& dir) {
	// La implementación de este método es igual que para la esfera. Debería cambiarse por un cubo.
	glm::vec3 dif;
	float A, B, C, aux;
	float t1, t2;
	float radius = glm::distance(center, box);

	A = glm::dot(dir, dir);
	dif = pos - center;
	B = 2.0f * glm::dot(dir, dif);
	C = glm::dot(dif, dif) - radius * radius;

	aux = (B * B) - (4 * A * C);

	if (aux < 0.0) return 0;

	if (aux == 0.0) {
		return -B / 2.0f;
	}

	aux = (float)glm::sqrt(aux);
	t1 = (-B + aux) / (2.0f * A);
	t2 = (-B - aux) / (2.0f * A);

	if (TMIN < t1)
		if (TMIN < t2)
			return (t1 < t2) ? t1 : t2;
		else
			return t1;

	if (TMIN < t2)
		return t2;

	return 0;
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
				if (auxo != NULL && aux < tnear) {
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