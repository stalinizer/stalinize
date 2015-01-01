cmake_minimum_required(VERSION 2.8)
#set (BASE  "../../")
#set (CVBASE   "${BASE}opencv-master/cgi/")
#message("CVBASE ${CVBASE}")
find_library(LIBOPENCV NAMES opencv-face HINTS "~/experiments/stalinizer/opencv/debian/libcv-dev/")
find_package( ZLIB REQUIRED )
find_package (Threads)
FIND_LIBRARY(LIBDL_LIBRARIES NAMES dl libdl )
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

message("OPENCV ${LIBOPENCV}")
include_directories( "/usr/include/")
include_directories( "/usr/include/opencv2/")
include_directories( "/usr/include/opencv2/objdetect/")
include_directories( "/usr/include/opencv2/core/")
include_directories( "/usr/include/opencv2/imgproc/")

include_directories(${PTHREAD_INCLUDE_DIR})


set_source_files_properties(stalinize.c PROPERTIES LANGUAGE CXX)
add_executable(stalinize.cgi stalinize.c )

#####################

#target_link_libraries(stalinize.cgi ${LIBCGI})
target_link_libraries(stalinize.cgi ${LIBOPENCV})
target_link_libraries(stalinize.cgi ${OPENCV_LINKER_LIBS} ${OPENCV_C_SAMPLES_REQUIRED_DEPS})
target_link_libraries(stalinize.cgi ${ZLIB_LIBRARIES} )
target_link_libraries(stalinize.cgi ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(stalinize.cgi ${PTHREAD_LIBRARY})
target_link_libraries(stalinize.cgi ${PNG_LIBRARY})
target_link_libraries(stalinize.cgi ${JPEG_LIBRARY})
#target_link_libraries(stalinize.cgi opencv-face)


######
set_source_files_properties(compose.c PROPERTIES LANGUAGE CXX)
add_executable(composetest compose.c )

#####################

#target_link_libraries(composetest ${LIBCGI})
target_link_libraries(composetest ${LIBOPENCV})
target_link_libraries(composetest ${OPENCV_LINKER_LIBS} ${OPENCV_C_SAMPLES_REQUIRED_DEPS})
target_link_libraries(composetest ${ZLIB_LIBRARIES} )
target_link_libraries(composetest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(composetest ${PTHREAD_LIBRARY})
target_link_libraries(composetest ${PNG_LIBRARY})
target_link_libraries(composetest ${JPEG_LIBRARY})
#target_link_libraries(composetest opencv-face)


