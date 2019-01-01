#ifndef XQLIB_TURTLE_FONT_HPP
#define XQLIB_TURTLE_FONT_HPP

#include <unordered_map>
#include <string>

namespace xgraphics::turtle_font
{
	struct font_style
	{
		int height; ///< height of the font in pixels.
		int stroke; ///< width of the lines in pixels.
		int dotsize; ///< radius of the dots in pixels.
	};

	struct rendertarget
	{
		void (*put)(int x, int y);
		void * context;
	};

	struct font
	{
		using codepoint = uint32_t;
		using glyph_code = std::string;

		std::unordered_map<codepoint, glyph_code> glyphs;

		void render_glyph(int x, int y, char c);
		void render_code(int x, int y, glyph_code const & code);
		void render_text(int x, int y, std::string const & text);
	};
}

#endif // XQLIB_TURTLE_FONT_HPP
