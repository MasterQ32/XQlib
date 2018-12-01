void uniform(GLint loc, int x) {
	glProgramUniform1i(id, loc, x);
}

void uniform(char const * loc, int x) {
	glProgramUniform1i(id, getUniformLocation(loc), x);
}

void uniform(std::string const & loc, int x) {
	glProgramUniform1i(id, getUniformLocation(loc), x);
}

void uniform(GLint loc, unsigned int x) {
	glProgramUniform1ui(id, loc, x);
}

void uniform(char const * loc, unsigned int x) {
	glProgramUniform1ui(id, getUniformLocation(loc), x);
}

void uniform(std::string const & loc, unsigned int x) {
	glProgramUniform1ui(id, getUniformLocation(loc), x);
}

void uniform(GLint loc, float x) {
	glProgramUniform1f(id, loc, x);
}

void uniform(char const * loc, float x) {
	glProgramUniform1f(id, getUniformLocation(loc), x);
}

void uniform(std::string const & loc, float x) {
	glProgramUniform1f(id, getUniformLocation(loc), x);
}

void uniform(GLint loc, double x) {
	glProgramUniform1d(id, loc, x);
}

void uniform(char const * loc, double x) {
	glProgramUniform1d(id, getUniformLocation(loc), x);
}

void uniform(std::string const & loc, double x) {
	glProgramUniform1d(id, getUniformLocation(loc), x);
}

void uniform(GLint loc, int x, int y) {
	glProgramUniform2i(id, loc, x, y);
}

void uniform(char const * loc, int x, int y) {
	glProgramUniform2i(id, getUniformLocation(loc), x, y);
}

void uniform(std::string const & loc, int x, int y) {
	glProgramUniform2i(id, getUniformLocation(loc), x, y);
}

void uniform(GLint loc, unsigned int x, unsigned int y) {
	glProgramUniform2ui(id, loc, x, y);
}

void uniform(char const * loc, unsigned int x, unsigned int y) {
	glProgramUniform2ui(id, getUniformLocation(loc), x, y);
}

void uniform(std::string const & loc, unsigned int x, unsigned int y) {
	glProgramUniform2ui(id, getUniformLocation(loc), x, y);
}

void uniform(GLint loc, float x, float y) {
	glProgramUniform2f(id, loc, x, y);
}

void uniform(char const * loc, float x, float y) {
	glProgramUniform2f(id, getUniformLocation(loc), x, y);
}

void uniform(std::string const & loc, float x, float y) {
	glProgramUniform2f(id, getUniformLocation(loc), x, y);
}

void uniform(GLint loc, double x, double y) {
	glProgramUniform2d(id, loc, x, y);
}

void uniform(char const * loc, double x, double y) {
	glProgramUniform2d(id, getUniformLocation(loc), x, y);
}

void uniform(std::string const & loc, double x, double y) {
	glProgramUniform2d(id, getUniformLocation(loc), x, y);
}

void uniform(GLint loc, int x, int y, int z) {
	glProgramUniform3i(id, loc, x, y, z);
}

void uniform(char const * loc, int x, int y, int z) {
	glProgramUniform3i(id, getUniformLocation(loc), x, y, z);
}

void uniform(std::string const & loc, int x, int y, int z) {
	glProgramUniform3i(id, getUniformLocation(loc), x, y, z);
}

void uniform(GLint loc, unsigned int x, unsigned int y, unsigned int z) {
	glProgramUniform3ui(id, loc, x, y, z);
}

void uniform(char const * loc, unsigned int x, unsigned int y, unsigned int z) {
	glProgramUniform3ui(id, getUniformLocation(loc), x, y, z);
}

void uniform(std::string const & loc, unsigned int x, unsigned int y, unsigned int z) {
	glProgramUniform3ui(id, getUniformLocation(loc), x, y, z);
}

void uniform(GLint loc, float x, float y, float z) {
	glProgramUniform3f(id, loc, x, y, z);
}

void uniform(char const * loc, float x, float y, float z) {
	glProgramUniform3f(id, getUniformLocation(loc), x, y, z);
}

void uniform(std::string const & loc, float x, float y, float z) {
	glProgramUniform3f(id, getUniformLocation(loc), x, y, z);
}

void uniform(GLint loc, double x, double y, double z) {
	glProgramUniform3d(id, loc, x, y, z);
}

void uniform(char const * loc, double x, double y, double z) {
	glProgramUniform3d(id, getUniformLocation(loc), x, y, z);
}

void uniform(std::string const & loc, double x, double y, double z) {
	glProgramUniform3d(id, getUniformLocation(loc), x, y, z);
}

void uniform(GLint loc, int x, int y, int z, int w) {
	glProgramUniform4i(id, loc, x, y, z, w);
}

