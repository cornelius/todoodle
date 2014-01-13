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
#ifndef SCRATCHWIDGET_H
#define SCRATCHWIDGET_H

#include <qpen.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpolygon.h>

class QMouseEvent;
class QResizeEvent;
class QPaintEvent;
class QToolButton;
class QSpinBox;

/**
  This class provides a scratch pad area. It's used by ScratchPad to provide a
  full scratch pad including controls.
*/
class ScratchWidget : public QWidget
{
    Q_OBJECT
  public:
    /**
      Create scratch pad area widget.
      
      \param parent parent widget
    */
    ScratchWidget( QWidget *parent = 0 );

    /**
      Set pen color.
      
      \param c color
    */
    void setPenColor( const QColor &c )
    { pen.setColor( c ); }
    /**
      Return color of pen.
    */
    QColor penColor()
    { return pen.color(); }

    /**
      Set width of pen.
      
      \param w width of pen in pixels
    */
    void setPenWidth( int w )
    { pen.setWidth( w ); }
    /**
      Return width of pen in pixels.
    */
    int penWidth()
    { return pen.width(); }

    /**
      Load scratch pad data from file.
      
      \param filename name of file.
    */
    void load( const QString &filename );
    /**
      Save scratch pad data to file.
      
      \param filename name of file.
    */
    void save( const QString &filename );

    /**
      Clear scratch pad area.
    */
    void clearScreen();

  protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );
    void paintEvent( QPaintEvent *e );

    QPen pen;

    QPolygon polyline;

    bool mousePressed;

    QPixmap buffer;
};

#endif
