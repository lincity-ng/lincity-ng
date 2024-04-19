FROM ubuntu

RUN apt-get update

RUN apt-get install -y \
  libsdl2-2.0-0 \
  libsdl2-gfx-1.0-0 \
  libsdl2-image-2.0-0 \
  libsdl2-mixer-2.0-0 \
  libsdl2-ttf-2.0-0 \
  libphysfs1 \
  zlib1g \
  libxml++2.6-2v5
RUN apt-get install -y \
  build-essential \
  cmake \
  libsdl2-dev \
  libsdl2-gfx-dev \
  libsdl2-image-dev \
  libsdl2-mixer-dev \
  libsdl2-ttf-dev \
  libphysfs-dev \
  zlib1g-dev \
  libxml++2.6-dev \
  libxslt1.1 \
  libxslt1-dev \
  xsltproc \
  gettext \
  git

RUN mkdir -p /home/lincity-ng
# COPY . /home/lincity-ng/
WORKDIR /home/lincity-ng

CMD bash -c "cmake -B build-ubuntu && cmake --build build-ubuntu --parallel -t package -t package_source"
