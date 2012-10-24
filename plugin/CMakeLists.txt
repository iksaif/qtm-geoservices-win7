# Copyright (C) 2009-2010 Corentin Chary <corentin.chary@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

project(qtposition_win7)

include_directories (
  ${CMAKE_CURRENT_BUILD_DIR}
  ${QT_MOBILITY_INCLUDE_DIR}
  ${QT_MOBILITY_LOCATION_INCLUDE_DIR}
  ${QT_INCLUDE_DIR}
)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

if (STATIC_PLUGINS)
  add_definitions(-DQT_STATICPLUGIN)
  set(PLUGIN_TYPE STATIC)
else()
  add_definitions(-DQT_SHARED)
  set(PLUGIN_TYPE SHARED)
endif()


set (win7_SRCS
  qgeopositioninfosource_win7.cpp
  qgeopositioninfosourcefactory_win7.cpp
)

set (win7_HDRS
  qgeopositioninfosource_win7.h
  qgeopositioninfosourcefactory_win7.h
)

qt4_wrap_cpp(win7_MOCS ${win7_HDRS})

add_library (qtposition_win7 ${PLUGIN_TYPE} ${win7_SRCS} ${win7_MOCS})

link_directories(${QT_MOBILITY_LIBRARY_DIR})
target_link_libraries(qtposition_win7
	${QT_LIBRARIES}
	${QT_MOBILITY_LOCATION_LIBRARY}
)

install(TARGETS qtposition_win7
  COMPONENT plugins
  LIBRARY DESTINATION ${PLUGINS_INSTALL_DIR}
  RUNTIME DESTINATION ${PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${PLUGINS_INSTALL_DIR}
)
