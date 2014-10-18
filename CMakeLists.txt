set(SRC Capture.h Capture.cc night_enhance.cc)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall ${CMAKE_CXX_FLAGS_RELEASE}")

add_executable(night_enhance ${SRC})
target_link_libraries(night_enhance 
opencv_highgui opencv_core opencv_ml opencv_features2d
opencv_imgproc opencv_legacy opencv_video opencv_flann
opencv_calib3d)
