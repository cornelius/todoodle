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

#include "topicmanager.h"

#include "todoodle.h"
#include "format.h"
#include "dbg.h"
#include "versioncontrol.h"
#include "textformats.h"
#include "topicinfo.h"
#include "topicmap.h"
#include "hypertextedit.h"
#include "prefs.h"
#include "nextactionslist.h"

#include <QTextCursor>
#include <QFile>
#include <QDir>
#include <QTextCharFormat>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QResource>
#include <QTextStream>

TopicManager::TopicManager( const QString &dirName, Mode mode,
  WindowMode windowMode )
  : mSingleEditor( 0 ), mTopicDir( dirName ), mTopicMap( 0 ),
    mNextActionsList( 0 ),
    mWindowMode( windowMode )
{
  mPrefs = new Prefs( topicDir() );

  if ( mode == Online && QFile::exists( topicDir() + "/.svn" ) ) {
    dbg() << "Activate Todoodle version control" << endl;
    
    mVersionControl = new VersionControl( topicDir() );
    connect( mVersionControl, SIGNAL( commitFinished() ),
      SLOT( slotSaveFinished() ) );
    connect( mVersionControl, SIGNAL( updateFinished() ),
      SLOT( slotLoadStartFinished() ) );
  } else {
    mVersionControl = 0;
  }
}

TopicManager::~TopicManager()
{
  dbg() << "~TopicManager()" << endl;

  if ( mWindowMode == Single ) {
    mPrefs->setStartTopic( mCurrentTopic );
  }

  delete mTopicMap;

  foreach( Todoodle *t, mEditors ) delete t;
  foreach( TopicInfo *i, mInfos ) delete i;

  delete mPrefs;
}

Todoodle *TopicManager::editor( const QString &topic )
{
  dbg() << "TopicManager::editor(): " << topic << endl;

  Todoodle *editor = 0;

  if ( mWindowMode == Single ) {
    if ( !mSingleEditor ) {
      mSingleEditor = new Todoodle( this );
    }
    editor = mSingleEditor;
    editor->loadTopic( topic );
    mCurrentTopic = topic;
  } else {
    QMap<QString, Todoodle *>::iterator it = mEditors.find( topic );
    if ( it != mEditors.end() ) return it.value();

    editor = new Todoodle( this );
    editor->loadTopic( topic );
    mEditors.insert( topic, editor );
    if ( mNextActionsList ) mNextActionsList->addEditor( editor->editor() );
  }
  
  return editor;
}

void TopicManager::removeEditor( const QString &topic )
{
  dbg() << "TopicManager::removeEditor(): " << topic << endl;

  mEditors.remove( topic );
}

bool TopicManager::load( const QString &topic, HyperTextEdit *editor )
{
  editor->setCurrentCharFormat( QTextCharFormat() );
  editor->clear();
  editor->init();

  Format format( editor->document() );
  
  QFileInfo fi( topicFilename( topic ) );
  
  if ( !format.load( topicFilename( topic ) ) && !fi.exists() ) {
    dbg() << "TopicManager::load() Creating new topic: " << topic << endl;

    if ( topic == "Start" || topic == "Manual" ) {    
      QString resourceName = QString(":/manual/%1.todoodle").arg( topic );
      dbg() << "RES: " << resourceName << endl;

      QFile f( resourceName );
      if ( !f.open( QIODevice::ReadOnly ) ) {
        dbg() << "Error opening resource " << resourceName << endl;
      } else {
        QTextStream t( &f );
        QString data = t.readAll();
        format.fromString( data );
      }
    } else {
      QTextCursor cursor = editor->textCursor();

      cursor.insertText( topic, TextFormats::titleCharFormat() );
      cursor.setBlockFormat( TextFormats::titleBlockFormat() );

      cursor.insertBlock();

      editor->setCurrentCharFormat( TextFormats::normalCharFormat() );
    }

    save( topic, editor );
    
    if ( mVersionControl ) mVersionControl->addFile( topicFilename( topic ) );
    
    return false;
  }

  return true;
}

