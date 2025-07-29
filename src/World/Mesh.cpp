#include "Mesh.h"


Mesh::Mesh() : VAO(0), VertexVBO(0), UVVBO(0), EBO(0), AOBO(0), NormalVBO(0) {

	Verts = std::make_unique<std::vector<glm::vec3>>();
	UVs = std::make_unique<std::vector<glm::vec2>>();
	Indices = std::make_unique<std::vector<unsigned int>>();
	AOVals = std::make_unique<std::vector<uint8_t>>();
	Normals = std::make_unique<std::vector<glm::vec3>>();
	indexCount = 0;
}

void Mesh::addIndices(int amtFaces)
{
	for (int i = 0; i < amtFaces; i++) {
		Indices->push_back(0 + indexCount);
		Indices->push_back(1 + indexCount);
		Indices->push_back(2 + indexCount);
		Indices->push_back(2 + indexCount);
		Indices->push_back(3 + indexCount);
		Indices->push_back(0 + indexCount);

		indexCount += 4;
	}
}

void Mesh::uploadToGPU() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBO);
	glBufferData(GL_ARRAY_BUFFER, Verts->size() * sizeof(glm::vec3), Verts->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &UVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVVBO);
	glBufferData(GL_ARRAY_BUFFER, UVs->size() * sizeof(glm::vec2), UVs->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &NormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVBO);
	glBufferData(GL_ARRAY_BUFFER, Normals->size() * sizeof(glm::vec3), Normals->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices->size() * sizeof(unsigned int), Indices->data(), GL_STATIC_DRAW);

	glGenBuffers(1, &AOBO);
	glBindBuffer(GL_ARRAY_BUFFER, AOBO);
	glBufferData(GL_ARRAY_BUFFER, AOVals->size() * sizeof(uint8_t), AOVals->data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::render(bool isWater) {

	glBindVertexArray(VAO);

	if (isWater) glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, Indices->size(), GL_UNSIGNED_INT, 0);
	if (isWater) glEnable(GL_CULL_FACE);

	glBindVertexArray(0);

}

void Mesh::addAOVal(int val) {
	AOVals->push_back(val);
}

void Mesh::addVert(glm::vec3 vert) {
	Verts->push_back(vert);
}

void Mesh::addUV(glm::vec2 uv) {
	UVs->emplace_back(uv);
}

void Mesh::addNormal(glm::vec3 normal) {
	Normals->push_back(normal);
}

void Mesh::Delete() {

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VertexVBO);
	glDeleteBuffers(1, &UVVBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &AOBO);
	glDeleteBuffers(1, &NormalVBO);

	Verts->clear();
	UVs->clear();
	Indices->clear();
	AOVals->clear();
	Normals->clear();
	indexCount = 0;

}