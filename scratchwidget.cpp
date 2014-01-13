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

#include "scratchwidget.h"

#include "dbg.h"

#include <qapplication.h>
#include <qevent.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qtooltip.h>
#include <qrect.h>
#include <qpoint.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qcursor.h>
#include <qimage.h>
#include <qpixmap.h>

#include <QVector>

ScratchWidget::ScratchWidget( QWidget *parent )
    : QWidget( parent ), pen( Qt::black, 2 ), polyline( 3 ),
      mousePressed( false ), buffer( width(), height() )
{
  buffer.fill( palette().base().color() );
  setCursor( Qt::CrossCursor );
}

void ScratchWidget::save( const QString &filename )
{
  buffer.save( filename, "PNG" );
}

void ScratchWidget::load( const QString &filename )
{
  // FIXME: Why does the load crash when called for the second time?
//  buffer.load( filename );
}

void ScratchWidget::clearScreen()
{
  buffer.fill( palette().base().color() );
  update();
}

void ScratchWidget::mousePressEvent( QMouseEvent *e )
{
  mousePressed = true;
  polyline[2] = polyline[1] = polyline[0] = e->pos();
}

void ScratchWidget::mouseReleaseEvent( QMouseEvent * )
{
  mousePressed = false;
}

void ScratchWidget::mouseMoveEvent( QMouseEvent *e )
{
  if ( mousePressed ) {
    QPainter painter;
    painter.begin( &buffer );
    painter.setPen( pen );
    polyline[2] = polyline[1];
    polyline[1] = polyline[0];
    polyline[0] = e->pos();
    painter.drawPolyline( polyline );
    painter.end();

    // FIXME(after beta1)
    QRect r( polyline.boundingRect().x(),
             polyline.boundingRect().y(),
             polyline.boundingRect().width(),
             polyline.boundingRect().height());
    r = r.normalized();
    r.setLeft( r.left() - penWidth() );
    r.setTop( r.top() - penWidth() );
    r.setRight( r.right() + penWidth() );
    r.setBottom( r.bottom() + penWidth() );

    update( r );
  }
}

void ScratchWidget::resizeEvent( QResizeEvent *e )
{
  QWidget::resizeEvent( e );

  int w = width() > buffer.width() ?
          width() : buffer.width();
  int h = height() > buffer.height() ?
          height() : buffer.height();

  QPixmap tmp( buffer );
  buffer = QPixmap( w, h );
  buffer.fill( palette().base().color() );

  QPainter p( &buffer );
  p.drawPixmap( 0, 0, tmp );
}

void ScratchWidget::paintEvent( QPaintEvent *e )
{
  QWidget::paintEvent( e );

  QPainter p( this );

  QVector<QRect> rects = e->region().rects();
  for ( int i = 0; i < rects.count(); i++ ) {
    QRect r = rects[(int)i];
    p.drawPixmap( r.x(), r.y(), buffer, r.x(), r.y(), r.width(), r.height() );
  }
}
