dnl worldforge check for libz, libpng, SDL and SDL_image
dnl the flags are returned in WF_SDL_IMAGE_CFLAGS and WF_SDL_IMAGE_LIBS
AC_DEFUN(WF_CHECK_SDL_IMAGE,[dnl

AC_LANG_PUSH([C])

dnl save CFLAGS and LIBS, so we can munge them when calling tests

WF_SDL_IMAGE_TMP_CFLAGS="$CFLAGS"
WF_SDL_IMAGE_TMP_LIBS="$LIBS"
WF_SDL_IMAGE_TMP_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $LIBS"
LIBS=""

dnl check for libpng prefix option
AC_ARG_WITH(png-prefix,[  --with-png-prefix=PFX   Prefix where libpng is installed (option
al)],[dnl
	WF_SDL_IMAGE_CFLAGS="$WF_SDL_IMAGE_CFLAGS -I$withval/include"
	WF_SDL_IMAGE_LIBS="$WF_SDL_IMAGE_LIBS -L$withval/lib"
],[])

dnl check for libz prefix configure option
AC_ARG_WITH(z-prefix,[  --with-z-prefix=PFX     Prefix where libz is installed (optional)]
,[dnl
	WF_SDL_IMAGE_CFLAGS="$WF_SDL_IMAGE_CFLAGS -I$withval/include"
	WF_SDL_IMAGE_LIBS="$WF_SDL_IMAGE_LIBS -L$withval/lib"
],[])

CFLAGS="$CFLAGS $WF_SDL_IMAGE_CFLAGS"
LDFLAGS="$LDFLAGS $WF_SDL_IMAGE_LIBS"

dnl FIXME AC_CHECK_LIB() doesn't put -lfoo after the -L args
dnl we've inserted in LIBS, this may break on some platforms.

dnl check for libpng and libz

AC_CHECK_HEADER(zlib.h,[],AC_MSG_ERROR( You need libz headers for SDL_image!))
LIBS="-lz $LIBS"
AC_MSG_CHECKING(for libz)
AC_TRY_LINK([
		#include <zlib.h>
	],[
		int main()
		{
			zlibVersion();
			return 0;
		}
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR( You need libz for SDL_image!)
	])


AC_CHECK_HEADER(png.h,[],[AC_MSG_ERROR( You need libpng headers for SDL_image!)])
LIBS="-lpng $LIBS"
AC_MSG_CHECKING(for libpng)
AC_TRY_LINK([
		#include <png.h>
	],[
		int main()
		{
			png_access_version_number();
			return 0;
		}
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR( You need libpng for SDL_image!)
	])

WF_SDL_IMAGE_LIBS="$WF_SDL_IMAGE_LIBS -lpng -lz"

dnl Check for SDL and SDL_image

AM_PATH_SDL($1,,AC_MSG_ERROR([Couldn't find libSDL $1 or greater]))

dnl We pull in SDL_LIBS twice, to get the -L args before
dnl SDL_LIBS, but the -l args after as well. We pull in
dnl -lSDL_image here as well, so we can test for it
dnl in the next check.
WF_SDL_IMAGE_CFLAGS="$SDL_CFLAGS $WF_SDL_IMAGE_CFLAGS"
WF_SDL_IMAGE_LIBS="$SDL_LIBS -lSDL_image $SDL_LIBS $WF_SDL_IMAGE_LIBS"

CFLAGS="$WF_SDL_IMAGE_TMP_CFLAGS $WF_SDL_IMAGE_CFLAGS"
LIBS="$WF_SDL_IMAGE_TMP_LIBS $WF_SDL_IMAGE_LIBS"
LDFLAGS="$WF_SDL_IMAGE_TMP_LDFLAGS"

AC_CHECK_HEADER(SDL/SDL_image.h,[],[AC_MSG_ERROR(Missing SDL_image.h)])

dnl Use AC_TRY_LINK for a custom SDL_image check
AC_MSG_CHECKING(for SDL_image)
AC_TRY_LINK([
		#include <SDL/SDL_image.h>
		// use our own main(), so SDL_main links properly
		#if 0
	],[
		#endif
		int main(int argc, char** argv) {
		IMG_Load(0);
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR(Couldn't link libSDL_image)
	])

dnl Got it, now clean up

CFLAGS="$WF_SDL_IMAGE_TMP_CFLAGS"
LIBS="$WF_SDL_IMAGE_TMP_LIBS"
LDFLAGS="$WF_SDL_IMAGE_TMP_LDFLAGS"

AC_LANG_POP([C])

])dnl end of WF_CHECK_SDL_IMAGE

dnl worldforge check for the GL and GLU libraries,
dnl the -l flags are added directly to LIBS

AC_DEFUN(WF_CHECK_GL_LIBS,[dnl

AC_LANG_PUSH(C)

AC_CHECK_LIB(GL,glViewport, ,
    AC_MSG_CHECKING(for glViewport in opengl32)
    LIBS="$LIBS -lopengl32"
    AC_TRY_LINK([
	#ifdef _WIN32
	#include <windows.h>
	#endif /* _WIN32 */
	#include <GL/gl.h>
    ],[
	glViewport(0, 0, 0, 0);
    ],[
	AC_MSG_RESULT(yes)
    ],[
	AC_MSG_RESULT(no)
	AC_MSG_ERROR(Could not find OpenGL library)
    ])
)

AC_CHECK_LIB(GLU,gluPerspective, ,
    AC_MSG_CHECKING(for gluPerspective in glu32)
    LIBS="$LIBS -lglu32"
    AC_TRY_LINK([
	#ifdef _WIN32
	#include <windows.h>
	#endif /* _WIN32 */
	#include <GL/glu.h>
    ],[
	gluPerspective(0.0, 0.0, 0.0, 0.0);
    ],[
	AC_MSG_RESULT(yes)
    ],[
	AC_MSG_RESULT(no)
	AC_MSG_ERROR(Could not find OpenGL U library)
    ])
)

AC_LANG_POP(C)

])dnl end of WF_CHECK_GL_LIBS
