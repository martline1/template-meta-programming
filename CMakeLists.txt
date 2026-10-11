cmake_minimum_required(VERSION 3.16)

project(eso_cpp LANGUAGES CXX)

file(GLOB_RECURSE ESO_CPP_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE ESO_CPP_HEADERS CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.tpp"
)

set(ESO_CPP_TEST_SOURCES ${ESO_CPP_SOURCES})
list(FILTER ESO_CPP_TEST_SOURCES INCLUDE REGEX "\\.test\\.cpp$")
set(ESO_CPP_APP_SOURCES ${ESO_CPP_SOURCES})
list(FILTER ESO_CPP_APP_SOURCES EXCLUDE REGEX "\\.test\\.cpp$")

add_executable(eso_cpp ${ESO_CPP_APP_SOURCES} ${ESO_CPP_HEADERS})
target_include_directories(eso_cpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(eso_cpp PRIVATE cxx_std_20)

if(ESO_CPP_TEST_SOURCES)
	enable_testing()
	find_package(GTest REQUIRED)

	set(ESO_CPP_TEST_SUPPORT_SOURCES ${ESO_CPP_APP_SOURCES})
	list(FILTER ESO_CPP_TEST_SUPPORT_SOURCES EXCLUDE REGEX "/main\\.cpp$")

	add_executable(eso_cpp_tests ${ESO_CPP_TEST_SOURCES} ${ESO_CPP_TEST_SUPPORT_SOURCES} ${ESO_CPP_HEADERS})
	target_include_directories(eso_cpp_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
	target_compile_features(eso_cpp_tests PRIVATE cxx_std_20)
	target_link_libraries(eso_cpp_tests PRIVATE GTest::gtest_main)

	add_test(NAME eso_cpp_tests COMMAND eso_cpp_tests)
endif()

add_custom_target(run
	COMMAND $<TARGET_FILE:eso_cpp>
	DEPENDS eso_cpp
	USES_TERMINAL
)
