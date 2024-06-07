cmake_minimum_required(VERSION 3.12)

file( GLOB_RECURSE HEADERS LIST_DIRECTORIES false include/dlg/* )
file( GLOB_RECURSE SOURCES LIST_DIRECTORIES false src/* )

add_library( dlg ${HEADERS} ${SOURCES} )
target_include_directories( dlg PUBLIC include )
target_link_libraries( dlg PUBLIC gRPC::grpc++ protobuf::libprotobuf gain )
target_sources( dlg PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include FILES ${HEADERS} )

if( NOT DLG_SKIP_INSTALL )
	install( TARGETS dlg FILE_SET HEADERS DESTINATION include )
endif()

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/dlg PREFIX \\ FILES ${HEADERS} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX \\ FILES ${SOURCES} )
