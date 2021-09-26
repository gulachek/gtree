cmake_minimum_required(VERSION 3.8)

project(gtree)

find_package(Boost REQUIRED
	system
	iostreams
	timer
	json
	unit_test_framework
	)

set(core "gulachek_gtree")

if (WIN32)
	set(platform_dir win)
elseif(UNIX)
	set(platform_dir posix)
endif()

add_library(${core}
	src/tree.cpp
	src/itreem.cpp
	src/fd_itreem.cpp
	src/dynamic_tree.cpp
	src/block.cpp
	src/gtree.cpp
	src/twos_complement.cpp
	src/error.cpp
	src/optimization_type.cpp
	)

target_include_directories(${core} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${platform_dir}/>
	$<INSTALL_INTERFACE:include>
	)

target_compile_features(${core}
	PUBLIC cxx_std_20
	)

target_link_libraries(${core}
	PUBLIC Boost::boost
	PRIVATE Boost::iostreams
	)

file(GLOB tests "test/*test.cpp")

add_library(testlib INTERFACE)

target_include_directories(testlib
	INTERFACE test/include
	)

target_link_libraries(testlib
	INTERFACE ${core}
	)

enable_testing()
foreach(a_test ${tests})
	get_filename_component(name ${a_test} NAME_WE)
	add_executable(${name} ${a_test})
	target_link_libraries(${name}
		PUBLIC ${core}
		PUBLIC testlib
		PUBLIC Boost::unit_test_framework
		)

	add_test(${name} ${name})
endforeach()

# Tools for testing
add_executable(gtree2hex tools/gtree2hex.cpp)
target_link_libraries(gtree2hex ${core})

add_executable(gnz2dec tools/gnz2dec.cpp)
target_link_libraries(gnz2dec ${core})

add_executable(nums example/nums.cpp)
target_link_libraries(nums ${core})

add_executable(string_or_uint example/string_or_uint.cpp)
target_link_libraries(string_or_uint ${core})

add_executable(write_nums example/write_nums.cpp)
target_link_libraries(write_nums ${core})

add_executable(custom_class example/custom_class.cpp)
target_link_libraries(custom_class ${core})

add_executable(vs_json test/vs_json.cpp)
target_link_libraries(vs_json ${core} Boost::timer Boost::json)

#############################################
# Installation
#############################################

set(export_name gulachek_gtree-config)

# Install library binaries
install(TARGETS ${core}
	EXPORT ${export_name}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)

install(TARGETS gtree2hex
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)

install(TARGETS gnz2dec
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)

install(DIRECTORY include/gulachek/
	DESTINATION include/gulachek
	)

install(DIRECTORY ${platform_dir}/gulachek/
	DESTINATION include/gulachek
	)

# Install cmake package
install(EXPORT ${export_name} 
	DESTINATION lib/cmake/${export_name}
	)