void uniform(char const * loc, int x, int y, int z, int w) {
	glProgramUniform4i(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(std::string const & loc, int x, int y, int z, int w) {
	glProgramUniform4i(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(GLint loc, unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	glProgramUniform4ui(id, loc, x, y, z, w);
}

void uniform(char const * loc, unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	glProgramUniform4ui(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(std::string const & loc, unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	glProgramUniform4ui(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(GLint loc, float x, float y, float z, float w) {
	glProgramUniform4f(id, loc, x, y, z, w);
}

void uniform(char const * loc, float x, float y, float z, float w) {
	glProgramUniform4f(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(std::string const & loc, float x, float y, float z, float w) {
	glProgramUniform4f(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(GLint loc, double x, double y, double z, double w) {
	glProgramUniform4d(id, loc, x, y, z, w);
}

void uniform(char const * loc, double x, double y, double z, double w) {
	glProgramUniform4d(id, getUniformLocation(loc), x, y, z, w);
}

void uniform(std::string const & loc, double x, double y, double z, double w) {
	glProgramUniform4d(id, getUniformLocation(loc), x, y, z, w);
}

#ifdef XGL_ENABLE_GLM

void uniform(GLint loc, glm::ivec2 const & vec) {
	glProgramUniform2i(id, loc, vec.x, vec.y);
}

void uniform(char const * loc, glm::ivec2 const & vec) {
	glProgramUniform2i(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(std::string const & loc, glm::ivec2 const & vec) {
	glProgramUniform2i(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(GLint loc, glm::uvec2 const & vec) {
	glProgramUniform2ui(id, loc, vec.x, vec.y);
}

void uniform(char const * loc, glm::uvec2 const & vec) {
	glProgramUniform2ui(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(std::string const & loc, glm::uvec2 const & vec) {
	glProgramUniform2ui(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(GLint loc, glm::vec2 const & vec) {
	glProgramUniform2f(id, loc, vec.x, vec.y);
}

void uniform(char const * loc, glm::vec2 const & vec) {
	glProgramUniform2f(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(std::string const & loc, glm::vec2 const & vec) {
	glProgramUniform2f(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(GLint loc, glm::dvec2 const & vec) {
	glProgramUniform2d(id, loc, vec.x, vec.y);
}

void uniform(char const * loc, glm::dvec2 const & vec) {
	glProgramUniform2d(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(std::string const & loc, glm::dvec2 const & vec) {
	glProgramUniform2d(id, getUniformLocation(loc), vec.x, vec.y);
}

void uniform(GLint loc, glm::ivec3 const & vec) {
	glProgramUniform3i(id, loc, vec.x, vec.y, vec.z);
}

void uniform(char const * loc, glm::ivec3 const & vec) {
	glProgramUniform3i(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(std::string const & loc, glm::ivec3 const & vec) {
	glProgramUniform3i(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(GLint loc, glm::uvec3 const & vec) {
	glProgramUniform3ui(id, loc, vec.x, vec.y, vec.z);
}

void uniform(char const * loc, glm::uvec3 const & vec) {
	glProgramUniform3ui(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(std::string const & loc, glm::uvec3 const & vec) {
	glProgramUniform3ui(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(GLint loc, glm::vec3 const & vec) {
	glProgramUniform3f(id, loc, vec.x, vec.y, vec.z);
}

void uniform(char const * loc, glm::vec3 const & vec) {
	glProgramUniform3f(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(std::string const & loc, glm::vec3 const & vec) {
	glProgramUniform3f(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(GLint loc, glm::dvec3 const & vec) {
	glProgramUniform3d(id, loc, vec.x, vec.y, vec.z);
}

void uniform(char const * loc, glm::dvec3 const & vec) {
	glProgramUniform3d(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(std::string const & loc, glm::dvec3 const & vec) {
	glProgramUniform3d(id, getUniformLocation(loc), vec.x, vec.y, vec.z);
}

void uniform(GLint loc, glm::ivec4 const & vec) {
	glProgramUniform4i(id, loc, vec.x, vec.y, vec.z, vec.w);
}

void uniform(char const * loc, glm::ivec4 const & vec) {
	glProgramUniform4i(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(std::string const & loc, glm::ivec4 const & vec) {
	glProgramUniform4i(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(GLint loc, glm::uvec4 const & vec) {
	glProgramUniform4ui(id, loc, vec.x, vec.y, vec.z, vec.w);
}

void uniform(char const * loc, glm::uvec4 const & vec) {
	glProgramUniform4ui(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(std::string const & loc, glm::uvec4 const & vec) {
	glProgramUniform4ui(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(GLint loc, glm::vec4 const & vec) {
	glProgramUniform4f(id, loc, vec.x, vec.y, vec.z, vec.w);
}

void uniform(char const * loc, glm::vec4 const & vec) {
	glProgramUniform4f(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(std::string const & loc, glm::vec4 const & vec) {
	glProgramUniform4f(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(GLint loc, glm::dvec4 const & vec) {
	glProgramUniform4d(id, loc, vec.x, vec.y, vec.z, vec.w);
}

void uniform(char const * loc, glm::dvec4 const & vec) {
	glProgramUniform4d(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(std::string const & loc, glm::dvec4 const & vec) {
	glProgramUniform4d(id, getUniformLocation(loc), vec.x, vec.y, vec.z, vec.w);
}

void uniform(GLint loc, glm::mat4 const & matrix) {
	glProgramUniformMatrix4fv(id, loc, 1, GL_FALSE, &matrix[0][0]);
}

void uniform(char const * loc, glm::mat4 const & matrix) {
	glProgramUniformMatrix4fv(id, getUniformLocation(loc), 1, GL_FALSE, &matrix[0][0]);
}

void uniform(std::string const & loc, glm::mat4 const & matrix) {
	glProgramUniformMatrix4fv(id, getUniformLocation(loc), 1, GL_FALSE, &matrix[0][0]);
}

#endif
