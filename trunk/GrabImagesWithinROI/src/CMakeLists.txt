cmake_minimum_required(VERSION 2.8)

project( GrabImagesWithinROI )
FILE(GLOB SOURCE_FILES *.cpp)
add_executable( GrabImagesWithinROI ${SOURCE_FILES} )
include_directories( GrabImagesWithinROI ${GIWR_INCLUDES})

