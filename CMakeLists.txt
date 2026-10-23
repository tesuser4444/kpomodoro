cmake_minimum_required(VERSION 3.20)
project(kpomodoro VERSION 1.0.0 LANGUAGES CXX)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

set(QT_DEFAULT_MAJOR_VERSION 6)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickControls2)
find_package(KF6 REQUIRED COMPONENTS Notifications StatusNotifierItem)

qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

qt_add_executable(kpomodoro main.cpp pomodoro.cpp pomodoro.h)

qt_add_qml_module(kpomodoro
    URI org.kpomodoro.app
    NO_IMPORT_SCAN
    QML_FILES qml/Main.qml
)

target_compile_definitions(kpomodoro PRIVATE KPOMODORO_VERSION="${PROJECT_VERSION}")

target_link_libraries(kpomodoro PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::QuickControls2
    KF6::Notifications KF6::StatusNotifierItem
)

install(TARGETS kpomodoro ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
configure_file(kpomodoro.desktop.in ${CMAKE_BINARY_DIR}/kpomodoro.desktop @ONLY)
install(PROGRAMS ${CMAKE_BINARY_DIR}/kpomodoro.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES kpomodoro.notifyrc DESTINATION ${KDE_INSTALL_DATADIR}/knotifications6)

