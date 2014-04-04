cmake_minimum_required(VERSION 2.8)
set (BASE  "../../../")
set (CVBASE   "${BASE}opencv-master/cgi/")

message("CVBASE ${CVBASE}")

find_library(LIBOPENCV NAMES opencv-face HINTS "${CVBASE}" )

message("OPENCV ${LIBOPENCV}")

find_library(LIBCGI NAMES cgi HINTS  "${BASE}libcgi-master/src/")
message("LIBCGI ${LIBCGI}")
include_directories( "${BASE}libcgi-master/src/")

set_source_files_properties(face.c PROPERTIES LANGUAGE CXX)
add_executable(stalinize stalinize.c )
target_link_libraries(stalinize ${LIBCGI})
target_link_libraries(stalinize ${LIBOPENCV})
