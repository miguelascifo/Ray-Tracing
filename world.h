/* world.h
 *
 * Interaccion y Visualizacion de la Informacion.
 *
 * Practice 2.
 * Ray tracing.
 *
 * Jose Pascual Molina Masso.
 * Escuela Superior de Ingenieria Informatica de Albacete.
 */

#ifndef _WORLD_H_
#define _WORLD_H_

#include "glm/vec3.hpp" // glm::vec3

#include "objectlist.h"
#include "lightlist.h"
#include "bbox.h"


/***************/
/* World class */
/***************/

class World {

public:

	/* Constructor */
	World();

	/* Reset statistics */
	void ClearStats();
	
	/* Key function in recursive ray tracing */
	glm::vec3 Trace(const glm::vec3& pos, const glm::vec3& dir, int depth);
	
	int maxDepth;  // Limit of recursion
	glm::vec3 bgcolor;  // Background color

	/* Lists of 3D objects and lights */
	ObjectList* objectList;
	BBox* bbox;
	LightList lights;

	/* Viewpoint parameters */
	glm::vec3 eye,  // Observer 
		center,     // Looking at this point
		up;         // View-up vector

	/* Statistics */
	int numPrimRays, numShadRays, numReflRays, numRefrRays, numTests;
};


#endif  // !defined _WORLD_H_
