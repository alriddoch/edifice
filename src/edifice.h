// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef EDIFICE_EDIFICE_H
#define EDIFICE_EDIFICE_H

#include <GL/osmesa.h>

#include <SDL_image.h>

#include <string>

namespace Edifice {

class Exception { };

class GLError : Exception { };

class SDLError : Exception { };

class Renderer {
  private:
    OSMesaContext context;
    void * buffer;
    size_t blen;
    int width, height;
    int textureId;

    SDL_Surface * imageTransform(SDL_Surface * image);
  public:
    Renderer();
    ~Renderer();

    bool loadTexture(const std::string & name, const std::string & file);
    bool setBuffer() { return false; }
    void render(double, double, double);
    void saveTarga(const std::string & filename);

    static const double meterSize = 40.2492;
};

} // namespace Edifice

#endif // EDIFICE_EDIFICE_H
