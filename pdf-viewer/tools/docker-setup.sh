#!/usr/bin/env bash
set -e

### Folder Structure for Runtiva PDF Development Environment:
## Ideally this should structure is located directly in your home directory so that the directory structure in Docker is identical
## pdf-sln  -- Root directory
##   - runtiva-pdf  (This is the root of the runtiva-pdf repo. 
##          ****  THIS SCRIPT IS TO BE RUN FROM runtiva-pdf directory  ****
##          All source code, including 3rd party is under this directory)
##     - pdfium
##       -- pdfium code, thirdparty and chromium build repo are in here
##     - pdf-viewer
##       -- thirdparty (pdf-viewer specific thirdparty code)

EMSDK_VERSION=2.0.12
PDFIUM_BRANCH=82408da2c17d3c58a2de27ac46bb41e89da86991


#Validate that we are in the pdf-viewer OR pdf-viewer/tools directory
dir=$(pwd)
ROOT="/pdf-sln"
# todo: Calculate root instead of hardcode 
#if [[ "$dir" == *"$"* ]]

current_directory_name="$(basename $dir)"
if [ "$current_directory_name" != "runtiva-pdf" ]; then
  echo "This script must be run from within the runtiva-pdf directory"
  exit 1
fi


## PDFium Source Code
# Note that we checkout a specific branch to ensure build consistency.
# we do not want to always be working off of the master branch
install_pdfium() {
    echo "pushd $ROOT/runtiva-pdf"
    pushd "$ROOT/runtiva-pdf"
    PDFIUM_DIR="$ROOT/runtiva-pdf/pdfium"
    if [ ! -d $PDFIUM_DIR ]; then
        mkdir -p pdfium
        gclient config --unmanaged https://pdfium.googlesource.com/pdfium.git 
        gclient sync
        cd pdfium
        #git checkout $PDFIUM_BRANCH
        cd ..

        echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections
        sudo apt-get update && apt-get -y install tzdata

    fi
    popd
}

install_pdfium

exit 0



RUN mkdir -p /build
WORKDIR /build
RUN gclient config --unmanaged https://pdfium.googlesource.com/pdfium.git 
RUN gclient sync
WORKDIR /build/pdfium
RUN git checkout $PDFIUM_BRANCH

## PDFium Build Dependencies
RUN echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections
RUN sudo DEBIAN_FRONTEND=noninteractive apt-get install tzdata

# Patch to remove snapcraft from dependencies as this fails in docker
COPY ./patches/install-build-deps.sh ./build/ 
RUN chmod +x ./build/install-build-deps.sh 
RUN echo n | ./build/install-build-deps.sh

#Apply Patches
COPY patches/pdfium.diff /build/pdfium/pdfium.diff
#RUN patch -i pdfium.diff -p1

COPY patches/build.diff /build/pdfium/build/build.diff
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




#RUN wget https://github.com/emscripten-core/emscripten/archive/2.0.12.tar.gz
#RUN tar zxf 2.0.12.tar.gz



