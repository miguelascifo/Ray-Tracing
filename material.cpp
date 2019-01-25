/* Material.cpp
*
* Interaccion y Visualizacion de la Informacion.
*
* Practice 2.
* Ray tracing.
*
* Jose Pascual Molina Masso.
* Escuela Superior de Ingenieria Informatica de Albacete.
*/


#include "glm/glm.hpp" // glm::vec3, glm::dot

#include "Material.h"
#include "light.h"
#include "lightlist.h"
#include "world.h"
#include "limits.h"


/* Constructors */
Material::Material(const glm::vec3 &diff) {

	Ka = glm::vec3(0.0, 0.0, 0.0);
	Kd = diff;
	Kdt = glm::vec3(0.0, 0.0, 0.0);
	Ks = glm::vec3(0.0, 0.0, 0.0);
	Kst = glm::vec3(0.0, 0.0, 0.0);
	n = 0;
	Ie = glm::vec3(0.0, 0.0, 0.0);
	Kr = glm::vec3(0.0, 0.0, 0.0);
	Kt = glm::vec3(0.0, 0.0, 0.0);
	ior = 0.0;
}

Material::Material(const glm::vec3 &amb, const glm::vec3 &diff, const glm::vec3 &diffTrans,
	const glm::vec3 &spec, const glm::vec3 &specTrans, int shine, const glm::vec3 &emis,
	const glm::vec3 &refl, const glm::vec3 &trans, float index) {

	Ka = amb;
	Kd = diff;
	Kdt = diffTrans;
	Ks = spec;
	Kst = specTrans;
	n = shine;
	Ie = emis;
	Kr = refl;
	Kt = trans;
	ior = index;
}

