FROM ubuntu:20.04

## Remove interactive prompts for apt-get installs
ARG DEBIAN_FRONTEND=noninteractive

ARG EMSDK_VERSION=2.0.12
ARG PDFIUM_BRANCH=82408da2c17d3c58a2de27ac46bb41e89da86991

RUN apt-get update \
    && apt-get install -y wget \
    && apt-get install -y python

RUN apt-get update \
    && apt-get install -y build-essential \
    && apt-get install -y git

RUN apt-get update \
    && apt-get install -y cmake \
    && apt-get install -y xz-utils

RUN apt-get update \
    && apt-get install -y lsb-release \
    && apt-get install -y sudo 
    
    #libjpeg-dev

#################################
## Environment Installs
#################################

## Install Emscripten SDK
WORKDIR /opt
RUN git clone https://github.com/emscripten-core/emsdk.git \
    && cd emsdk \
    && git checkout $EMSDK_VERSION \
    && ./emsdk install $EMSDK_VERSION \
    && ./emsdk activate $EMSDK_VERSION \
    && echo 'source "$(pwd)/emsdk_env.sh"' >> $HOME/.bash_profile

## Install Google Depot Tools
# The git config to set remote.origin.url is to remove a gclient prompt about pointint to an obsolete url
RUN cd /opt \
    && git clone https://chromium.googlesource.com/chromium/tools/depot_tools \
    && cd depot_tools \
    && git config remote.origin.url "https://chromium.googlesource.com/chromium/tools/depot_tools.git" \
    && cd ..
ENV PATH="/opt/depot_tools:${PATH}"


#################################
## Source Code Installs
#################################

#RUN mkdir -p /pdf-sln/runtiva-pdf
VOLUME /pdf-sln/runtiva-pdf
WORKDIR /pdf-sln/runtiva-pdf
#ENTRYPOINT [ "./pdf-viewer/tools/docker-setup.sh" ]

# ## PDFium Source Code
# # Note that we checkout a specific branch to ensure build consistency.
# # we do not want to always be working off of the master branch
# WORKDIR /pdf-sln/runtiva-pdf
# RUN gclient config --unmanaged https://pdfium.googlesource.com/pdfium.git 
# RUN gclient sync
# WORKDIR /pdf-sln/runtiva-pdf/pdfium
# RUN git checkout $PDFIUM_BRANCH


# ## PDFium Build Dependencies
# RUN echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections
# RUN sudo DEBIAN_FRONTEND=noninteractive apt-get install tzdata

# # Patch to remove snapcraft from dependencies as this fails in docker
# COPY ./pdf-viewer/patches/install-build-deps.sh ./build/ 
# RUN chmod +x ./build/install-build-deps.sh 
# RUN echo n | ./build/install-build-deps.sh





#Apply Patches
#COPY ./patches/pdfium.diff /build/pdfium/pdfium.diff
#RUN patch -i pdfium.diff -p1

#COPY ./patches/build.diff /build/pdfium/build/build.diff
#RUN patch -d build -i build.diff -p1




# # Build process
# COPY config/debug.wasm.config /build/pdfium/out/Debug/args.gn
# RUN gn gen out/Debug

# RUN cp /usr/include/jpeglib.h /usr/include/jmorecfg.h /usr/include/zlib.h /usr/include/zconf.h /usr/include/x86_64-linux-gnu/jconfig.h .
# RUN mkdir linux
# RUN cp /usr/include/linux/limits.h linux/limits.h


# ### Build it!
# RUN bash -c 'source /opt/emsdk-portable/emsdk_env.sh && ninja -C out/Debug pdfium'

# ## Cache system libraries
# RUN bash -c 'echo "int main() { return 0; }" > /tmp/main.cc'
# RUN bash -c 'source /opt/emsdk-portable/emsdk_env.sh && em++ -o /tmp/main.html /tmp/main.cc'

# ## Build pdfium-wasm.js
# RUN apt-get install -y doxygen
# WORKDIR /build/pdfium/public
# COPY config/Doxyfile Doxyfile
# RUN doxygen

# ADD utils utils
# RUN bash -ic 'cd utils && npm install'
# ENV OBJ_DIR=/build/pdfium/out/Debug/obj
# RUN bash -ic 'utils/build-pdfium-wasm.sh'




#./build/install-build-deps.sh --no-prompt --no-arm --no-chromeos-fonts --no-backwards-compatible

#/home/roger/pdf-sln/pdfium/runtiva-pdf/pdf-viewer/externals
WORKDIR /pdf-sln/pdfium/runtiva-pdf/pdf-viewer/third_party/externals
RUN git clone https://skia.googlesource.com/third_party/sdl
# RUN wget https://libsdl.org/release/SDL2-2.0.14.tar.gz \
#     && tar zxf SDL2-2.0.14.tar.gz \
#     && mv SDL2-2.0.14 sdl2 \
#     && rm SDL2-2.0.14.tar.gz

# TODO: Update src/dynapi/SDL_dynapi.h -- line: 65 -- #define SDL_DYNAMIC_API 0
# TODO: Update src/SDL_internal.h -- line:58 -- #define SDL_LEAN_AND_MEAN               1 // Runtiva: Patch -- updated to 1
# TODO: Update include/SDL_config.h
#   -#if defined(__WIN32__)
#   +#ifdef USING_PREMAKE_CONFIG_H
#   +#include "SDL_config_premake.h"
#   +#elif defined(__WIN32__)


#SDL_TTF
RUN git clone https://github.com/SDL-mirror/SDL_ttf.git \
    && cd SDL_ttf \
    && git checkout c0295db5fbf0e06148e47be0e7fc37ac8703162b


#SDL2_Image
RUN git clone https://github.com/SDL-mirror/SDL_image.git \
    && cd SDL_image \
    && git checkout b4f28c1fc9ab2f91ae594fc3e86622c5c13ffbbb

#RUN wget https://github.com/emscripten-core/emscripten/archive/2.0.12.tar.gz
#RUN tar zxf 2.0.12.tar.gz



