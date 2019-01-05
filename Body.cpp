#include "Body.h"

Body::Body()
{
}

 Body::~Body()
 {
 }

 /* TRANSFORMATION METHODS */
 void Body::translate(const glm::vec3 & vect) {
	 m_pos = m_pos + vect;
	 m_mesh.translate(vect);
	}

 void Body::rotate(float angle, const glm::vec3 & vect) {
	 m_mesh.rotate(angle, vect);
}

 void Body::scale(const glm::vec3 & vect) {
	 m_mesh.scale(vect);
 }

