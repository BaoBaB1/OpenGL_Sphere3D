#include "Texture.hpp"
#include "glad/glad.h"
#include <stdexcept>

Texture::Texture() : m_id(0), m_width(0), m_height(0), m_nchannels(0), m_disabled(true) {
}

Texture::Texture(const std::string& filename) {
	load(filename);
}

void Texture::load(const std::string& filename) {
	if (m_id == 0)
		glGenTextures(1, &m_id);
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename.c_str(), &m_width, &m_height, &m_nchannels, 0);
	if (data) {
		glBindTexture(GL_TEXTURE_2D, m_id);
		// texture wrapping around x,y axes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// filtering methods 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_LINEAR_MIPMAP_LINEAR*/GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		m_disabled = false;
	}
	else {
		throw std::runtime_error(std::string("Could not load texture from file ") + filename);
	}
}

Texture::~Texture() {
	if (m_id != 0) {
		glDeleteTextures(1, &m_id);
	}
}
