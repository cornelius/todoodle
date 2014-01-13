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
#ifndef TOPICMAPWIDGET_H
#define TOPICMAPWIDGET_H

#include <QFrame>
#include <QPoint>

class QSettings;

class TopicManager;

/**
  This class represents a topic as an item of the topic map widget.
*/
class TopicItem
{
  public:
    QPoint pos;
    QString text;
    QString topic;
};

/**
  This class is a widget showing a map of topics.
*/
class TopicMapWidget : public QFrame
{
  public:
    TopicMapWidget( QWidget *parent );
    ~TopicMapWidget();

    QSize sizeHint();
    QSize minimumSizeHint();

    /**
      Setup map for topics managed by given topic manager.
      
      \param tm topic manager
    */
    void setupItems( TopicManager *tm );

  protected:
    void writeSettings();
    void readSettings();
  
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );

    void drawTopic( QPainter *p, TopicItem * );

  private:
    QList<TopicItem *> mItems;

    TopicItem *mMoveItem;
    QPoint mClickOffset;
    
    QSettings *mSettings;
};

#endif
