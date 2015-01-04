cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project("stalinizer")
#add_subdirectory(src/cgi)
#install(FILES src/cgi/composetest src/cgi/stalinize.cgi  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

set_source_files_properties(src/cgi/stalinize.c PROPERTIES LANGUAGE CXX)
add_executable(stalinize.cgi src/cgi/stalinize.c )
set_source_files_properties(src/cgi/compose.c PROPERTIES LANGUAGE CXX)
add_executable(composetest src/cgi/compose.c )

find_library(LIBOPENCV_CORE      NAMES opencv_core      PATH /usr/lib/) 
find_library(LIBOPENCV_IMGPROC   NAMES opencv_imgproc   PATH /usr/lib/) 
find_library(LIBOPENCV_HIGHGUI   NAMES opencv_highgui   PATH /usr/lib/) 
find_library(LIBOPENCV_ML        NAMES opencv_ml        PATH /usr/lib/) 
find_library(LIBOPENCV_OBJDETECT NAMES opencv_objdetect PATH /usr/lib/) 

find_package( ZLIB REQUIRED )
find_package (Threads)
FIND_LIBRARY(LIBDL_LIBRARIES NAMES dl libdl )
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

#message("OPENCV ${LIBOPENCV}")
include_directories( "/usr/include/")
include_directories( "/usr/include/opencv2/")
include_directories( "/usr/include/opencv2/objdetect/")
include_directories( "/usr/include/opencv2/core/")
include_directories( "/usr/include/opencv2/imgproc/")

include_directories(${PTHREAD_INCLUDE_DIR})




target_link_libraries(stalinize.cgi rt)
# message("OPENCV ${OPENCV_LINKER_LIBS} ${LIBOPENCV} ${LIBOPENCV_CORE}  ${LIBOPENCV_IMGPROC}  ${LIBOPENCV_HIGHGUI}  ${LIBOPENCV_ML}  ${LIBOPENCV_OBJDETECT}")
target_link_libraries(stalinize.cgi ${LIBOPENCV} ${OPENCV_LINKER_LIBS} ${LIBOPENCV_CORE} ${LIBOPENCV_IMGPROC}  ${LIBOPENCV_HIGHGUI}  ${LIBOPENCV_ML}  ${LIBOPENCV_OBJDETECT})
target_link_libraries(stalinize.cgi ${ZLIB_LIBRARIES} )
target_link_libraries(stalinize.cgi ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(stalinize.cgi ${PTHREAD_LIBRARY})
target_link_libraries(stalinize.cgi ${PNG_LIBRARY})
target_link_libraries(stalinize.cgi ${JPEG_LIBRARY})

######

target_link_libraries(composetest rt)
target_link_libraries(composetest ${LIBOPENCV} ${LIBOPENCV_CORE} ${LIBOPENCV_IMGPROC} ${LIBOPENCV_HIGHGUI} ${LIBOPENCV_ML}  ${LIBOPENCV_OBJDETECT}  ${OPENCV_LINKER_LIBS} )
target_link_libraries(composetest ${ZLIB_LIBRARIES} )
target_link_libraries(composetest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(composetest ${PTHREAD_LIBRARY})
target_link_libraries(composetest ${PNG_LIBRARY})
target_link_libraries(composetest ${JPEG_LIBRARY})


install (TARGETS composetest stalinize.cgi DESTINATION bin)
