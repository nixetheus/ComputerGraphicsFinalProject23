#pragma once
#include <glm/glm.hpp>


#define M_PI 3.141595f
void ProjectTSP::createProcedural(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx) {

    int definition = 25;

    float height = 0.18f; // In meters
    float internalRay = 0.12;
    float externalRay = 0.13;

    float x, y, z;

    // External cylinder
    for (int i = 0; i < definition; i++) {

        x = cos(i * 2 * M_PI / definition) * externalRay;
        z = sin(i * 2 * M_PI / definition) * externalRay;

        // Top Vertexes
        y = 1 * height;
        vDef.push_back({ {x, y, z}, {x, 0, z}, {-i / (float)definition, 0} }); // EXTERNAL NORMAL
        vDef.push_back({ {x, y, z}, {0.0f, 1.0f, 0.0f}, {0,0} }); // TOP NORMAL

        // Bottom Vertexes
        y = -1 * height;
        vDef.push_back({ {x, y, z}, {x, 0, z}, {-i / (float)definition, 0.65f} }); // EXTERNAL NORMAL
        vDef.push_back({ {x, y, z}, {0.0f, -1.0f, 0.0f}, {0,0} }); // BOTTOM NORMAL

        // Triangles
        vIdx.push_back(i * 4); vIdx.push_back((i * 4 + 4) % (4 * definition)); vIdx.push_back(i * 4 + 2); 
        vIdx.push_back(i * 4 + 2); vIdx.push_back((i * 4 + 4) % (4 * definition)); vIdx.push_back((i * 4 + 6) % (4 * definition));
    }

    // Internal Cylinder
    float bottomBorder = 0.01f;
    for (int i = 0; i < definition; i++) {

        x = cos(i * 2 * M_PI / definition) * internalRay;
        z = sin(i * 2 * M_PI / definition) * internalRay;

        // Top Vertexes
        y = 1 * height;
        vDef.push_back({ {x, y, z}, {x, 0, z}, {0,0} }); // EXTERNAL NORMAL
        vDef.push_back({ {x, y, z}, {0.0f, 1.0f, 0.0f}, {0,0} }); // TOP NORMAL

        // Bottom Vertexes
        y = -1 * height + bottomBorder;
        vDef.push_back({ {x, y, z}, {x, 0, z}, {0,0} }); // EXTERNAL NORMAL
        vDef.push_back({ {x, y, z}, {0.0f, -1.0f, 0.0f}, {0,0} }); // BOTTOM NORMAL

        // Triangles
        vIdx.push_back(i * 4 + definition * 4); vIdx.push_back(i * 4 + 2 + definition * 4); vIdx.push_back((i * 4 + 4) % (4 * definition) + definition * 4);
        vIdx.push_back(i * 4 + 2 + definition * 4); vIdx.push_back((i * 4 + 6) % (4 * definition) + definition * 4); vIdx.push_back((i * 4 + 4) % (4 * definition) + definition * 4);
    }

    // BOTTOM CIRCLES
    vDef.push_back({ {0, -1 * height, 0}, {0.0f, -1.0f, 0.0f}, {0,0} }); // BOTTOM VERTEX
    vDef.push_back({ {0, -1 * height + bottomBorder, 0}, {0.0f, 1.0f, 0.0f}, {0,0} }); // BOTTOM VERTEX + Border
    for (int i = 0; i < definition; i++) {
        vIdx.push_back(4 * definition * 2); vIdx.push_back(i * 4 + 3); vIdx.push_back((i * 4 + 7) % (4 * definition));  // BOTTOM
        vIdx.push_back(4 * definition * 2 + 1); vIdx.push_back((i * 4 + 7) % (4 * definition) + definition * 4); vIdx.push_back(i * 4 + 3 + definition * 4);  // BOTTOM + border
    }

    // ANNULUS
    for (int i = 0; i < definition; i++) {
        vIdx.push_back(i * 4 + 1 + definition * 4); vIdx.push_back((i * 4 + 5) % (4 * definition)); vIdx.push_back(i * 4 + 1);
        vIdx.push_back(i * 4 + 1 + definition * 4); vIdx.push_back((i * 4 + 5) % (4 * definition) + definition * 4); vIdx.push_back((i * 4 + 5) % (4 * definition));
    }

    // TORUS
    int torusDef = 15;
    float tubeRadius = 0.02;
    float torusRadius = height * 0.6;
    int startIndex = vDef.size();
    for (int u = 0; u < torusDef + 1; u++) {
        for (int v = 0; v < torusDef; v++) {

            // Coordinates
            x = (torusRadius + tubeRadius * cos(v * M_PI / torusDef)) * cos(u * M_PI / torusDef - M_PI / 2);
            y = (torusRadius + tubeRadius * cos(v * M_PI / torusDef)) * sin(u * M_PI / torusDef - M_PI / 2);
            z = tubeRadius * sin(v * M_PI / torusDef);

            // Normals
            /* tangent vector with respect to big circle */
            float tx = -sin(u * M_PI / torusDef - M_PI / 2);
            float ty = cos(u * M_PI / torusDef - M_PI / 2);
            float tz = 0;
            /* tangent vector with respect to little circle */
            float sx = cos(u * M_PI / torusDef - M_PI / 2) * (-sin(v * M_PI / torusDef));
            float sy = sin(u * M_PI / torusDef - M_PI / 2) * (-sin(v * M_PI / torusDef));
            float sz = cos(v * M_PI / torusDef);
            /* normal is cross-product of tangents */
            float nx = ty * sz - tz * sy;
            float ny = tz * sx - tx * sz;
            float nz = tx * sy - ty * sx;
            /* normalize normal */
            float length = sqrt(nx * nx + ny * ny + nz * nz);
            nx /= length;
            ny /= length;
            nz /= length;

            vDef.push_back({ {x + internalRay, y, z}, {nx, ny, nz}, {0,0} });

            if (u != torusDef) {
                vIdx.push_back(startIndex + u * torusDef + v); vIdx.push_back(startIndex + ((u + 1) % (torusDef + 1)) * torusDef + v); vIdx.push_back(startIndex + ((u + 1) % (torusDef + 1)) * torusDef + (v + 1) % torusDef);
                vIdx.push_back(startIndex + u * torusDef + v); vIdx.push_back(startIndex + ((u + 1) % (torusDef + 1)) * torusDef + (v + 1) % (torusDef)); vIdx.push_back(startIndex + u * torusDef + (v + 1) % (torusDef));
            }
        }
    }
}