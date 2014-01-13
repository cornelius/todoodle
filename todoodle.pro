TEMPLATE        = app
TARGET          = todoodle

CONFIG          += qt warn_on

HEADERS         = todoodle.h hypertextedit.h format.h topicmanager.h \
                  topiclist.h topicmap.h versioncontrol.h scratchpad.h \
                  textformats.h scratchwidget.h topicmapwidget.h topicinfo.h \
                  wordhandler.h cmdlineargs.h formatplaintext.h prefs.h \
                  nextactionslist.h

SOURCES         = todoodle.cpp format.cpp \
                  main.cpp hypertextedit.cpp topicmanager.cpp \
                  topiclist.cpp topicmap.cpp versioncontrol.cpp \
                  scratchpad.cpp textformats.cpp scratchwidget.cpp \
                  topicmapwidget.cpp topicinfo.cpp cmdlineargs.cpp \
                  formatplaintext.cpp prefs.cpp nextactionslist.cpp

RESOURCES += todoodle.qrc

QT += xml

target.path = /usr/local/bin
INSTALLS += target
