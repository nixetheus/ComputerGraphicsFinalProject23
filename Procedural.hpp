#pragma once
#include <glm/glm.hpp>


#define M_PI 3.141595f
void ProjectTSP::createProcedural(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx) {

	int numVerticalCuts = 1000;
	int numHorizontalCuts = 1000;
	double thetaStep = M_PI / numVerticalCuts;
	double phiStep = 2 * M_PI / numHorizontalCuts;

	for (int i = 0; i < numVerticalCuts; i++) {

		double theta = i * thetaStep;
		for (int j = 0; j < numHorizontalCuts; j++) {

			double phi = j * phiStep;
			double x = sin(theta) * cos(phi);
			double z = sin(theta) * sin(phi);
			double y = cos(theta);

			vDef.push_back({ {x, y, z}, {x, y, z}, {(atan2(z, x) + M_PI) / (2 * M_PI), acos(y) / M_PI} });

			int circleEnd = 0;
			if (j % numHorizontalCuts == 0) circleEnd = numHorizontalCuts;

			// First triangle
			vIdx.push_back(i * numHorizontalCuts + j);
			vIdx.push_back((i + 1) * numHorizontalCuts + j);
			vIdx.push_back((i + 1) * numHorizontalCuts + j - 1 + circleEnd);

			// Second triangle
			vIdx.push_back(i * numHorizontalCuts + j);
			vIdx.push_back(i * numHorizontalCuts + j - 1 + circleEnd);
			vIdx.push_back((i + 1) * numHorizontalCuts + j - 1 + circleEnd);
		}
	}
}