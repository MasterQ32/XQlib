#include "../include/xgraphics/shader_preprocessor"

#include <xlog>
#include <algorithm>
#include <xcept>

using xgraphics::shader_preprocessor;

void shader_preprocessor::add_include(std::string const & name, std::string const & src)
{
	this->include_files.emplace(name, src);
}

void shader_preprocessor::add_define(std::string const & define)
{
	this->defines.emplace(define);
}

static std::string make_include_guard_name(std::string const & src)
{
	std::string result = ("__" + src + "__");
	for(char & c : result)
	{
		if(isalpha(c))
			c = char(toupper(c));
		else if(not isascii(c))
			c = 'X';
		else if(isdigit(c))
			; // no-op
		else
			c = '_';
	}
	return result;
}

std::string shader_preprocessor::preprocess(std::string const & src, std::string const & chunk_name) const
{
	std::string result;
	size_t start_of_line = 0;
	size_t line_number = 1;
	bool add_include_guard = false;
	for(size_t i = 0; i < src.size(); i++)
	{
		char c = src.at(i);
		if(c != '\n')
			continue;
		if(i >= start_of_line)
		{
			auto const line = src.substr(start_of_line, i - start_of_line);
			auto const offset = line.find('#');
			if(offset != line.npos)
			{
				if(not std::all_of(line.begin(), line.begin() + ssize_t(offset), isspace))
					throw xcept::compile_error("# must only be preceeded by spaces!");
				auto const directive = line.substr(offset);
				if(directive == "#pragma once")
				{
					if(chunk_name.empty())
						xlog::log("shaderpp", xlog::warning) << "Tried to use `#pragma once` in a root chunk!";
					else
						add_include_guard = true;
				}
				else if(directive.substr(0, 8) == "#include")
				{
					auto const start = directive.find('<');
					auto const end   = directive.find('>');
					if((start == directive.npos) or (end == directive.npos))
						throw xcept::compile_error("#include requires < > for file name!");
					auto const file_name = directive.substr(start + 1, end - start - 1);

					auto const include_file = this->include_files.find(file_name);
					if(include_file == this->include_files.end())
						throw xcept::compile_error("The include <" + file_name + "> does not exist!");

					result.append(preprocess(include_file->second, include_file->first));

					result.append("\n#line ");
					result.append(std::to_string(line_number + 1));
					result.append("\n");
				}
				else
				{
					result.append(line);
					result.append("\n");
				}
			}
			else
			{
				result += line;
				result.append("\n");
			}
		}
		start_of_line = i + 1;
		line_number += 1;
	}
	if(add_include_guard)
	{
		auto const guard = make_include_guard_name(chunk_name);
		return "#ifndef " + guard + "\n"
		       "#define " + guard + "\n"
					 "#line 1\n" +
					 result +
					 "#endif\n" ;
	}
	else
	{
		return result;
	}
}
