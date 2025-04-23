cmake_minimum_required(VERSION 3.14)
project(colorize-ncnn-wasm)

set(CMAKE_CXX_STANDARD 11)

# Set the OpenCV and ncnn directories
set(OpenCV_DIR "/Users/asparkoffire/.bin/opencv-wasm/lib/cmake/opencv4")
set(ncnn_DIR "/Users/asparkoffire/.bin/ncnn-wasm/lib/cmake/ncnn")

# Find packages
find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)

# Add include directories
include_directories(${OpenCV_INCLUDE_DIRS} "/Users/asparkoffire/.bin/ncnn-wasm/include")

# Automatically collect source and header files
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/**/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/**/*.h")

# Create executable
add_executable(lib ${SRC_FILES} ${HEADER_FILES} src/main.h)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Link necessary libraries
target_link_libraries(lib opencv_core opencv_imgproc opencv_imgcodecs ncnn)

# Compiler and linker options
target_compile_options(lib PRIVATE -pthread -matomics -mbulk-memory)
target_link_options(lib PRIVATE -pthread -matomics -mbulk-memory)

# Emscripten specific flags
set(EMSCRIPTEN_FLAGS "
     -s NO_DISABLE_EXCEPTION_CATCHING \
     -s ALLOW_MEMORY_GROWTH=1 \
     -s USE_PTHREADS=1 \
     -s PTHREAD_POOL_SIZE=10 \
     -s MODULARIZE=1 \
     -s EXPORT_NAME='colornet' \
     -s EXPORTED_FUNCTIONS='[\"_main\", \"_colorize_rgba\", \"_colorize\", \"_malloc\", \"_free\"]' \
     -s WASM_MEM_MAX=2147483648 \
     -s EXPORT_ALL=1 \
     -s EXPORTED_RUNTIME_METHODS='[\"FS\", \"ccall\", \"cwrap\"]'"
)

# Apply Emscripten flags
set_target_properties(lib PROPERTIES LINK_FLAGS ${EMSCRIPTEN_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSCRIPTEN_FLAGS}")

# Define source and destination directories
set(SOURCE_DIR "${CMAKE_BINARY_DIR}")
set(DEST_DIR "${CMAKE_SOURCE_DIR}/dist")

# Create dist directory if it doesn't exist
file(MAKE_DIRECTORY ${DEST_DIR})

# Model files
set(MODEL_FILES "${CMAKE_SOURCE_DIR}/model/model.param" "${CMAKE_SOURCE_DIR}/model/model.bin")

# Preload model files
set_target_properties(lib PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/model@/model")

# Custom command to copy necessary files after build
add_custom_command(
        TARGET lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/lib.js ${DEST_DIR}/lib.js
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/lib.data ${DEST_DIR}/lib.data
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/public/index.html ${DEST_DIR}/index.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/public/styles.css ${DEST_DIR}/styles.css
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/lib.wasm ${DEST_DIR}/lib.wasm
)
