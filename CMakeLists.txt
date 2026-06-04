cmake_minimum_required(VERSION 3.15) #CMake surumunu 3.15 olarak ayarlar
project(bsp_viewer CXX C) #Proje ismi, c++ ve c dillerinin kullanilacagi belirtilir
set(CMAKE_CXX_STANDARD 17) #C++ dil srümünü 17 olarak ayarlar
set(CMAKE_CXX_STANDARD_REQUIRED ON)#C++ 17 standarti yoksa hata ver
set(EXTERN_DIR "${CMAKE_SOURCE_DIR}/extern")
find_package(OpenGL REQUIRED) #OpenGL kütüphanesini sistemde ara

# GLAD
set(GLAD_SRC "${EXTERN_DIR}/glad/src/glad.c")
set(GLAD_INC "${EXTERN_DIR}/glad/include")

# GLM
set(GLM_INC "${EXTERN_DIR}/glm")

# SDL2
set(SDL2_INC     "${EXTERN_DIR}/SDL2/include")
set(SDL2_LIB_DIR "${EXTERN_DIR}/SDL2/lib/x64")

add_executable(bsp_viewer
    src/main.cpp
    "${GLAD_SRC}"
)#Kaynak kod dosyalarini bu klasorlerde ara
target_include_directories(bsp_viewer PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${GLAD_INC}"
    "${GLM_INC}"
    "${SDL2_INC}"
)#include ile eklenecek dosyalari bu klasörlerde ara
target_link_libraries(bsp_viewer PRIVATE
    "${SDL2_LIB_DIR}/SDL2.lib"
    "${SDL2_LIB_DIR}/SDL2main.lib"
    OpenGL::GL
)# .lib dosyalarını ekle
target_compile_options(bsp_viewer PRIVATE /W3 /MP /utf-8)#MSVC icin derleme secenekleri
set_target_properties(bsp_viewer PROPERTIES WIN32_EXECUTABLE FALSE)#Konsol pencerisinin acilmasi icin secenek FALSE olarak ayarlanir
set_target_properties(bsp_viewer PROPERTIES WIN32_EXECUTABLE FALSE)
add_custom_command(TARGET bsp_viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_LIB_DIR}/SDL2.dll"
        "$<TARGET_FILE_DIR:bsp_viewer>/SDL2.dll"
    COMMENT "SDL2.dll kopyalaniyor..."
) #SDL2.dll dosyasini exe dosyasinin yanina kopyalar
