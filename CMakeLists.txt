cmake_minimum_required(VERSION 3.16)
include(ExternalProject)
project(ImageViewer)
find_package(Git REQUIRED)

#libjpeg-turbo 다운로드, 빌드
ExternalProject_Add(libjpeg-turbo
	GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
	GIT_TAG 3.1.0
	CMAKE_ARGS	-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DENABLE_STATIC=ON -DENABLE_SHARED=OFF -DUNICODE=ON -D_UNICODE=ON
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
	LOG_INSTALL ON 
)

#libjpeg-turbo에서 경로 변수 받아 설정
ExternalProject_Get_Property(libjpeg-turbo install_dir)
include_directories(${install_dir}/include)
link_directories(${install_dir}/lib)

#ImageViewer 컴파일 설정
file(GLOB SOURCES "./*.cpp")
#resource.rc를 꼭 추가해야함
add_executable(ImageViewer WIN32 ${SOURCES} resource.rc)
#precompile 설정
target_precompile_headers(ImageViewer PRIVATE stdafx.h)

#유니코드
target_compile_definitions(ImageViewer PRIVATE UNICODE)
target_compile_definitions(ImageViewer PRIVATE _UNICODE)

#정적링크
target_link_libraries(ImageViewer jpeg-static turbojpeg-static)

#종속성 (안하면 빌드순서 이상해짐)
add_dependencies(ImageViewer libjpeg-turbo)