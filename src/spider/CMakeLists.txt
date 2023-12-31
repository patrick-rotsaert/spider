#
# Copyright (C) 2023 Patrick Rotsaert
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.19)

# FIXME: improve this
if(TARGET Boost::system AND TARGET Boost::url AND TARGET Boost::filesystem AND TARGET Boost::json AND TARGET Boost::date_time)
	message("Boost targets already defined")
else()
	# Boost.URL was added in version 1.81
	find_package(Boost 1.81 REQUIRED COMPONENTS system url filesystem json date_time)
endif()

include(FetchContent)

# {fmtlib}
# If this project is included as source, the fmt::fmt target may already be defined.
if(NOT TARGET fmt::fmt)
	# Prefer the system package
	find_package(fmt QUIET)
	if(NOT fmt_FOUND)
		set(FMT_INSTALL "${${PROJECT_NAME_UC}_INSTALL}")
		FetchContent_Declare(fmt
		  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
		  GIT_TAG 10.0.0
		)
		FetchContent_MakeAvailable(fmt)
	endif()
endif()

# spdlog
if(${PROJECT_NAME_UC}_USE_SPDLOG)
	# If this project is included as source, the spdlog::spdlog target may already be defined.
	if(NOT TARGET spdlog::spdlog)
		# Prefer the system package
		find_package(spdlog QUIET)
		if(NOT spdlog_FOUND)
			set(SPDLOG_FMT_EXTERNAL ON)
			set(SPDLOG_INSTALL "${${PROJECT_NAME_UC}_INSTALL}")
			FetchContent_Declare(spdlog
			  GIT_REPOSITORY https://github.com/gabime/spdlog.git
			  GIT_TAG v1.12.0
			)
			FetchContent_MakeAvailable(spdlog)
		endif()
	endif()
endif()


add_project_library(${PROJECT_NAME}
	SOURCES
		listener.cpp
		http_session.cpp
		http_session.h
		irequest_handler.cpp
		request_router.cpp
		json_response.cpp
		file_response.cpp
		error_response.cpp
		empty_response.cpp
		tracked_file.cpp
		tracked_file.h
		ifile_event_listener.cpp
		noop_file_event_listener.cpp
		controller.cpp
		ilogger.cpp
		logging.cpp
		formatters.h
		conversions.cpp
		log_response.cpp
		exception.cpp
		exception.h
		demangle.cpp
		demangle.h
		demangled_type_name.h
		tag_invoke/time_point.cpp
		tag_invoke/date.cpp
		tag_invoke/time_duration.cpp
		version.cpp
		version.h.in
		config.h.in
	PUBLIC_HEADERS
		api.h
		aliases.h
		message.h
		listener.h
		irequest_handler.h
		request_router.h
		json_response.h
		file_response.h
		error_response.h
		empty_response.h
		response_wrapper.h
		ifile_event_listener.h
		noop_file_event_listener.h
		dispatcher.hpp
		controller.hpp
		conversions.h
		ilogger.h
		logging.h
		log_level.h
		log_response.h
		tag_invoke/time_point.h
		tag_invoke/date.h
		tag_invoke/time_duration.h

	PUBLIC_LIBRARIES
		Boost::system
		Boost::url
		Boost::filesystem
		Boost::json
		Boost::date_time
		fmt::fmt

	UNIT_TEST_SOURCES
		test/unit/test_conversions.cpp

	PUBLIC_INCLUDE_DIRS
		${CMAKE_CURRENT_BINARY_DIR}/.. # for configured headers, see below
)

if(${PROJECT_NAME_UC}_TEST)
	set(${PROJECT_NAME}_TARGET ${PROJECT_NAME}_objects)
else()
	set(${PROJECT_NAME}_TARGET ${PROJECT_NAME})
endif()

if(TARGET spdlog::spdlog)
	target_sources(${${PROJECT_NAME}_TARGET} PRIVATE spdlog_logger.cpp spdlog_logger.h)
	target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
	if(${PROJECT_NAME_UC}_INSTALL)
		install_project_header(spdlog_logger.h ${PROJECT_SRC_DIR})
	endif()
endif()

#
# Build time logging levels

# Values are as in log_level.h
set(LOGGING_LEVELS trace debug info warn err critical off)

if(${PROJECT_NAME_UC}_DEBUG_BUILD)
	set(DEFAULT_LOGGING_LEVEL trace)
else()
	set(DEFAULT_LOGGING_LEVEL warn)
endif()

# Logging verbosity of the library
set(${PROJECT_NAME_UC}_LOGGING_LEVEL ${DEFAULT_LOGGING_LEVEL} CACHE STRING "Logging verbosity of the ${PROJECT_NAME} library")
set_property(CACHE ${PROJECT_NAME_UC}_LOGGING_LEVEL PROPERTY STRINGS ${LOGGING_LEVELS})

configure_file(
	version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/version.h
)

configure_file(
	config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
)

if(${PROJECT_NAME_UC}_INSTALL)
	install_project_header(${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_BINARY_DIR}/..)
	install_project_header(${CMAKE_CURRENT_BINARY_DIR}/config.h ${CMAKE_CURRENT_BINARY_DIR}/..)
endif()


##
#add_executable(json_test json_test.cpp)
#target_link_libraries(json_test Boost::json fmt::fmt)
#target_compile_features(json_test PRIVATE cxx_std_20)
#target_compile_options(json_test PRIVATE
#	"$<$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>:$<BUILD_INTERFACE:-Wall;-Wextra;-pedantic;-Werror>>"
#	"$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:$<BUILD_INTERFACE:/W4;/WX>>"
#)
