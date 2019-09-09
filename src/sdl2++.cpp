#include "../include/sdl2++/renderer"
#include "../include/sdl2++/exception"

const char *sdl::exception::what() const noexcept
{
    return SDL_GetError();
}

sdl2::renderer::renderer(SDL_Window *window, Uint32 flags, int index)
    : ptr(SDL_CreateRenderer(window, index, flags))
{

}

sdl2::renderer::renderer(SDL_Surface *surface)
    : ptr(SDL_CreateSoftwareRenderer(surface))
{

}



void sdl2::renderer::clear()
{
    SDL_RenderClear(*this);
}

void sdl2::renderer::copy(SDL_Texture *texture)
{
    SDL_RenderCopy(
                *this,
                texture,
                nullptr,
                nullptr
                );
}

void sdl2::renderer::copy(SDL_Texture *texture, const SDL_Rect &dest)
{
    SDL_RenderCopy(
                *this,
                texture,
                nullptr,
                &dest
                );
}

void sdl2::renderer::copy(SDL_Texture *texture, const SDL_Rect &dest, const SDL_Rect &src)
{
    SDL_RenderCopy(
                *this,
                texture,
                &src,
                &dest
                );
}

void sdl2::renderer::drawLine(int x1, int y1, int x2, int y2)
{
    SDL_RenderDrawLine(*this, x1, y1, x2, y2);
}

void sdl2::renderer::drawLine(SDL_Point p0, SDL_Point p1)
{
    SDL_RenderDrawLine(*this, p0.x, p0.y, p1.x, p1.y);
}

void sdl2::renderer::drawLines(const SDL_Point *points, size_t count)
{
    SDL_RenderDrawLines(
                *this,
                points,
                gsl::narrow<int>(count)
                );
}

void sdl2::renderer::drawPoint(int x, int y)
{
    SDL_RenderDrawPoint(*this, x, y);
}

void sdl2::renderer::drawPoints(const SDL_Point *points, size_t count)
{
    SDL_RenderDrawPoints(
                *this,
                points,
                gsl::narrow<int>(count)
                );
}

void sdl2::renderer::drawRect(int x, int y, int w, int h)
{
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(*this, &rect);
}

void sdl2::renderer::drawRect(const SDL_Rect &rect)
{
    SDL_RenderDrawRect(*this, &rect);
}

void sdl2::renderer::drawRects(const SDL_Rect *rects, size_t count)
{
    SDL_RenderDrawRects(
                *this,
                rects,
                gsl::narrow<int>(count)
                );
}

void sdl2::renderer::fillRect(const SDL_Rect &rect)
{
    SDL_RenderFillRect(*this, &rect);
}

void sdl2::renderer::fillRects(const SDL_Rect *rects, size_t count)
{
    SDL_RenderFillRects(
                *this,
                rects,
                gsl::narrow<int>(count)
                );
}

SDL_Rect sdl2::renderer::getClipRect() const
{
    SDL_Rect result;
    SDL_RenderGetClipRect(*this, &result);
    return result;
}

bool sdl2::renderer::isTargetSupported() const {
    return SDL_RenderTargetSupported(*this);
}



bool sdl2::renderer::hasIntegerScale() const {
    return SDL_RenderGetIntegerScale(*this);
}

std::tuple<int, int> sdl2::renderer::getLogicalSize() const {
    int w, h;
    SDL_RenderGetLogicalSize(*this, &w, &h);
    return std::make_tuple(w, h);
}

std::tuple<float, float> sdl2::renderer::getScale() const
{
    float h, v;
    SDL_RenderGetScale(*this, &h, &v);
    return std::make_tuple(h, v);
}

SDL_Rect sdl2::renderer::getViewport() const {
    SDL_Rect res;
    SDL_RenderGetViewport(*this, &res);
    return res;
}

bool sdl2::renderer::isClipEnabled() const {
    return SDL_RenderIsClipEnabled(*this);
}

void sdl2::renderer::present()
{
    SDL_RenderPresent(*this);
}

void sdl2::renderer::readPixels(SDL_Rect region, Uint32 format, void *target, size_t pitch)
{
    SDL_RenderReadPixels(*this, &region, format, target, gsl::narrow<int>(pitch));
}

void sdl2::renderer::setClipRect(const SDL_Rect &rect)
{
    SDL_RenderSetClipRect(*this, &rect);
}

void sdl2::renderer::enableIntegerScale()
{
    SDL_RenderSetIntegerScale(*this, SDL_TRUE);
}

void sdl2::renderer::disableIntergerScale()
{
    SDL_RenderSetIntegerScale(*this, SDL_FALSE);
}

void sdl2::renderer::setIntegerScale(bool value)
{
    SDL_RenderSetIntegerScale(*this, value ? SDL_TRUE : SDL_FALSE);
}

void sdl2::renderer::setLogicalSize(int w, int h)
{
    SDL_RenderSetLogicalSize(*this, w, h);
}

void sdl2::renderer::setScale(float h, float v)
{
    SDL_RenderSetScale(*this, h, v);
}

void sdl2::renderer::setViewport(const SDL_Rect &rect)
{
    SDL_RenderSetViewport(*this, &rect);
}

void sdl2::renderer::setViewport(int x, int y, int w, int h)
{
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderSetViewport(*this, &rect);
}

void sdl2::renderer::setBlendMode(SDL_BlendMode blendMode)
{
    if(SDL_SetRenderDrawBlendMode(*this, blendMode) < 0)
        throw sdl::exception();
}

SDL_BlendMode sdl2::renderer::getBlendMode() const
{
    SDL_BlendMode mode;
    if(SDL_GetRenderDrawBlendMode(*this, &mode) < 0)
        throw sdl::exception();
    return mode;
}

void sdl2::renderer::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if(SDL_SetRenderDrawColor(*this, r, g, b, a) < 0)
        throw sdl::exception();
}

void sdl2::renderer::setColor(const SDL_Color &color)
{
    if(SDL_SetRenderDrawColor(*this, color.r, color.g, color.b, color.a) < 0)
        throw sdl::exception();
}

SDL_Color sdl2::renderer::getColor() const
{
    SDL_Color color;
    if(SDL_GetRenderDrawColor(*this, &color.r, &color.g, &color.b, &color.a) < 0)
        throw sdl::exception();
    return color;
}


void sdl2::renderer::setRenderTarget(SDL_Texture *target)
{
    if(SDL_SetRenderTarget(*this, target) < 0)
        throw sdl::exception();
}

SDL_Texture *sdl2::renderer::getRenderTarget() const
{
    SDL_GetRenderTarget(*this);
}

std::tuple<int, int> sdl2::renderer::getOutputSize() const
{
    int w, h;
    if(SDL_GetRendererOutputSize(*this, &w, &h) < 0)
        throw sdl::exception();
    return std::make_tuple(w, h);
}
