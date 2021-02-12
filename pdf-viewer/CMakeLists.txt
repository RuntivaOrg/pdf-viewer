#===============================================================================
# RUNTIVA_PDF Viewer Solution - Client Viewer App
#===============================================================================
cmake_minimum_required(VERSION 3.17)

set(projname "pdf-viewer")
project(${projname})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_executable(${projname}
    src/main.cpp
    src/application.cpp
    src/color.cpp
    src/deja_vu_sans_mono.cpp
    src/emscripten.cpp
    src/main_viewer_window.cpp
    src/layout.cpp
    src/layoutable.cpp
    src/painter.cpp
    src/to_utf16.cpp
    src/widget.cpp
    
    src/events/key_event.cpp
    src/events/text_input_event.cpp

    src/viewer/document.cpp
	src/viewer/geometry.cpp
	src/viewer/panel_layout.cpp
	src/viewer/page.cpp
	src/viewer/panel.cpp
	src/viewer/params.cpp
	src/viewer/scroll.cpp
	src/viewer/slot.cpp
	src/viewer/view.cpp
	src/viewer/zoom.cpp
    )

target_include_directories(${projname} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# TODO: CLEAN this up to be included in project
target_include_directories(${projname} PRIVATE C:\\Runtiva\\temp\\pdfium_wasm_trial\\pdfium)


## SDL2
# SDL2 -- Download and unzip on Windows. Use installed SDL2 on Linux. For WASM/Emscripten, the SDL2 port is used. 
if (WIN32)
    SET(SDL2_ZIP_FILE "${CMAKE_SOURCE_DIR}/third_party/SDL2/SDL2-devel-2.0.12-VC.zip")
    if(NOT EXISTS ${SDL2_ZIP_FILE})
        message("Downloading SDL2...")
        file(DOWNLOAD "https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip" ${SDL2_ZIP_FILE})
        file(ARCHIVE_EXTRACT INPUT ${SDL2_ZIP_FILE} DESTINATION "${CMAKE_SOURCE_DIR}/third_party/SDL2")
    endif()
    set(SDL2_DIR ${CMAKE_SOURCE_DIR}/third_party/SDL2/SDL2-2.0.12 CACHE PATH "SDL2 directory")

elseif(UNIX)

endif()

## SDL2_TTF
# SDL2_TTF -- Download and unzip on Windows. Use installed SDL2_TTF on Linux. For WASM/Emscripten, the SDL2 port is used.
if (WIN32)
    SET(SDL2_TTF_ZIP_FILE "${CMAKE_SOURCE_DIR}/third_party/SDL2_ttf/SDL2_ttf-devel-2.0.15-VC.zip")
    if(NOT EXISTS ${SDL2_TTF_ZIP_FILE})
        message("Downloading SDL2_TTF...")
        file(DOWNLOAD "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-VC.zip" ${SDL2_TTF_ZIP_FILE})
        file(ARCHIVE_EXTRACT INPUT ${SDL2_TTF_ZIP_FILE} DESTINATION "${CMAKE_SOURCE_DIR}/third_party/SDL2_TTF")
    endif()
    set(SDL2TTF_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL2_TTF/SDL2_ttf-2.0.15" CACHE PATH "SDL2TTL directory")
endif()

## SDL2_Image
# SDL2_Image -- Download and unzip on Windows. Use installed SDL2_Image on Linux. For WASM/Emscripten, the SDL2 port is used.
if (WIN32)
    SET(SDL2_IMAGE_ZIP_FILE "${CMAKE_SOURCE_DIR}/third_party/SDL2_image/SDL2_image-devel-2.0.5-VC.zip")
    if(NOT EXISTS ${SDL2_IMAGE_ZIP_FILE})
        message("Downloading SDL2_Image...")
        file(DOWNLOAD "https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip" ${SDL2_IMAGE_ZIP_FILE})
        file(ARCHIVE_EXTRACT INPUT ${SDL2_IMAGE_ZIP_FILE} DESTINATION "${CMAKE_SOURCE_DIR}/third_party/SDL2_image")
    endif()
    set(SDL2IMAGEDIR "${CMAKE_SOURCE_DIR}/third_party/SDL2_image/SDL2_image-2.0.5" CACHE PATH "SDL2IMAGE directory")
endif()


#  TODO: Change WASM flag
if (BUILD_WASM)
    message("Building for WASM")

    # TODO -- ADD -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="[""png""]"
    # ,\"\"xpm\"\"
	SET (EM_FLAGS
	"-s WASM=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=\"[\"\"png\"\",\"\"pnm\"\"]\" -s ALLOW_MEMORY_GROWTH=1 
	-s FORCE_FILESYSTEM=0 -s MODULARIZE=0 -s NO_EXIT_RUNTIME=1 -s INITIAL_MEMORY=128MB -s WARN_UNALIGNED=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 
	-s FETCH=1 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=2 -s EXPORTED_FUNCTIONS=\"[\"\"_main\"\", \"\"_resize_viewer\"\"]\" --use-preload-plugins --preload-file airplane.pdf.0.ppm") #lena.png") #airplane.pdf.0.ppm") 

	#SET (EM_COMPILE_LINK_PARAMS "-lEGL -lGLESv2")
	SET (EM_DEFINITIONS "-DSK_BUILD_FOR_WASM -DSK_SUPPORT_GPU=1 -DSK_GL -DSK_DISABLE_LEGACY_SHADERCONTEXT -DBUILD_WASM")
	#SET (EM_COMPILE_FLAGS "-O3 -DSK_RELEASE")
	#SET (EM_COMPILE_FLAGS "-O0 -g4 -DSK_DEBUG --source-map-base /")

    #target_compile_definitions(${projname} PRIVATE "BUILD_WASM")

	add_definitions(${EM_FLAGS} ${EM_DEFINITIONS} ${EM_COMPILE_FLAGS})

	set_target_properties(${projname} 
		PROPERTIES SUFFIX ".html"    
		LINK_FLAGS "${EM_FLAGS} ${EM_COMPILE_LINK_PARAMS} ${EM_DEFINITIONS} ${EM_COMPILE_FLAGS}")
	  #em_link_js_library(${SDL_PROJ} ${libraryJsFiles})

elseif (UNIX)
    find_package(SDL2_image REQUIRED)
    target_include_directories(${projname} SYSTEM PRIVATE "${SDL2_IMAGE_INCLUDE_DIRS}")
    target_link_libraries(${projname} PRIVATE "${SDL2_IMAGE_LIBRARIES}")

    find_package(SDL2 REQUIRED)
	target_include_directories(${projname} SYSTEM PRIVATE "${SDL2_INCLUDE_DIRS}")
	target_link_libraries(${projname} PRIVATE "${SDL2_LIBRARIES}")

	find_package(SDL2TTF REQUIRED)
	target_include_directories(${projname} SYSTEM PRIVATE "${SDL2TTF_INCLUDE_DIR}")
	target_link_libraries(${projname} PRIVATE "${SDL2TTF_LIBRARY}")

elseif (WIN32)
    find_package(SDL2_image REQUIRED)
    target_include_directories(${projname} PRIVATE "${SDL2_IMAGE_INCLUDE_DIRS}")
    target_link_libraries(${projname} PRIVATE "${SDL2_IMAGE_LIBRARIES}")

    find_package(SDL2 REQUIRED)
	target_include_directories(${projname} PRIVATE "${SDL2_INCLUDE_DIRS}")
	target_link_libraries(${projname} PRIVATE "${SDL2_LIBRARIES}")

	find_package(SDL2TTF REQUIRED)
	target_include_directories(${projname} PRIVATE "${SDL2TTF_INCLUDE_DIR}")
	target_link_libraries(${projname} PRIVATE "${SDL2TTF_LIBRARY}")

    # Copy SDL2 DLL to target directory 
    configure_file("${SDL2_DIR}/lib/x64/SDL2.dll" "${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll" COPYONLY)

    # Copy SDL2 TTF DLL to target directory 
    configure_file("${SDL2TTF_DIR}/lib/x64/SDL2_ttf.dll" "${CMAKE_CURRENT_BINARY_DIR}/SDL2_ttf.dll" COPYONLY)
    configure_file("${SDL2TTF_DIR}/lib/x64/libfreetype-6.dll" "${CMAKE_CURRENT_BINARY_DIR}/libfreetype-6.dll" COPYONLY)
    configure_file("${SDL2TTF_DIR}/lib/x64/zlib1.dll" "${CMAKE_CURRENT_BINARY_DIR}/zlib1.dll" COPYONLY)

    # Copy SDL2 Image DLLs to target directory 
    configure_file("${SDL2IMAGEDIR}/lib/x64/SDL2_image.dll" "${CMAKE_CURRENT_BINARY_DIR}/SDL2_image.dll" COPYONLY)
    configure_file("${SDL2IMAGEDIR}/lib/x64/libjpeg-9.dll" "${CMAKE_CURRENT_BINARY_DIR}/libjpeg-9.dll" COPYONLY)
    configure_file("${SDL2IMAGEDIR}/lib/x64/libpng16-16.dll" "${CMAKE_CURRENT_BINARY_DIR}/libpng16-16.dll" COPYONLY)
    configure_file("${SDL2IMAGEDIR}/lib/x64/libtiff-5.dll" "${CMAKE_CURRENT_BINARY_DIR}/libtiff-5.dll" COPYONLY)
    configure_file("${SDL2IMAGEDIR}/lib/x64/libwebp-7.dll" "${CMAKE_CURRENT_BINARY_DIR}/libwebp-7.dll" COPYONLY)
    configure_file("${SDL2IMAGEDIR}/lib/x64/zlib1.dll" "${CMAKE_CURRENT_BINARY_DIR}/zlib1.dll" COPYONLY)
endif()



