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
#ifndef TOPICLIST_H
#define TOPICLIST_H

#include <qwidget.h>
#include <QAbstractItemModel>
#include <QProxyModel>

class TopicManager;

class QListWidgetItem;

/**
  This class represents a model of Todoodle topics.
*/
class TopicModel : public QAbstractItemModel
{
    Q_OBJECT
  public:
    /**
      Create topic model for topics managed by given TopicManager.
      
      \param tm topic manager
      \param parent parent object
    */
    TopicModel( TopicManager *tm, QObject *parent );

    int columnCount ( const QModelIndex & parent ) const;
    QVariant data ( const QModelIndex & index, int role ) const;
    QModelIndex index ( int row, int column, const QModelIndex & parent ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    int rowCount ( const QModelIndex & parent ) const;
    bool hasChildren( const QModelIndex & parent ) const;

  public slots:
    void slotClicked( const QModelIndex & index, Qt::MouseButton button, Qt::KeyboardModifiers modifiers );
    void slotReturnPressed ( const QModelIndex & index );

  signals:
    void topicSelected( const QString & );

  private:
    TopicManager *mTopicManager;
};

/**
  This class implements a sorting proxy model for topics.
*/
class TopicProxyModel : public QProxyModel
{
  public:
    TopicProxyModel( QObject *parent );
  
    bool isSortable() const;
    
    void sort( int column, const QModelIndex &parent, Qt::SortOrder order );
};

/**
  This class is a widget for showing a list of topics.
*/
class TopicList : public QWidget
{
    Q_OBJECT
  public:
    TopicList( TopicManager *, QWidget *parent );
  
  protected slots:
    void slotItemClicked( QListWidgetItem * );
    void slotTopicSelected( const QString &topic );

  private:
    TopicManager *mTopicManager;
};

#endif
