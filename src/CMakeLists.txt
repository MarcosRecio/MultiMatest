find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${MaTest_SOURCE_DIR}/src/logics)
SET(SRC
    MaTest.c
    callbacks.c
    evaluation.c
    user.c
    user_gui.c
    user_tui.c)
SET(LIBSRC
    logics
    ${GTK3_LIBRARIES})
#SET(CMAKE_C_FLAGS "${GTK_CFLAGS} ${GTHREAD_CFLAGS}")
#STRING(REGEX REPLACE "-l" "" GTK_LIBRARIES "${GTK_LINK_FLAGS} ${GTHREAD_LINK_FLAGS}")
ADD_EXECUTABLE(matest ${SRC})
TARGET_LINK_LIBRARIES(matest ${LIBSRC})
add_subdirectory (logics)
