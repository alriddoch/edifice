// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "../src/edifice.h"

int main(int argc, char ** argv)
{
    Edifice::Renderer r;

    r.loadTexture("texture", "tex.png");
    r.render(0.25,3.0,2.5);
    r.saveTarga("foo.tga");
}
