#include "../include/xgraphics/texture_loader"
#include "../include/xlog"

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::optional<xgl::texture2D> xgraphics::load_texture(std::string const & file)
{
	int _w, _h;
	std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> mem {
		stbi_load(file.c_str(), &_w, &_h, nullptr, 4),
		stbi_image_free
	};
	if(not mem)
	{
		xlog::log("texture_loader", xlog::error) << "could not load texture '" << file << "'.";
		return std::nullopt;
	}

	unsigned int w = gsl::narrow<unsigned int>(_w);
	unsigned int h = gsl::narrow<unsigned int>(_h);

	std::vector<stbi_uc> line(4 * w);
	for(size_t y = 0; y < h / 2; y++)
	{
		memcpy(line.data(), mem.get() + line.size() * y, line.size());
		memcpy(mem.get() + line.size() * y, mem.get() + line.size() * (h - y - 1), line.size());
		memcpy(mem.get() + line.size() * (h - y - 1), line.data(), line.size());
	}

	xgl::texture2D tex;
	tex.storage2D(1, GL_RGBA8, w, h);
	tex.subImage2D(0, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, mem.get());
	return std::move(tex);
}
