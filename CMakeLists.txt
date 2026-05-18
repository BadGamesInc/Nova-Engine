cmake_minimum_required(VERSION 3.20)
project(Nova_Engine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -- GLFW -----------------------------------------------------------
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# -- ASSIMP (Added via git clone/submodule) -------------------------
# Set these BEFORE add_subdirectory to keep your build clean
set(ASSIMP_BUILD_ZLIB         ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS         OFF CACHE BOOL "" FORCE)

# Assuming you cloned it into external/assimp
add_subdirectory(external/assimp)

# -- GLAD -----------------------------------------------------------
add_library(glad STATIC external/src/gl.c)
target_include_directories(glad PUBLIC
        external/include/glad
        include
)

# -- Your executable ------------------------------------------------
add_executable(${PROJECT_NAME}
        src/main.c
        src/util/novalogger.c
        src/util/config.c
        src/util/fileutils.c
        src/render/renderer.c
        src/render/shader.c
        src/render/model/model.c
        src/render/model/resourceloader.c
        include/render/model/resourceloader.h
        src/engine.c
        include/engine.h
        src/window.c
        include/window.h
        include/render/texture.h
        src/render/texture.c
        include/util/mathutils.h
        src/util/mathutils.c
        include/entity/entity.h
        src/entity/entity.c
        include/entity/camera.h
        src/entity/camera.c
        # Header files usually don't need to be in add_executable,
        # but it doesn't hurt for IDE visibility.
)

# Use target_include_directories to tell your app where Assimp headers are
target_include_directories(${PROJECT_NAME} PRIVATE
        external/assimp/include
        external/assimp/build/include # Assimp generates some headers here
        external/assimp/contrib/unzip
        external/include/stb
        external/include/cglm
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glad
        glfw
        assimp  # <--- Link the library here
)

if(NOT WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()
