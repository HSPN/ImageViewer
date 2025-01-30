cmake_minimum_required(VERSION 3.16)
include(ExternalProject)
project(ImageViewer)
find_package(Git REQUIRED)

#libjpeg-turbo 다운로드, 빌드
ExternalProject_Add(libjpeg-turbo
	GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
	GIT_TAG 3.1.0
	CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DENABLE_STATIC=ON -DENABLE_SHARED=OFF
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
	LOG_INSTALL ON 
)

#libjpeg-turbo에서 경로 변수 받기
ExternalProject_Get_Property(libjpeg-turbo install_dir)
include_directories(${install_dir}/include)
link_directories(${install_dir}/lib)

#add_subdirectory(zlib)
file(GLOB SOURCES "./*.cpp")
add_executable(ImageViewer WIN32 ${SOURCES} resource.rc)
target_precompile_headers(ImageViewer PRIVATE stdafx.h)

target_compile_definitions(ImageViewer PRIVATE UNICODE)
target_compile_definitions(ImageViewer PRIVATE _UNICODE)
# 프로젝트 B와 C의 라이브러리 링크

target_link_libraries(ImageViewer jpeg-static)
target_link_libraries(ImageViewer turbojpeg-static)

add_dependencies(ImageViewer libjpeg-turbo)