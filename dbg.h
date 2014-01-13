#ifndef DBG_H
#define DBG_H

#include <QTextOStream>

#include <stdio.h>

/**
  This class is a crude approximation of qDebug. It's here for historical
  reasons only.
*/
class dbg : public QTextStream
{
  public:
    dbg() : QTextStream( stderr ) {}
};

#endif
