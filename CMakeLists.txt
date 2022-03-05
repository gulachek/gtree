cmake_minimum_required(VERSION 3.8)

project(gtree)

find_package(Boost REQUIRED
	system
	iostreams
	fiber
	unit_test_framework
	)

set(core "gulachek_gtree")

add_library(${core}
	src/base128.cpp
	src/twos_complement.cpp
	src/fd.cpp
	src/cause.cpp
	src/flat_tree.cpp
	src/translate.cpp
	)

target_include_directories(${core} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
	$<INSTALL_INTERFACE:include>
	)

target_compile_features(${core}
	PUBLIC cxx_std_20
	)

target_link_libraries(${core}
	PUBLIC Boost::boost # for fusion encoding - consider moving
	PRIVATE Boost::iostreams
	PRIVATE Boost::fiber
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

add_executable(nums example/nums.cpp)
target_link_libraries(nums ${core})

#############################################
# Installation
#############################################

set(export_name gulachek_gtree-config)
set(export_targets_name gulachek_gtree_targets)
set(export_config_dir "lib/cmake/${export_name}")

# Install library binaries
install(TARGETS ${core}
	EXPORT ${export_targets_name}
	)

install(TARGETS
	gtree2hex
	)

install(DIRECTORY include/gulachek/
	DESTINATION include/gulachek
	)

# Install cmake package
install(EXPORT ${export_targets_name} 
	DESTINATION ${export_config_dir}
	)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	  "${CMAKE_CURRENT_BINARY_DIR}/${export_name}.cmake"
		  INSTALL_DESTINATION ${export_config_dir}
			)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${export_name}.cmake"
	DESTINATION ${export_config_dir}
	)
