#pragma once
#include "stdafx.h"

namespace Shader {
	uint buildVertexShader(std::string const& filename);
	uint buildGeometryShader(std::string const& filename);
	uint buildFragShader(std::string const& filename);
	void deleteShader(uint shader);
}

namespace Texture {
	uint loadFromFile(std::string const& filename, bool RGBA = true);
}

class ShaderProgram {
public:
	ShaderProgram();
	ShaderProgram(std::string const& vertPath, std::string const& fragPath);
	ShaderProgram(std::string const& vertPath, std::string const& geomPath, std::string const& fragPath);
	~ShaderProgram();

	void setVertexShader(uint shader);
	void setVertexShader(std::string const& filename);
	void setGeometryShader(uint shader);
	void setGeometryShader(std::string const& filename);
	void setFragShader(uint shader);
	void setFragShader(std::string const& filename);

	void link();

	const uint ID;
private:
	std::set<uint> exclusiveShaders;
};

