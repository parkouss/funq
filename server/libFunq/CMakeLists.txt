set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

add_library(
  Funq SHARED
  delayedresponse.cpp
  delayedresponse.h
  dragndropresponse.cpp
  dragndropresponse.h
  funq.cpp
  funq.h
  json.cpp
  json.h
  jsonclient.cpp
  jsonclient.h
  objectpath.cpp
  objectpath.h
  pick.cpp
  pick.h
  player.cpp
  protocole.cpp
  protocole.h
  shortcutresponse.cpp
  shortcutresponse.h
)
if(WIN32)
  target_sources(Funq PRIVATE WindowsInjector.cpp WindowsInjector.h)
else()
  target_sources(Funq PRIVATE ldPreloadInjector.cpp)
endif()
target_link_libraries(
  Funq PUBLIC
  ${QT}::Core
  ${QT}::Gui
  ${QT}::Network
  ${QT}::Widgets
  ${QT}::Test
  $<$<BOOL:${WITH_QTQUICK}>:${QT}::Quick>
)
