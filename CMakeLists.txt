cmake_minimum_required(VERSION 3.5)

project(hello-dicom LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(dicomhero6 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello-dicom main.cpp)

target_link_libraries(hello-dicom dicomhero6)

include(GNUInstallDirs)
install(TARGETS hello-dicom
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
