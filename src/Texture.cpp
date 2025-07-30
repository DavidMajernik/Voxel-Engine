#include <iostream>
#include "Texture.h"

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
	m_textureTarget = TextureTarget;
	m_fileName = FileName;
}

bool Texture::Load()
{
	stbi_set_flip_vertically_on_load(1);
	int width = 0, height = 0, bpp = 0;
	unsigned char* image_data = stbi_load(m_fileName.c_str(), &width, &height, &bpp, 0);

	if (!image_data) {
		printf("Error loading texture: %s\n", m_fileName.c_str());
		exit(0);
	}

	printf("Loaded texture: %s\n", m_fileName.c_str());

	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	if (m_textureTarget == GL_TEXTURE_2D) {
		if (bpp == 3) {
			glTexImage2D(m_textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		}
		else if (bpp == 4) {
			glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		}
		else {
			printf("Channel number error: %s\n", m_fileName.c_str());
		}
	}
	else {
		printf("Support for texture target %x not implemented\n", m_textureTarget);
	}

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(m_textureTarget, 0);

	stbi_image_free(image_data);
}

void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}


