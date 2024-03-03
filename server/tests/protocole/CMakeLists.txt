set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

add_executable(
  testProtocole
  test.cpp
)
target_include_directories(
  testProtocole PUBLIC
  "${CMAKE_SOURCE_DIR}/libFunq"
)
target_link_libraries(
  testProtocole PUBLIC
  Funq Qt5::Core
)
