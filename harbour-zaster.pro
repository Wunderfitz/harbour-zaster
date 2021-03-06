# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-zaster

CONFIG += sailfishapp

QT += core sql

SOURCES += src/harbour-zaster.cpp \
    src/zaster.cpp \
    src/fints/dataelement.cpp \
    src/fints/dataelementgroup.cpp \
    src/fints/fintselement.cpp \
    src/fints/segment.cpp \
    src/fints/message.cpp \
    src/fints/fintsserializer.cpp \
    src/fints/fintsdeserializer.cpp \
    src/fints/fintsdialog.cpp \
    src/fints/institutessearchworker.cpp \
    src/fints/fintsbalances.cpp \
    src/simplecrypt.cpp \
    src/fints/fintsaccounts.cpp

DISTFILES += qml/*.qml \
    rpm/harbour-zaster.changes.in \
    rpm/harbour-zaster.spec \
    rpm/harbour-zaster.yaml \
    translations/*.ts \
    harbour-zaster.desktop \
    db/*.db \
    qml/pages/SingleTransactionPage.qml

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-zaster-de.ts

HEADERS += \
    src/zaster.h \
    src/fints/dataelement.h \
    src/fints/dataelementgroup.h \
    src/fints/fintselement.h \
    src/fints/segment.h \
    src/fints/message.h \
    src/fints/fintsserializer.h \
    src/fints/fintsdeserializer.h \
    src/fints/fintsdialog.h \
    src/fints/fintsglobals.h \
    src/fints/bpdconstants.h \
    src/fints/updconstants.h \
    src/fints/messageconstants.h \
    src/fints/transactionconstants.h \
    src/fints/institutessearchworker.h \
    src/fints/fintsbalances.h \
    src/simplecrypt.h \
    src/fints/fintsaccounts.h

database.files = db
database.path = /usr/share/$${TARGET}

images.files = images
images.path = /usr/share/$${TARGET}

INSTALLS += database images
