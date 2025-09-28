#pragma once
#include <vector>
#include <glad/glad.h>

namespace NullEngine {

	class SkyBox
	{
	public:
		//! #TODO maybe not needed in future
		SkyBox() = default;
		//! Init constructor
		SkyBox(const std::vector<float> &vertexData) { Init(vertexData); }

		//! Bing to OpenGL
		void Bind() { glBindVertexArray(_VAO); }
		//! Initialization with vertices
		void Init(const std::vector<float> &vertexData);
		//! OpenGL Vertex array object & Vertex buffer object
		unsigned _VAO, _VBO;
	};

} // namespace NullEngine