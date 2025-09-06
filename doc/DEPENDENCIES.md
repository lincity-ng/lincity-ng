# Dependencies

Following is the list of dependencies for building and/or running LinCity-NG.

### Runtime Dependencies

* SDL2 2.0.18 or later

  http://www.libsdl.org

* SDL2_mixer 2.0.0 or later (with ogg-support enabled)

  http://www.libsdl.org/projects/SDL_mixer/

* SDL2_image 2.0.0 or later (with png support)

  http://www.libsdl.org/projects/SDL_image/

* SDL2_ttf 2.0.12 or later

  http://www.libsdl.org/projects/SDL_ttf/

* zlib 1.0 or later

  https://zlib.net/

* libxml2 2.6.11 or later (recommended 2.13.0 or later)

  http://xmlsoft.org/

* libxml++ 5.0

  https://libxmlplusplus.github.io/libxmlplusplus/

* fmt 9.0.0 or later

  https://fmt.dev/latest/index.html

* libintl (optional)

  https://www.gnu.org/software/gettext/

  Required for native language support.

  Some libc implementations provide libintl built-in. If this is the case,
  libintl need not be separately installed.

### Build Dependencies

* C++ compiler supporting C++17 (such as gcc)

  https://gcc.gnu.org/

* CMake 3.21 or later

  https://cmake.org/

* xsltproc

  https://gitlab.gnome.org/GNOME/libxslt

  xsltproc is provided by LibXslt

* Header files for all [runtime dependencies](#runtime-dependencies)

  If you use packages from your distribution, header files are sometimes
  provided in separate `*-dev` packages.

* Include What You Use (optional)

  https://include-what-you-use.org/

  Recommended for identifying and fixing potential `#include` issues.

* gettext (optional)

  https://www.gnu.org/software/gettext/

  Required for native language support.


## Other Optional Tools

* git

  https://git-scm.com/

  git is only necessary for cloning the LinCity-NG git repository as described
  in the readme. If sources are obtained via other means, e.g. downloading a
  source package from the LinCity-NG GitHub releases, then git is not needed.

* Python, Bash, Ammonite

  Some helper scripts are written in Python, Bash, and Scala (Ammonite), but
  none of these scripts are necessary for building the project.

* Docker

  A Dockerfile is provided in contrib/build/ that will package LinCity-NG for
  Ubuntu. Naturally, Docker is required for this.
