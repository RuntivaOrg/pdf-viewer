#!/usr/bin/env bash
set -e

### Folder Structure for Runtiva PDF Development Environment:
## Ideally this should structure is located directly in your home directory so that the directory structure in Docker is identical
## pdf-sln  -- Root directory
##   - emsdk
##       contents installed from Emscripten GIT repo
##   - depot_tools
##       contents installed from Emscripten GIT repo
##
##   - runtiva-pdf  (This is the root of the runtiva-pdf repo. All source code, including 3rd party is under this directory)
##     - pdfium
##       -- pdfium code, thirdparty and chromium build repo are in here
##     - pdf-viewer
##       -- thirdparty (pdf-viewer specific thirdparty code)

EMSDK_VERSION=2.0.12
PDFIUM_BRANCH=82408da2c17d3c58a2de27ac46bb41e89da86991


#Validate that we are in the pdf-viewer OR pdf-viewer/tools directory
dir=$(pwd)
ROOT="$HOME/pdf-sln"
# todo: Calculate root instead of hardcode 
#if [[ "$dir" == *"$"* ]]

current_directory_name="$(basename $dir)"
if [ "$current_directory_name" = "pdf-viewer" ]; then
  pushd ./tools
fi

dir=$(pwd)
current_directory_name="$(basename $dir)"
if [ "$current_directory_name" != "tools" ]; then
  echo "This script must be run from within tools folder"
  exit 1
fi


echo "current_directory_name: $current_directory_name"


install_package_deps() {
    apt-get update \
        && apt-get install -y wget \
        && apt-get install -y python
    RUN et install -y build-essential \
        && apt-get install -y git

    apt-get update \
        && apt-get install -y cmake \
        && apt-get install -y xz-utils

    apt-get update \
        && apt-get install -y lsb-release \
        && apt-get install -y sudo 
}



## Install Emscripten SDK
install_emsdk() {
    pushd "$ROOT"
    EMSDK_DIR="$ROOT/emsdk"
    if [ ! -d $EMSDK_DIR ]; then
      git clone https://github.com/emscripten-core/emsdk.git \
        && cd emsdk \
        && git checkout $EMSDK_VERSION \
        && ./emsdk install $EMSDK_VERSION \
        && ./emsdk activate $EMSDK_VERSION \
        && echo "source \"${EMSDK_DIR}/emsdk_env.sh\"" >> $HOME/.bash_profile \
        && source "${EMSDK_DIR}/emsdk_env.sh" \
        && cd ..
    fi


echo "111111111111111111111"
    . ${EMSDK_DIR}/emsdk_env.sh
    popd
}

## Install Google Depot Tools
# The git config to set remote.origin.url is to remove a gclient prompt about pointint to an obsolete url
install_depot_tools() {
    pushd "$ROOT"
    DEPOT_TOOLS_DIR="$ROOT/depot_tools"
    NEW_PATH="$DEPOT_TOOLS_DIR"
    NEW_PATH="export PATH=\"$NEW_PATH:"
    if [ ! -d $DEPOT_TOOLS_DIR ]; then
        git clone https://chromium.googlesource.com/chromium/tools/depot_tools \
            && cd depot_tools \
            && git config remote.origin.url "https://chromium.googlesource.com/chromium/tools/depot_tools.git" \
            && echo $NEW_PATH'${PATH}"' >> $HOME/.bash_profile \
            && cd ..
        
            #&& export $NEW_PATH'${PATH}"' \
        
    fi
echo "XXXXXXXXXXXXXXXXXXXXXXX"
echo export PATH="$DEPOT_TOOLS_DIR:${PATH}"
    export PATH="$DEPOT_TOOLS_DIR:${PATH}"
    popd
}



# install_package_deps
install_emsdk
install_depot_tools


exit 0

## Install Google Depot Tools
# The git config to set remote.origin.url is to remove a gclient prompt about pointint to an obsolete url
RUN cd /opt \
    && git clone https://chromium.googlesource.com/chromium/tools/depot_tools \
    && cd depot_tools \
    && git config remote.origin.url "https://chromium.googlesource.com/chromium/tools/depot_tools.git" \
    && cd ..
ENV PATH="/opt/depot_tools:${PATH}"

## PDFium Source Code
# Note that we checkout a specific branch to ensure build consistency.
# we do not want to always be working off of the master branch
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



