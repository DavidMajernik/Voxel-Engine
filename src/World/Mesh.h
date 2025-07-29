#pragma once  
#include <memory>  
#include <vector>  
#include <glm/glm.hpp> 
#include <glad/glad.h>  

class Mesh {  

public:   

	Mesh();  

	void addIndices(int amtFaces);
	void uploadToGPU();
	void render(bool isWater);
	void addAOVal(int val);
	void addVert(glm::vec3 vert);
	void addUV(glm::vec2 uv);
	void addNormal(glm::vec3 normal);
	void Delete();  

private:  

	int indexCount;

	unsigned int VAO;  
	unsigned int VertexVBO;  
	unsigned int UVVBO;  
	unsigned int EBO;  
	unsigned int AOBO;  
	unsigned int NormalVBO;  

	std::unique_ptr < std::vector<glm::vec3>> Verts;  
	std::unique_ptr < std::vector<glm::vec2>> UVs;  
	std::unique_ptr < std::vector<unsigned int>> Indices;  
	std::unique_ptr<std::vector<uint8_t>> AOVals;  
	std::unique_ptr<std::vector<glm::vec3>> Normals;  
};