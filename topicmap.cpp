/*
    This file is part of Todoodle.

    Copyright (c) 2004  <schumacher@kde.org>

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

#include "topicmap.h"

#include "topicmapwidget.h"
#include "topicmanager.h"
#include "dbg.h"

#include <QPushButton>
#include <QPainter>
#include <QPen>
#include <QSettings>

#include "qlayout.h"

TopicMap::TopicMap( TopicManager *topicManager, QWidget *parent )
  : QWidget( parent )
{
  QBoxLayout *topLayout = new QVBoxLayout( this );
  
  TopicMapWidget *map = new TopicMapWidget( this );
  map->setupItems( topicManager );
  topLayout->addWidget( map );
  
  QPushButton *button = new QPushButton( "Close", this );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  topLayout->addWidget( button );

  mSettings = new QSettings( "kde.org", "todoodle" );
  mSettings->beginGroup( topicManager->topicDir() );

  if ( mSettings->contains( "/map/pos" ) ) {
    QPoint pos = mSettings->value( "/map/pos" ).toPoint();
    move( pos );
  }
  if ( mSettings->contains( "/map/size" ) ) {
    QSize size = mSettings->value( "/map/size" ).toSize();
    resize( size );
  }
}

TopicMap::~TopicMap()
{
  mSettings->setValue( "/map/pos", pos() );
  mSettings->setValue( "/map/size", size() );

  delete mSettings;
}
