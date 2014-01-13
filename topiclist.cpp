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

#include "topiclist.h"

#include "topicmanager.h"
#include "todoodle.h"
#include "dbg.h"
#include "topicinfo.h"

#include "qlayout.h"
#include "QListWidget"
#include "QPushButton"
#include <QTreeView>
#include <QTreeWidget>
#include <QTableView>
#include <QHeaderView>

TopicModel::TopicModel( TopicManager *topicManager, QObject *parent )
  : QAbstractItemModel( parent ), mTopicManager( topicManager )
{
}

int TopicModel::columnCount( const QModelIndex & ) const
{
  return 2;
}

QVariant TopicModel::data( const QModelIndex & index, int role ) const
{
  if ( !index.isValid() ) return QVariant();

  if ( role == Qt::DisplayRole ) {
    QStringList topics = mTopicManager->topics();
    QString topic = topics.at( index.row() );
    if ( index.column() == 0 ) {
      return topic;
    } else if ( index.column() == 1 ) {
      TopicInfo *info = mTopicManager->info( topic );
      if ( info ) {
        return info->lastModified().toString();
      } else {
        return "Unknown";
      }
    } else {
      dbg() << "TopicModel::data(): Index out of range: " << index.column()
        << endl;
    }
#if 0
  } else if ( role == QAbstractItemModel::DecorationRole ) {
    return QPixmap( ":/images/filenew.png" );
#endif
  }
  
  return QVariant();
}

QModelIndex TopicModel::index( int row, int column,
  const QModelIndex & ) const
{
  return createIndex( row, column, 0 );
}

QModelIndex TopicModel::parent( const QModelIndex & ) const
{
  return QModelIndex();
}

int TopicModel::rowCount( const QModelIndex & ) const
{
  return mTopicManager->topics().count();
}

bool TopicModel::hasChildren( const QModelIndex & ) const
{
  return false;
}

void TopicModel::slotClicked( const QModelIndex & index, Qt::MouseButton /*button*/, Qt::KeyboardModifiers /*modifiers*/ )
{
  dbg() << "Clicked: " << index.row() << endl;

  QModelIndex i = createIndex( index.row(), 0 );

  emit topicSelected( data( i, Qt::DisplayRole ).toString() );
}
 
void TopicModel::slotReturnPressed( const QModelIndex & index )
{
  dbg() << "Return pressed: " << index.row() << endl;

  emit topicSelected( data( index, Qt::DisplayRole ).toString() );
}


TopicProxyModel::TopicProxyModel( QObject *parent )
  : QProxyModel( parent )
{
}

bool TopicProxyModel::isSortable() const
{
  return true;
}

void TopicProxyModel::sort( int column, const QModelIndex &parent,
  Qt::SortOrder order )
{
  Q_UNUSED( parent );
  Q_UNUSED( order );

  dbg() << "TopicProxyModel::sort() column: " << column << endl;
}


TopicList::TopicList( TopicManager *topicManager, QWidget *parent )
  : QWidget( parent ), mTopicManager( topicManager )
{
  qDebug( "TopicList()" );

  QBoxLayout *topLayout = new QVBoxLayout( this );
  
#if 0
  QListWidget *list = new QListWidget( this );
  connect( list, SIGNAL( clicked( QListWidgetItem *,
    ButtonState ) ), SLOT( slotItemClicked( QListWidgetItem * ) ) );
  connect( list, SIGNAL( returnPressed( QListWidgetItem * ) ),
    SLOT( slotItemClicked( QListWidgetItem * ) ) );
  topLayout->addWidget( list );

  QStringList topics = mTopicManager->topics();
  
  foreach( QString topic, topics ) {
    new QListWidgetItem( topic, list );
  }
#endif

  TopicModel *model = new TopicModel( mTopicManager, this );
  
  TopicProxyModel *proxyModel = new TopicProxyModel( this );
  proxyModel->setModel( model );
  
#if 0
  QListView *view = new QListView( this );
  view->setViewMode( QListView::IconMode );
#endif
#if 1
  QTableView *view = new QTableView( this );
  view->verticalHeader()->hide();
//  view->horizontalHeader()->hide();
  view->setAlternatingRowColors( true );
  view->setShowGrid( false );
#else
#if 0
  QTreeWidget *treeWidget = new QTreeWidget( this );
  treeWidget->setSortingEnabled( true );
  QTreeView *view = treeWidget;
#else
  QTreeView = new QTreeView( this );
#endif
  view->setAlternatingRowColors( true );
  view->header()->setMovable( true );
#endif
  topLayout->addWidget( view );
  view->setModel( proxyModel );

  connect( view, SIGNAL( clicked( const QModelIndex &, Qt::MouseButton, Qt::KeyboardModifier ) ),
    model, SLOT( slotClicked( const QModelIndex &, Qt::ButtonState, Qt::KeyboardModifier ) ) );
  connect( view, SIGNAL( returnPressed( const QModelIndex & ) ),
    model, SLOT( slotReturnPressed( const QModelIndex & ) ) );

  connect( model, SIGNAL( topicSelected( const QString & ) ),
    SLOT( slotTopicSelected( const QString & ) ) );

  QPushButton *button = new QPushButton( "C&lose", this );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  topLayout->addWidget( button );
}

void TopicList::slotItemClicked( QListWidgetItem *item )
{
  dbg() << "TopicList::slotItemClicked()" << endl;

  slotTopicSelected( item->text() );
}

void TopicList::slotTopicSelected( const QString &topic )
{
  dbg() << "TopicList::slotTopicSelected() " << topic << endl;

  Todoodle *t = mTopicManager->editor( topic );
  t->show();
  t->raise();
}
