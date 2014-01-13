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

#include "cmdlineargs.h"

#include "dbg.h"

#include <QApplication>

CmdLineArgs::CmdLineArgs( QApplication *app )
{
  parseArgs( app );
}

int CmdLineArgs::argCount() const
{
  return mArgs.count();
}

void CmdLineArgs::parseArgs( QApplication *app )
{
  for( int i = 1; i < app->argc(); ++i ) {
    QString arg = app->argv()[ i ];
    if ( arg.startsWith( "-" ) ) {
      arg.remove( QRegExp( "^-+" ) );
      mOptions.insert( arg, "" );
    } else {
      mArgs.append( arg );
    }
  }
}

QString CmdLineArgs::arg( int n ) const
{
  if ( n > mArgs.size() - 1 ) {
    dbg() << "Illegal argument count: " << n << endl;
    return QString::null;
  }

  return mArgs.at( n );
}

bool CmdLineArgs::hasOption( const QString &option ) const
{
  return mOptions.contains( option );
}
