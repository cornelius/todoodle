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

#include "topicmapwidget.h"

#include "topicmanager.h"
#include "dbg.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QSettings>

TopicMapWidget::TopicMapWidget( QWidget *parent )
  : QFrame( parent ), mMoveItem( 0 )
{
  mSettings = new QSettings( "kde.org", "todoodle" );

  setBackgroundRole( QPalette::Midlight );
  setLineWidth( 2 );
  setFrameStyle( Panel + Sunken );
  setMinimumSize( 20, 20 );
}

TopicMapWidget::~TopicMapWidget()
{
  writeSettings();

  delete mSettings;

  qDeleteAll( mItems.begin(), mItems.end() );
  mItems.clear();
}

void TopicMapWidget::writeSettings()
{
  foreach( TopicItem *item, mItems ) {
    mSettings->setValue( item->topic + "/map/pos", item->pos );
  }
}

void TopicMapWidget::readSettings()
{
  foreach( TopicItem *item, mItems ) {
    if ( mSettings->contains( item->topic + "/map/pos" ) ) {
      item->pos = mSettings->value( item->topic + "/map/pos" ).toPoint();
    }
  }
}

void TopicMapWidget::setupItems( TopicManager *topicManager )
{
  dbg() << "TopicMapWidget::setupItems()" << endl;

  mSettings->beginGroup( topicManager->topicDir() );

  static int x = 20;
  static int y = 20;

  qDeleteAll( mItems.begin(), mItems.end() );
  mItems.clear();

  QStringList topics = topicManager->topics();
  foreach( QString t, topics ) {
  
    TopicItem *item = new TopicItem;
    item->topic = t;
    item->text = t;
    item->pos = QPoint( x, y );
  
    x += 70;
    if ( x > 400 ) {
      x = 20;
      y += 30;
    }

    mItems.append( item );
  }

  readSettings();
}

QSize TopicMapWidget::sizeHint()
{
  return QSize( 400, 200 );
}

QSize TopicMapWidget::minimumSizeHint()
{
  return QSize( 200, 100 );
}

void TopicMapWidget::paintEvent( QPaintEvent *e )
{
  QFrame::paintEvent( e );

  QPainter p( this );
  
  p.setPen( QPen( Qt::black, 2 ) );
  p.setBrush( Qt::cyan );

  foreach( TopicItem *item, mItems ) {
    drawTopic( &p, item );
  }
}

void TopicMapWidget::drawTopic( QPainter *p, TopicItem *item )
{
  QFontMetrics fm( font() );
  QRect r = fm.boundingRect( item->text );
  
  int margin = 6;

  r.translate( item->pos );

  r.setX( r.x() - margin );
  r.setY( r.y() - margin );
  r.setWidth( r.width() + 2 * margin );
  r.setHeight( r.height() + 2 * margin );

  p->drawRoundRect( r );
  p->drawText( item->pos, item->text );
}

struct SortHelper
{
  SortHelper( TopicItem *i, float dist ) : item( i ), distance( dist ) {}
  SortHelper() {}

  TopicItem *item;
  float distance;

  bool operator<( const SortHelper &other ) const
  {
    return distance < other.distance;
  }
};

void TopicMapWidget::mousePressEvent( QMouseEvent *e )
{
  QFrame::mousePressEvent( e );

  QList<SortHelper> l;
  foreach( TopicItem *item, mItems ) {
    l << SortHelper( item, QLineF( e->pos(), item->pos ).length() );
  }
  qSort( l );
  mMoveItem = l.at( 0 ).item;
  
  mClickOffset = e->pos() - mMoveItem->pos;
}

void TopicMapWidget::mouseReleaseEvent( QMouseEvent *e )
{
  QFrame::mouseReleaseEvent( e );

  mMoveItem = 0;
}

void TopicMapWidget::mouseMoveEvent( QMouseEvent *e )
{
  QFrame::mouseMoveEvent( e );

  if ( mMoveItem ) {
    QPoint p = e->pos();
    mMoveItem->pos = p - mClickOffset;
    update();
  }
}
