#HEADERS += \
#    $$PWD/handler/QBreakpadHandler.h


INCLUDEPATH += $$PWD/handler/

HEADERS += \
    $$PWD/handler/QBreakpadHandler.h

LIBS += \
-L$$PWD/handler -lqBreakpad