bool TopicManager::save( const QString &topic, HyperTextEdit *editor )
{
  Format f( editor->document() );
  return f.save( topicFilename( topic ) );
}

QString TopicManager::topicFilename( const QString &topic )
{
  return topicDir() + topic + ".todoodle";
}

QString TopicManager::scratchPadFilename( const QString &topic )
{
  return topicDir() + topic + ".scratchpad";
}

QString TopicManager::topicText( const QString &topic )
{
  QFile file( topicFilename( topic ) );
  if ( !file.open( QIODevice::ReadOnly ) ) {
    dbg() << "topicText(): Unable to open file '" + topicFilename( topic ) +
      "'." << endl;
    return QString::null;
  } else {
    QTextStream ts( &file );
    return ts.readAll();
  }
}

bool TopicManager::topicExists( const QString &topic )
{
  QFileInfo fi( topicFilename( topic ) );
  
  return fi.exists();
}

QString TopicManager::topicDir()
{
  if ( mTopicDir.isEmpty() ) {
    QString dir = getenv( "HOME" );
    dir += "/.todoodle/";
    QDir d( dir );
    d.makeAbsolute();

    // dbg() << "topicDir: " << d.path() << endl;

    if ( !d.exists() ) d.mkdir( dir );

    mTopicDir = d.path();
  }

  if ( !mTopicDir.endsWith( "/" ) ) mTopicDir.append( "/" );
  
  return mTopicDir;
}

QStringList TopicManager::topics()
{
  if ( mTopics.isEmpty() ) {
    QDir dir( topicDir() );

    QStringList entries = dir.entryList();  
    foreach( QString entry, entries ) {
      if ( entry.endsWith( ".todoodle" ) ) {
        mTopics.append( entry.left( entry.length() - 9 ) );
      }
    }
  }
  
  return mTopics;
}

void TopicManager::finishSave()
{
  if ( mVersionControl ) {
    mVersionControl->commitDirectory( "Todoodle was here" );
  } else {
    slotSaveFinished();
  }
}

void TopicManager::slotSaveFinished()
{
  qApp->quit();
}

void TopicManager::closeAll()
{
  QMap<QString, Todoodle *>::ConstIterator it;
  for( it = mEditors.begin(); it != mEditors.end(); ++it ) {
    it.value()->close();
  }
}

void TopicManager::loadStart()
{
  if ( mVersionControl ) {
    mVersionControl->updateDirectory();
  } else {
    slotLoadStartFinished();
  }
}

void TopicManager::slotLoadStartFinished()
{
  QString startTopic = mPrefs->startTopic();
  if ( startTopic.isEmpty() ) startTopic = "Start";

  Todoodle *e = editor( startTopic );
  
  e->show();
}

TopicInfo *TopicManager::info( const QString &topic )
{
  QMap<QString,TopicInfo *>::ConstIterator it = mInfos.find( topic );
  if ( it == mInfos.end() ) {
    if ( !topicExists( topic ) ) {
      return 0;
    } else {
      TopicInfo *info = new TopicInfo;
      
      QFileInfo fi( topicFilename( topic ) );
      info->setLastModified( fi.lastModified() );
      
      mInfos.insert( topic, info );
      
      return info;
    }
  } else {
    return it.value();
  }
}

void TopicManager::showTopicMap()
{
  if ( !mTopicMap ) {
    mTopicMap = new TopicMap( this, 0 );
  }
  mTopicMap->show();
  mTopicMap->raise();
}

void TopicManager::showNextActionsList()
{
  if ( !mNextActionsList ) {
    mNextActionsList = new NextActionsList;
    foreach( Todoodle *t, mEditors ) {
      mNextActionsList->addEditor( t->editor() );
    }
    mNextActionsList->refreshList();
  }
  mNextActionsList->show();
  mNextActionsList->raise();
}
