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

#include "versioncontrol.h"

#include "dbg.h"

#include <QProcess> 
#include <QDebug>
#include <QTimer>

VersionControl::VersionControl( const QString &dirname )
  : mDirName( dirname ), mProcess( 0 ), mCommand( Undefined )
{
}

VersionControl::~VersionControl()
{
  if ( mProcess ) {
    dbg() << "Killing svn process" << endl;
    mProcess->terminate();
  }
}

bool VersionControl::isRunning()
{
  return mProcess;
}

bool VersionControl::addFile( const QString &filename )
{
  createProcess();

  mCommand = Add;

  mArguments << "add" << filename;

  return startProcess();
}

bool VersionControl::commitDirectory( const QString &log )
{
  createProcess();

  mCommand = Commit;

  mArguments << "ci" << "-m" << log;

  return startProcess();
}

bool VersionControl::updateDirectory()
{
  createProcess();
  
  mCommand = Update;
  
  mArguments << "up";
  
  return startProcess();
}

void VersionControl::createProcess()
{
  mProcess = new QProcess();
  mProcess->setWorkingDirectory( mDirName );
  connect( mProcess, SIGNAL( finished( int ) ), SLOT( slotProcessExited() ) );
  mArguments.clear();
}

bool VersionControl::startProcess()
{
  dbg() << "VersionControl::startProcess() CMD: " <<
    mArguments.join( " " ) << endl;

  mProcess->start( "svn", mArguments );

  return true;  
}

void VersionControl::slotProcessExited()
{
  QString standardOutput = mProcess->readAllStandardOutput();
  dbg() << "SVN STDOUT: " << endl;
  if ( standardOutput.isEmpty() )
    dbg() << "<empty>" << endl;
  else
    dbg() << standardOutput;

  QString standardError = mProcess->readAllStandardError();
  dbg() << "SVN STDERR: " << endl;
  if ( standardError.isEmpty() )
    dbg() << "<empty>" << endl;
  else
    dbg() << standardError;

  if ( mCommand == Commit ) emit commitFinished();
  else if ( mCommand == Update ) emit updateFinished();

  QTimer::singleShot( 0, this, SLOT( destroyProcess() ) );
}

void VersionControl::destroyProcess()
{
  delete mProcess;
  mProcess = 0;
}
