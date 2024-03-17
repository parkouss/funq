set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

set(
  FUNQ_SOURCES
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
  list(APPEND FUNQ_SOURCES WindowsInjector.cpp WindowsInjector.h)
else()
  list(APPEND FUNQ_SOURCES ldPreloadInjector.cpp)
endif()

set(
  FUNQ_DEPENDENCIES
  ${QT}::Core
  ${QT}::Gui
  ${QT}::Network
  ${QT}::Widgets
  ${QT}::Test
  $<$<BOOL:${WITH_QTQUICK}>:${QT}::Quick>
)

add_library(FunqStatic STATIC ${FUNQ_SOURCES})
target_link_libraries(FunqStatic PUBLIC ${FUNQ_DEPENDENCIES})

add_library(Funq SHARED ${FUNQ_SOURCES})
target_link_libraries(Funq PUBLIC ${FUNQ_DEPENDENCIES})
