set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

add_executable(
  testLibFunq
  test.cpp
)
target_include_directories(
  testLibFunq PUBLIC
  "${CMAKE_SOURCE_DIR}/libFunq"
)
target_compile_options(
  testLibFunq PUBLIC
  "-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}/tests/libFunq/\""
)
target_link_libraries(
  testLibFunq PUBLIC
  Funq Qt5::Core Qt5::Gui Qt5::Widgets
)
if(Qt5Quick_FOUND)
  target_link_libraries(testLibFunq PUBLIC Qt5::Quick)
endif()
