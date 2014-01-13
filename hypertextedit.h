/*
    This file is part of Todoodle.

    Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef HYPERTEXTEDIT_H
#define HYPERTEXTEDIT_H

#include <QTextEdit>

class WordHandler;
class SequenceHandler;

class QHelpEvent;

/**
  This class provides the text editing widget used by Todoodle. It's derived
  from QTextEdit and adds functionality to handle clicks on links and extends
  direct keyboard manipulation.
*/
class HyperTextEdit : public QTextEdit
{
    Q_OBJECT
  public:
    /**
      Create editing widget.
      
      \param parent parent widget
    */
    HyperTextEdit( QWidget *parent );
    ~HyperTextEdit();

    /**
      Initialize widget.
    */
    void init();

    /**
      Add word handler. HyperTextEdit takes ownership of the object.
    */
    void addHandler( WordHandler * );
    /**
      Add sequence handler. HyperTextEdit takes ownership of the object.
    */
    void addHandler( SequenceHandler * );

  signals:
    /**
      Emitted when the user clicks on a hyper link.
      
      \param link link reference
    */
    void anchorClicked( const QString &link );

  protected:
    QPoint translateCoordinates(const QPoint &point);

    bool event( QEvent *e );

    void helpEvent( QHelpEvent *ev );

    void mouseMoveEvent( QMouseEvent *ev );
    void mouseReleaseEvent( QMouseEvent *ev );
    void keyPressEvent( QKeyEvent *ev );

    void updent();
    void downdent();
    void indent();
    void outdent();
    void removeBlock();

  private:
    QList<WordHandler *> mWordHandlers;
    QList<SequenceHandler *> mSequenceHandlers;
};

#endif
