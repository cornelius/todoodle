/*
    This file is part of Todoodle.

    Copyright (c) 2005 Cornelius Schumacher <schumacher@kde.org>
    
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
#ifndef PREFS_H
#define PREFS_H

#include <QString>

class QSettings;

/**
  This class holds preferences data and makes it persistent.
*/
class Prefs
{
  public:
    /**
      Create preferences object for topic directory.
      
      \param topicDir name of topic directory
    */
    Prefs( const QString &topicDir );
    ~Prefs();
    
    /**
      Set start topic
      
      \param topic name of topic.
    */
    void setStartTopic( const QString &topic );
    /**
      Return name of start topic.
    */
    QString startTopic() const;

    /**
      Return QSettings object which is used to store the preferences data.
    */
    QSettings *settings() const { return mSettings; }
        
  private:
    QSettings *mSettings;
};

#endif
