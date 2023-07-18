#pragma once
#include <glm/glm.hpp>


#define M_PI 3.141595f
void ProjectTSP::createProcedural(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx) {

	// The primitive built here is a box centered in the origin, with proportions respecting the texture.

	// The procedure fills array vPos with the positions of the vertices and of the normal vectors of the mesh
	vDef.push_back({{0,0,0}, {0,0,1}, {0,0}});	// vertex 0 - Position, Normal and uv
	vDef.push_back({{1,0,0}, {0,0,1}, {1,0}});	// vertex 1 - Position and Normal
	vDef.push_back({{0,1,0}, {0,0,1}, {0,1}});	// vertex 2 - Position and Normal
	vDef.push_back({{1,1,0}, {0,0,1}, {1,1}});// vertex 3 - Position and Normal

	// The procedures also fill the array vIdx with the indices of the vertices of the triangles
	vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);	// First triangle
	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);	// Second triangle
}