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
#ifndef CMDLINEARGS_H
#define CMDLINEARGS_H

#include <QStringList>
#include <QMap>

class QApplication;

/**
  This class provides handling of command line arguments given to the
  application on startup.
*/
class CmdLineArgs
{
  public:
    /**
      Create command line handler for given QApplication.
      
      \param app QApplication instance
    */
    CmdLineArgs( QApplication *app );

    /**
      Return number of given arguments.
    */
    int argCount() const;
    
    /**
      Return nth argument.
      
      \param n position of argument to return
      \return argument
    */
    QString arg( int n ) const;
    
    /**
      Return, if the gien option was given as command line argument.
      
      \param option name of option
      \return \c true, if the option was part of the command line, otherwise \c
        false      
    */
    bool hasOption( const QString &option ) const;

  protected:
    void parseArgs( QApplication *app );

  private:
    QStringList mArgs;
    QMap<QString,QString> mOptions;
};

#endif
