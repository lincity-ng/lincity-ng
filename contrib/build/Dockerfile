FROM ubuntu

RUN apt-get update

RUN apt-get install -y \
  libsdl2-2.0-0 \
  libsdl2-image-2.0-0 \
  libsdl2-mixer-2.0-0 \
  libsdl2-ttf-2.0-0 \
  libphysfs1 \
  zlib1g \
  libxml2 \
  libfmt9
RUN apt-get install -y \
  build-essential \
  cmake \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-mixer-dev \
  libsdl2-ttf-dev \
  libphysfs-dev \
  zlib1g-dev \
  libxml2-dev \
  libxslt1.1 \
  libxslt1-dev \
  libfmt-dev \
  xsltproc \
  git \
  meson \
  curl
  # gettext \

# install libxml++ 5.0
WORKDIR /home/ubuntu
RUN curl -L https://github.com/libxmlplusplus/libxmlplusplus/releases/download/5.4.0/libxml++-5.4.0.tar.xz --output libxml++-5.4.0.tar.xz
RUN tar -xJf /home/ubuntu/libxml++-5.4.0.tar.xz
WORKDIR /home/ubuntu/libxml++-5.4.0
RUN meson setup --default-library static . build-ubuntu
RUN ninja -C build-ubuntu install

RUN mkdir -p /home/ubuntu/lincity-ng
# COPY . /home/ubuntu/lincity-ng/
WORKDIR /home/ubuntu/lincity-ng

CMD bash -c "cmake -B build/ubuntu -DCMAKE_BUILD_TYPE=Release && cmake --build build/ubuntu --parallel -t package"