/* Implements the global illumination model */
glm::vec3 Material::Shade(ShadingInfo &shadInfo)
{
	glm::vec3 color(0.0, 0.0, 0.0), V;
	float VdotN, ratio;
	bool isTrans;

	V = -shadInfo.rayDir;
	VdotN = glm::dot(V, shadInfo.normal);
	isTrans = (Kt != glm::vec3(0.0, 0.0, 0.0));
	if (VdotN < 0) {

		// The viewer stares at an interior or back face of the object,
		// we will only illuminate it if material is transparent
		if (isTrans) {
			shadInfo.normal = -shadInfo.normal;  // Reverse normal
			VdotN = -VdotN;
			ratio = 1.0 / ior; // Ray always comes from vacuum (hollow objects)
			//ratio = ior;  // Use this instead for solid objects
		}
		else
			return color;
	}
	else {

		// The viewer stares at a front face of the object
		if (isTrans)
			ratio = 1.0 / ior; // Ray comes from vacuum
	}


	Light *pLight;

	/* Ambient */
	if (Ka != glm::vec3(0.0, 0.0, 0.0)) {

		if (shadInfo.pWorld->lights.Length() > 0) {

			glm::vec3 sumIa(0.0, 0.0, 0.0);

			/* Access the list of lights in sequence */
			pLight = shadInfo.pWorld->lights.First();
			while (pLight != NULL) {

				sumIa += pLight->Ia;
				pLight = shadInfo.pWorld->lights.Next();
			}
			color += Ka * sumIa;
		}
	}

	/* Diffuse and specular */
	bool computeDiff = (Kd != glm::vec3(0.0, 0.0, 0.0));
	bool computeSpec = (Ks != glm::vec3(0.0, 0.0, 0.0));
	if (computeDiff || computeSpec) {

		if (shadInfo.pWorld->lights.Length() > 0) {

			glm::vec3 sumId(0.0, 0.0, 0.0), sumIs(0.0, 0.0, 0.0),
				sumIdt(0.0, 0.0, 0.0), sumIst(0.0, 0.0, 0.0);

			/* Access the list of lights in sequence */
			pLight = shadInfo.pWorld->lights.First();
			while (pLight != NULL) {

				glm::vec3 L, R, T;
				float LdotN, t;
				Object *optr;
				glm::vec3 atten(1.0, 1.0, 1.0);

				L = glm::normalize(pLight->position - shadInfo.point);

				LdotN = glm::dot(L, shadInfo.normal);
				if (LdotN > 0.0) {  // Light is on the viewer's side

					/* Check if the point is in shadow */
					optr = shadInfo.pWorld->objectList->First();
					while (optr != NULL) {

						/* Cast shadow ray */
						t = optr->NearestInt(shadInfo.point, L);
						if (TMIN < t) {  // Beware of self-occlusion

							if (optr->pMaterial->Kt == glm::vec3(0.0, 0.0, 0.0)) {
								atten = glm::vec3(0.0, 0.0, 0.0);
								break;
							}
							atten *= optr->pMaterial->Kt;
						}
						optr = shadInfo.pWorld->objectList->Next();
					}
					shadInfo.pWorld->numShadRays++;

					if (atten != glm::vec3(0.0, 0.0, 0.0)) {  // If not in shadow...

						if (computeDiff)
							sumId += atten * pLight->Id * LdotN;

						if (computeSpec) {
							R = (2 * LdotN * shadInfo.normal) - L;
							sumIs += atten * pLight->Is * glm::pow(glm::dot(R, V), n);
						}
					}
				}
				else if (isTrans) {  // Light is on the other side, only visible if material is transparent

					/* Check if the point is in shadow */
					optr = shadInfo.pWorld->objectList->First();
					while (optr != NULL) {

						/* Cast shadow ray */
						t = optr->NearestInt(shadInfo.point, L);
						if (TMIN < t) {  // Beware of self-occlusion

							if (optr->pMaterial->Kt == glm::vec3(0.0, 0.0, 0.0)) {
								atten = glm::vec3(0.0, 0.0, 0.0);
								break;
							}
							atten *= optr->pMaterial->Kt;
						}
						optr = shadInfo.pWorld->objectList->Next();
					}
					shadInfo.pWorld->numShadRays++;

					if (atten != glm::vec3(0.0, 0.0, 0.0)) {  // If not in shadow...

						if (computeDiff)
							sumIdt += atten * pLight->Id * (-LdotN);

						if (computeSpec) {

							glm::vec3 T;
							float radical;

							radical = 1.0 + (ratio * ratio) * ((LdotN * LdotN) - 1.0);
							if (radical > 0.0) {

								T = glm::normalize(ratio*(-L) + (ratio*(-LdotN) - sqrt(radical))*(-shadInfo.normal));
								sumIst += atten * pLight->Is * glm::pow(glm::dot(T, V), n);
							}
						}
					}
				}

				pLight = shadInfo.pWorld->lights.Next();
			}
			color += (Kd * sumId) + (Ks * sumIs) + (Kdt * sumIdt) + (Kst * sumIst);
		}
	}

	/* Emission term */
	color += Ie;

	/* Check recursion limit */
	if (shadInfo.depth >= shadInfo.pWorld->maxDepth)
		return color;

	/* Global reflection */
	if (Kr != glm::vec3(0.0, 0.0, 0.0)) {

		glm::vec3 Rv;

		Rv = (2 * VdotN * shadInfo.normal) - V;
		color += Kr * shadInfo.pWorld->Trace(shadInfo.point, Rv, shadInfo.depth + 1);

		shadInfo.pWorld->numReflRays++;
	}

	/* Global refraction */
	if (isTrans) {

		glm::vec3 Tv;
		float radical;

		radical = 1.0 + (ratio * ratio) * ((VdotN * VdotN) - 1.0);
		if (radical > 0.0) {

			Tv = glm::normalize(ratio*shadInfo.rayDir + (ratio*VdotN - sqrt(radical))*shadInfo.normal);
			color += Kt * shadInfo.pWorld->Trace(shadInfo.point, Tv, shadInfo.depth + 1);

			shadInfo.pWorld->numRefrRays++;
		}
	}

	return color;
}

