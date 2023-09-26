QT += core gui printsupport
# QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TRANSLATIONS += QtLanguage_ru.ts
CODECFORSRC     = UTF-8

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CalendarWidget.cpp \
    EditWindow.cpp \
    TextEditor.cpp \
    SearchWidget.cpp \
    main.cpp

HEADERS += \
    CalendarWidget.h \
    EditWindow.h \
    TextEditor.h \
    SearchWidget.h

FORMS += \
    TextEditor.ui \
    SearchWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
