TARGET   = rtf-qt
TEMPLATE = lib

#
# Build configuration
#
CONFIG += qt thread warn_on

#
# Подключаем библиотеку QUAZIP
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../quazip/debug/ -lquazip
else:mac: LIBS += -F$$OUT_PWD/../../libs/quazip/ -framework quazip
else:unix: LIBS += -L$$OUT_PWD/../quazip/ -lquazip

INCLUDEPATH += $$PWD/../quazip
DEPENDPATH += $$PWD/../quazip
#

HEADERS += \
    controlword.h \
    rtfreader.h \
    UserPropsDestination.h \
    Tokenizer.h \
    Token.h \
    TitlePcdataDestination.h \
    TextDocumentRtfOutput.h \
    SubjectPcdataDestination.h \
    StyleSheetTableEntry.h \
    StyleSheetDestination.h \
    RtfGroupState.h \
    PictDestination.h \
    PcdataDestination.h \
    OperatorPcdataDestination.h \
    ODFOutput.h \
    ManagerPcdataDestination.h \
    KeywordsPcdataDestination.h \
    InfoTimeDestination.h \
    InfoRevisedTimeDestination.h \
    InfoPrintedTimeDestination.h \
    InfoDestination.h \
    InfoCreatedTimeDestination.h \
    IgnoredDestination.h \
    HLinkBasePcdataDestination.h \
    GeneratorPcdataDestination.h \
    FontTableEntry.h \
    FontTableDestination.h \
    DocumentDestination.h \
    DocumentCommentPcdataDestination.h \
    Destination.h \
    CompanyPcdataDestination.h \
    CommentPcdataDestination.h \
    ColorTableDestination.h \
    CategoryPcdataDestination.h \
    AuthorPcdataDestination.h \
    AbstractRtfOutput.h

SOURCES += \
    PictDestination.cpp \
    rtfreader.cpp \
    UserPropsDestination.cpp \
    Tokenizer.cpp \
    Token.cpp \
    TitlePcdataDestination.cpp \
    TextDocumentRtfOutput.cpp \
    SubjectPcdataDestination.cpp \
    StyleSheetDestination.cpp \
    PcdataDestination.cpp \
    OperatorPcdataDestination.cpp \
    ODFOutput.cpp \
    ManagerPcdataDestination.cpp \
    KeywordsPcdataDestination.cpp \
    InfoTimeDestination.cpp \
    InfoRevisedTimeDestination.cpp \
    InfoPrintedTimeDestination.cpp \
    InfoDestination.cpp \
    InfoCreatedTimeDestination.cpp \
    IgnoredDestination.cpp \
    HLinkBasePcdataDestination.cpp \
    GeneratorPcdataDestination.cpp \
    FontTableDestination.cpp \
    DocumentDestination.cpp \
    DocumentCommentPcdataDestination.cpp \
    Destination.cpp \
    controlword.cpp \
    CompanyPcdataDestination.cpp \
    CommentPcdataDestination.cpp \
    ColorTableDestination.cpp \
    CategoryPcdataDestination.cpp \
    AuthorPcdataDestination.cpp \
    AbstractRtfOutput.cpp
