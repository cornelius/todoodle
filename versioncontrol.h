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
#ifndef VERSIONCONTROL_H
#define VERSIONCONTROL_H

#include <QObject>
#include <QStringList>

class QProcess;

/**
  This class manages version control of a given directory.
  
  It currently supports Subversion as version control system.
*/
class VersionControl : public QObject
{
    Q_OBJECT
  public:
    enum Cmd { Undefined, Add, Update, Commit };

    /**
      Setup version control for directory.
      
      \param dirname Name of directory.
    */
    VersionControl( const QString &dirname );
    ~VersionControl();
  
    /**
      Put new file under version control.
      
      \param filename Name of new file
      \return \c true on success, otherwise \c false
    */
    bool addFile( const QString &filename );
    /**
      Commit changes to version control system.
      
      \param log Log message briefly describing changes
      \return \c true on success, otherwise \c false
    */
    bool commitDirectory( const QString &log );

    /**
      Retrieve changes from version control system.

      \return \c true on success, otherwise \c false
    */
    bool updateDirectory();

    /**
      Return if a version control action is running.
      
      \return \c true, if an action is running, otherwise \c false
    */
    bool isRunning();

  signals:
    /**
      Emitted when commit of changes to version control system is finsihed.
    */
    void commitFinished();
    /**
      Emitted when retrieval of changes from version control system is finished.
    */
    void updateFinished();

  protected slots:
    void slotProcessExited();

    void destroyProcess();

  protected:
    void createProcess();
    bool startProcess();

  private:
    QString mDirName;
    QProcess *mProcess;
    QStringList mArguments;

    Cmd mCommand;
};

#endif
