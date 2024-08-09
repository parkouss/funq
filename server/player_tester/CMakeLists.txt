set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

add_executable(
  player_tester
  WIN32
  fenPrincipale.cpp
  fenPrincipale.h
  fenPrincipale.ui
  main.cpp
)
target_link_libraries(
  player_tester PUBLIC
  ${QT}::Core
  ${QT}::Gui
  ${QT}::Widgets
)
