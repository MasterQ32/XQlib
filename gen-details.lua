local types = {
	-- C++, GL, GLM
	{ "int", "i", "i" },
	{ "unsigned int", "ui", "u" },
	{ "float", "f", "" },
	{ "double", "d", "d" },
}

local args = {
	{ "x" },
	{ "x", "y" },
	{ "x", "y", "z" },
	{ "x", "y", "z", "w" },
}

local f = io.open("include/xgl/detail/program_uniforms.h", "w")

-- Default parameters
for i=1,4 do
	for _,t in ipairs(types) do
		local at = args[i]

		function put(decl, call)

			f:write("void uniform(",decl)

			for x=1,#at do
				f:write(", ", t[1], " ", at[x])
			end

			f:write(") {\n\tglProgramUniform")
			f:write(i)
			f:write(t[2])
			f:write("(id, ", call)

			for x=1,#at do
				f:write(", ")
				f:write(at[x])
			end

			f:write(");\n}\n\n");
		end
		put("GLint loc", "loc")
		put("char const * loc", "getUniformLocation(loc)")
		put("std::string const & loc", "getUniformLocation(loc)")


	end
end

-- Generate GLM stuff

glm_manual = {
	{
		"glm::mat4 const & matrix",
		{ "Matrix4fv", "1, GL_FALSE, &matrix[0][0]" }
	}
}

f:write("#ifdef XGL_ENABLE_GLM\n\n")

for i=2,4 do
	for _,t in ipairs(types) do
		local at = args[i]
		function put(decl, call)

			f:write("void uniform(",decl,", glm::", t[3], "vec", i, " const & vec)")
			f:write(" {\n\tglProgramUniform")
			f:write(i)
			f:write(t[2])
			f:write("(id, ", call)
			for x=1,#at do
				f:write(", vec.")
				f:write(at[x])
			end
			f:write(");\n}\n\n");
		end
		put("GLint loc", "loc")
		put("char const * loc", "getUniformLocation(loc)")
		put("std::string const & loc", "getUniformLocation(loc)")

	end
end

for _,val in ipairs(glm_manual) do
	function put(decl, call)
		f:write("void uniform(",decl,", ",val[1],")")
		f:write(" {\n\tglProgramUniform")
		f:write(val[2][1])
		f:write("(id, ", call, ", ")
		f:write(val[2][2])
		f:write(");\n}\n\n");
	end
	put("GLint loc", "loc")
	put("char const * loc", "getUniformLocation(loc)")
	put("std::string const & loc", "getUniformLocation(loc)")
end

f:write("#endif\n")




f:close()
