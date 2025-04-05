#pragma once

#include <string>
#include <glad/glad.h>

#include "C:\Users\dpmaj\LearnC++\SlimeCraft\LinkerStuff\dependencies\stb_image.h"

class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;


};
