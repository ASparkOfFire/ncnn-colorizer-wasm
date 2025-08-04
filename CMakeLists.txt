cmake_minimum_required(VERSION 3.14)
project(colorize-ncnn-wasm)
set(CMAKE_CXX_STANDARD 11)

# Set the OpenCV and ncnn directories
set(OpenCV_DIR "/Users/asparkoffire/.bin/opencv-wasm/lib/cmake/opencv4")
set(ncnn_DIR "/Users/asparkoffire/.bin/ncnn-wasm/lib/cmake/ncnn")

# Find packages
find_package(OpenCV REQUIRED)
#find_package(ncnn REQUIRED)

# Add include directories
include_directories(${OpenCV_INCLUDE_DIRS} "/Users/asparkoffire/.bin/ncnn-wasm/include")
include_directories(${CMAKE_SOURCE_DIR}/src)

# Automatically collect source and header files - FIXED PATTERNS
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
)

# Debug: Print found source files
message(STATUS "Source files found: ${SRC_FILES}")
list(LENGTH SRC_FILES SRC_COUNT)
message(STATUS "Number of source files: ${SRC_COUNT}")

# Check if we found any source files
if(NOT SRC_FILES)
    message(FATAL_ERROR "No source files found! Check your src directory structure.")
endif()

file(GLOB_RECURSE HEADER_FILES
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/**/*.h"
)

# Create executable
add_executable(lib ${SRC_FILES})

# Link necessary libraries
target_link_libraries(lib
    /Users/asparkoffire/.bin/ncnn-wasm/lib/libncnn.a
    opencv_core opencv_imgproc opencv_imgcodecs
)

# Compile-time options (only threading-related flags for compilation)
# Compile-time options
target_compile_options(lib PRIVATE
    # Flags for WebAssembly compatibility
    -fexceptions
    -frtti
    -fno-asynchronous-unwind-tables
    -msimd128
    -pthread
    -matomics
    -mbulk-memory
)

# Build the preload file path properly
set(MODEL_PRELOAD_PATH "--preload-file")
set(MODEL_PRELOAD_PATH "${MODEL_PRELOAD_PATH} ${CMAKE_SOURCE_DIR}/model@/model")

# Build from the project root so relative paths work
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Link-time Emscripten flags
set(EM_FLAGS
    -sNO_DISABLE_EXCEPTION_CATCHING=0
    -sNO_DISABLE_EXCEPTION_CATCHING
    -sALLOW_MEMORY_GROWTH=1
    -sUSE_PTHREADS=0
    -sPTHREAD_POOL_SIZE=10
    -sMODULARIZE=1
    -sEXPORT_NAME=Colornet
    -sWASM_MEM_MAX=2147483648
    -sEXPORTED_FUNCTIONS=['_main','_colorize_rgba','_colorize','_malloc','_free']
    -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','HEAPU8','FS','UTF8ToString','lengthBytesUTF8']
    -pthread
    -matomics
    -mbulk-memory
    -msimd128
)

target_link_options(lib PRIVATE ${EM_FLAGS})

# Add the preload file flag using the old-style LINK_FLAGS for better compatibility
get_target_property(CURRENT_LINK_FLAGS lib LINK_FLAGS)
if(NOT CURRENT_LINK_FLAGS)
    set(CURRENT_LINK_FLAGS "")
endif()
set_target_properties(lib PROPERTIES
    LINK_FLAGS "${CURRENT_LINK_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/model@/model"
)

# Define source and destination directories
set(SOURCE_DIR "${CMAKE_BINARY_DIR}")
set(DEST_DIR "${CMAKE_SOURCE_DIR}/dist")

# Create dist directory if it doesn't exist
file(MAKE_DIRECTORY ${DEST_DIR})

# Debug: Check if model files exist
if(EXISTS "${CMAKE_SOURCE_DIR}/model")
    message(STATUS "Model directory exists: ${CMAKE_SOURCE_DIR}/model")
    file(GLOB MODEL_FILES "${CMAKE_SOURCE_DIR}/model/*")
    message(STATUS "Model files: ${MODEL_FILES}")

    # Check individual files
    if(EXISTS "${CMAKE_SOURCE_DIR}/model/model.param")
        message(STATUS "model.param found")
    else()
        message(WARNING "model.param NOT found")
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/model/model.bin")
        message(STATUS "model.bin found")
    else()
        message(WARNING "model.bin NOT found")
    endif()
else()
    message(WARNING "Model directory does not exist: ${CMAKE_SOURCE_DIR}/model")
endif()

# Print the full link command for debugging
get_target_property(LINK_FLAGS lib LINK_FLAGS)
message(STATUS "Final link flags: ${LINK_FLAGS}")

# Custom command to copy necessary files after build
add_custom_command(
    TARGET lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_DIR}/lib.js ${DEST_DIR}/lib.js
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_DIR}/lib.wasm ${DEST_DIR}/lib.wasm
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/src/public/index.html ${DEST_DIR}/index.html
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/src/public/styles.css ${DEST_DIR}/styles.css
    # Copy lib.data only if it exists
    COMMAND ${CMAKE_COMMAND} -E echo "Checking for lib.data file..."
    COMMAND bash -c "if [ -f ${SOURCE_DIR}/lib.data ]; then ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_DIR}/lib.data ${DEST_DIR}/lib.data; echo 'lib.data copied'; else echo 'lib.data not found, skipping'; fi"
    COMMENT "Copying build artifacts to dist directory"
    VERBATIM
)
