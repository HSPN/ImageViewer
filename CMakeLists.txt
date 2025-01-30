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

#zlib 다운로드, 빌드
ExternalProject_Add(zlib
	GIT_REPOSITORY https://github.com/madler/zlib.git
	GIT_TAG v1.3.1
	CMAKE_ARGS	-DCMAKE_INSTALL_PREFIX=${install_dir} -DENABLE_STATIC=ON -DENABLE_SHARED=OFF -DUNICODE=ON -D_UNICODE=ON
	INSTALL_COMMAND "" #install 막음. 프로젝트 내부에 파일을 두기 위한 것
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
	LOG_INSTALL ON 
)

#libpng에서 사용하기 위해 ZLIB_LIBRARY, ZLIB_INCLUDE_DIR 설정
set(ZLIB_PATH  ${PROJECT_BINARY_DIR}/zlib-prefix)
set(ZLIB_LIBRARY_DIR ${ZLIB_PATH}/lib)
set(ZLIB_INCLUDE_DIR ${ZLIB_PATH}/include)
make_directory(${ZLIB_INCLUDE_DIR})
make_directory(${ZLIB_LIBRARY_DIR})
include_directories(${ZLIB_INCLUDE_DIR})

file(GLOB ZLIB_HEADER ${ZLIB_PATH}/src/zlib-build/*.h ${ZLIB_PATH}/src/zlib/*.h)
file(GLOB ZLIB_LIBRARY ${ZLIB_PATH}/src/zlib-build/*.lib)
file(COPY ${ZLIB_HEADER} DESTINATION ${ZLIB_INCLUDE_DIR})
file(COPY ${ZLIB_LIBRARY} DESTINATION ${ZLIB_LIBRARY_DIR})

#libpng 빌드
ExternalProject_Add(libpng
	GIT_REPOSITORY https://github.com/libpng/libpng.git
	GIT_TAG v1.4.16
	CMAKE_ARGS     -DENABLE_STATIC=ON -DENABLE_SHARED=OFF -DUNICODE=ON -D_UNICODE=ON -DZLIB_LIBRARY=${ZLIB_LIBRARY} -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
	INSTALL_COMMAND "" #install 막음. 프로젝트 내부에 파일을 두기 위한 것
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
	LOG_INSTALL ON 
)

set(LIBPNG_PATH  ${PROJECT_BINARY_DIR}/libpng-prefix)
set(LIBPNG_LIBRARY_DIR ${LIBPNG_PATH}/lib)
set(LIBPNG_INCLUDE_DIR ${LIBPNG_PATH}/include)
make_directory(${LIBPNG_INCLUDE_DIR})
make_directory(${LIBPNG_LIBRARY_DIR})
include_directories(${LIBPNG_INCLUDE_DIR})

file(GLOB LIBPNG_HEADER ${LIBPNG_PATH}/src/libpng/*.h)
file(GLOB LIBPNG_LIBRARY ${LIBPNG_PATH}/src/libpng-build/*.lib)
file(COPY ${LIBPNG_HEADER} DESTINATION ${LIBPNG_INCLUDE_DIR})
file(COPY ${LIBPNG_LIBRARY} DESTINATION ${LIBPNG_LIBRARY_DIR})

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
target_link_libraries(ImageViewer ${LIBPNG_LIBRARY})
target_link_libraries(ImageViewer jpeg-static turbojpeg-static)

#종속성 (안하면 빌드순서 이상해짐)
add_dependencies(libpng zlib)
add_dependencies(ImageViewer libjpeg-turbo)
add_dependencies(ImageViewer libpng)