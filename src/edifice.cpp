// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "edifice.h"

#include <GL/glut.h>

#include <iostream>

extern "C" {
    #include <stdio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
}

#define C_TSZ 4

namespace Edifice {

Renderer::Renderer() : buffer(NULL), width(200), height(200)
{
    context = OSMesaCreateContext(OSMESA_RGBA, NULL);
    if (context == 0) {
        throw GLError();
    }

    blen = (200 * 200 + 1) * C_TSZ;
    buffer = ::operator new(blen);

    if (OSMesaMakeCurrent(context, buffer, GL_UNSIGNED_BYTE, width, height)
                          == GL_FALSE) {
        throw GLError();
    }

}

Renderer::~Renderer()
{
    if (buffer != NULL) {
        ::operator delete(buffer);
    }
}

bool Renderer::loadTexture(const std::string & name, const std::string & file)
// This code was created by Jeff Molofee '99
// (ported to SDL by Sam Lantinga '2000)
//
// If you've found this code useful, please let me know.
{
    int tex_id;
    int format;

    SDL_Surface *image;

    image = IMG_Load(file.c_str());
    if ( image == NULL ) {
        std::cout << "Failed to load image" << std::endl << std::flush;
        throw SDLError();
    }

    image = imageTransform(image);

    int bpp = image->format->BitsPerPixel;

    if(bpp != 24 && bpp != 32) {
        SDL_FreeSurface(image);
        std::cout << "Image bpp is " << bpp << std::endl << std::flush;
        throw SDLError();
    }

    format = (bpp == 24) ? GL_RGB : GL_RGBA;

    /* load the texture into OGL */
    glGenTextures(1, (unsigned int *)&tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0,
                 format, GL_UNSIGNED_BYTE, image->pixels);
    std::cout << image->w << " " << image->h << std::endl << std::flush;
    int foo;
    if ((foo = glGetError()) != 0) {
        std::cout << "Texture installation failed" << foo << std::endl << std::flush;
        throw GLError();
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    textureId = tex_id;
    return true;
}

SDL_Surface * Renderer::imageTransform(SDL_Surface * image)
// This code was created by Jeff Molofee '99
// (ported to SDL by Sam Lantinga '2000)
//
// If you've found this code useful, please let me know.
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf, tmpch;
    int i, j;

    // GL surfaces are upsidedown and RGB, not BGR :-)
    tmpbuf = (Uint8 *)malloc(image->pitch);
    if ( tmpbuf == NULL ) {
        fprintf(stderr, "Out of memory\n");
        return(NULL);
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
        memcpy(tmpbuf, rowhi, image->pitch);
        memcpy(rowhi, rowlo, image->pitch);
        memcpy(rowlo, tmpbuf, image->pitch);
        rowhi += image->pitch;
        rowlo -= image->pitch;
    }
    free(tmpbuf);
    return(image);
}

void Renderer::render(double x, double y, double z)
{
    std::cout << "Render" << std::endl << std::flush;

    // Light parameters
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    // Set up lights
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    // Enable lights
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_DEPTH_TEST);

    // Set up uclient scale isometric projection.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/(meterSize*2), width/(meterSize*2), 0, height/meterSize,
            -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);

    // Clear the image
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Rotate axis to isometric alignment
    glRotatef(-60.0, 1.0, 0.0, 0.0);
    glRotatef(45.0, 0.0, 0.0, 1.0);

    // Set up the textrure
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_TEXTURE_2D);

    // Draw the wall section.
    glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(0,0,z);
    glTexCoord2f(x,0); glVertex3f(x,0,z);
    glTexCoord2f(x,y); glVertex3f(x,y,z);
    glTexCoord2f(0,y); glVertex3f(0,y,z);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(x,0); glVertex3f(x,0,0);
    glTexCoord2f(x,z); glVertex3f(x,0,z);
    glTexCoord2f(0,z); glVertex3f(0,0,z);
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(0,z); glVertex3f(0,0,z);
    glTexCoord2f(y,z); glVertex3f(0,y,z);
    glTexCoord2f(y,0); glVertex3f(0,y,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // If this is not here, nothing appears in the output. WHY!
    glPopMatrix();
}

void Renderer::saveTarga(const std::string & filename)
{
#ifdef WRITE_RAW
    // std::ofstream out("test.out");
    int fd = open("test.out", O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        perror("open");
    }

    int linelen = width * C_TSZ;
    char * buf = (char *)buffer;
    for(int i = 0; i < height; i++, buf += linelen) {
        int f = write(fd, buf, linelen);
        if (f != linelen) {
            cerr << "Write wrong ammmount " << fd << "," << f << "," << blen << std::endl << std::flush;
            perror("write");
        }
    }
#endif
   FILE *f = fopen( filename.c_str(), "w" );
   if (f) {
      int i, x, y;
      const GLubyte *ptr = (GLubyte *)buffer;
      printf ("osdemo, writing tga file \n");
      fputc (0x00, f);  /* ID Length, 0 => No ID        */
      fputc (0x00, f);  /* Color Map Type, 0 => No color map included   */
      fputc (0x02, f);  /* Image Type, 2 => Uncompressed, True-color Image */
      fputc (0x00, f);  /* Next five bytes are about the color map entries */
      fputc (0x00, f);  /* 2 bytes Index, 2 bytes length, 1 byte size */
      fputc (0x00, f);
      fputc (0x00, f);
      fputc (0x00, f);
      fputc (0x00, f);  /* X-origin of Image    */
      fputc (0x00, f);
      fputc (0x00, f);  /* Y-origin of Image    */
      fputc (0x00, f);
      fputc (width & 0xff, f);      /* Image Width      */
      fputc ((width>>8) & 0xff, f);
      fputc (height & 0xff, f);     /* Image Height     */
      fputc ((height>>8) & 0xff, f);
      fputc (0x18, f);          /* Pixel Depth, 0x18 => 24 Bits */
      fputc (0x20, f);          /* Image Descriptor     */
      fclose(f);
      f = fopen( filename.c_str(), "ab" );  /* reopen in binary append mode */
      for (y=height-1; y>=0; y--) {
         for (x=0; x<width; x++) {
            i = (y*width + x) * 4;
            fputc(ptr[i+2], f); /* write blue */
            fputc(ptr[i+1], f); /* write green */
            fputc(ptr[i], f);   /* write red */
         }
      }
   }
}

} // namespace Edifice
