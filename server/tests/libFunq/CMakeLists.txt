set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

add_executable(
  testLibFunq
  WIN32
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
  FunqStatic
  ${QT}::Core
  ${QT}::Gui
  ${QT}::Widgets
  $<$<BOOL:${WITH_QTQUICK}>:${QT}::Quick>
)
