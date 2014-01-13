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

#include "todoodle.h"
#include "topicmanager.h"
#include "dbg.h"
#include "cmdlineargs.h"

#include <qapplication.h>
#include <QDir>
#include <QMessageBox>

int main( int argc, char ** argv )
{
  QApplication a( argc, argv );

  QString topicDir;

  CmdLineArgs args( &a );

  if ( args.argCount() == 1 ) {
    topicDir = args.arg( 0 );
    dbg() << "TOPIC DIR: " << topicDir << endl;

    QDir d( topicDir );
    d.makeAbsolute();
    if ( !d.exists() ) {
      int result = QMessageBox::question( 0, "Directory does not exist.",
        "Directory '" + d.path() + "' is does not exist.\n"
        "Create it now?", QMessageBox::Yes, QMessageBox::No );
      if ( result == QMessageBox::Yes ) {
        d.mkdir( d.path() );
      } else {
        return 1;
      }
    }
  }

  TopicManager::Mode mode = TopicManager::Online;

  if ( args.hasOption( "offline" ) ) {
    dbg() << "OFFLINE MODE" << endl;
    mode = TopicManager::Offline;
  }

  TopicManager::WindowMode windowMode;
  if ( args.hasOption( "singlewindow" ) ) {
    windowMode = TopicManager::Single;
  } else {
    windowMode = TopicManager::Multiple;
  }

  TopicManager topicManager( topicDir, mode, windowMode );

  topicManager.loadStart();
  
  a.connect( &a, SIGNAL( lastWindowClosed() ), &topicManager,
    SLOT( finishSave() ) );

  int r = a.exec();
  
  return r;
}
