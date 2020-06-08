# -------------------------------------------------
# Project created by QtCreator 2009-09-18T12:29:43
# -------------------------------------------------
QT += network \
      xml \
      script

TARGET = CapiCity
TEMPLATE = app
DESTDIR = .
!win32 { 
    MOC_DIR = build
    OBJECTS_DIR = build
    RCC_DIR = build
    UI_HEADERS_DIR = build
}
SOURCES += main.cpp \
    src/Capi/capiobject.cpp \
    src/Capi/capitemplate.cpp \
    src/Capi/capigame.cpp \
    src/Capi/capiestate.cpp \
    src/Capi/capiplayer.cpp \
    src/Capi/capicard.cpp \
    src/Capi/capigroup.cpp \
    src/Capic/capiclient.cpp \
    src/Capic/capiclientgame.cpp \
    src/Capic/capiclientestate.cpp \
    src/Capic/Gui/field.cpp \
    src/Capic/Gui/playerauction.cpp \
    src/Capic/Gui/playerinfo.cpp \
    src/Capic/Gui/playeravatar.cpp \
    src/Capic/Gui/gameconfig.cpp \
    src/Capic/Gui/cmdtoolbutton.cpp \
    src/Capic/Gui/auction.cpp \
    src/Capic/Gui/tradeitem.cpp \
    src/Capic/Gui/trade.cpp \
    src/Capic/Gui/config.cpp \
    src/Capic/Gui/gameoption.cpp \
    src/Capic/Gui/gamebooloption.cpp \
    src/Capic/Gui/gameintoption.cpp \
    src/Capic/Gui/capiclientplayer.cpp \
    src/Capic/Gui/about.cpp \
    src/Capid/capiserver.cpp \
    src/Capid/capiserverplayer.cpp \
    src/Capid/capiservergame.cpp \
    src/Capid/capiserverestate.cpp \
    src/Capid/capiservergroup.cpp \
    src/Capid/capiservercard.cpp \
    src/Capid/capidept.cpp \
    src/Capid/capitradeitem.cpp \
    src/Capid/capitrade.cpp \
    src/Capid/messagetranslator.cpp \
    src/Capic/Gui/notifier.cpp \
    src/Capic/Gui/serverchooser.cpp \
    src/Capic/capiserverinfo.cpp \
    src/Capic/Gui/capicity.cpp
HEADERS += src/Capi/capiobject.h \
    src/Capi/capitemplate.h \
    src/Capi/capigame.h \
    src/Capi/capiestate.h \
    src/Capi/capiplayer.h \
    src/Capi/capicard.h \
    src/Capi/capigroup.h \
    src/Capic/capiclient.h \
    src/Capic/capiclientgame.h \
    src/Capic/capiclientestate.h \
    src/Capic/Gui/field.h \
    src/Capic/Gui/playerauction.h \
    src/Capic/Gui/playerinfo.h \
    src/Capic/Gui/playeravatar.h \
    src/Capic/Gui/gameconfig.h \
    src/Capic/Gui/cmdtoolbutton.h \
    src/Capic/Gui/auction.h \
    src/Capic/Gui/tradeitem.h \
    src/Capic/Gui/trade.h \
    src/Capic/Gui/config.h \
    src/Capic/Gui/gameoption.h \
    src/Capic/Gui/gamebooloption.h \
    src/Capic/Gui/gameintoption.h \
    src/Capic/Gui/capiclientplayer.h \
    src/Capic/Gui/about.h \
    src/Capid/capiserver.h \
    src/Capid/capiserverplayer.h \
    src/Capid/capiservergame.h \
    src/Capid/capiserverestate.h \
    src/Capid/capiservergroup.h \
    src/Capid/capiservercard.h \
    src/Capid/capidept.h \
    src/Capid/capitradeitem.h \
    src/Capid/capitrade.h \
    src/Capid/messagetranslator.h \
    src/Capic/Gui/notifier.h \
    src/defines.h \
    src/Capic/Gui/serverchooser.h \
    src/Capic/capiserverinfo.h \
    src/Capic/Gui/capicity.h
FORMS += \
    src/Capic/Gui/playerauction.ui \
    src/Capic/Gui/playerinfo.ui \
    src/Capic/Gui/gameconfig.ui \
    src/Capic/Gui/field.ui \
    src/Capic/Gui/auction.ui \
    src/Capic/Gui/tradeitem.ui \
    src/Capic/Gui/trade.ui \
    src/Capic/Gui/config.ui \
    src/Capic/Gui/gamebooloption.ui \
    src/Capic/Gui/gameintoption.ui \
    src/Capic/Gui/about.ui \
    src/Capic/Gui/notifier.ui \
    src/Capic/Gui/serverchooser.ui \
    src/Capic/Gui/capicity.ui
RESOURCES += ressources.qrc
TRANSLATIONS = ts/CapiCity_de_DE.ts \
    ts/CapiCity_fi_FI.ts \
    ts/CapiCity_pl.ts \
    ts/CapiCity_it.ts \
    ts/CapiCity_es_ES.ts \
    ts/CapiCity_ru_RU.ts
