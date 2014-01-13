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
#ifndef TOPICINFO_H
#define TOPICINFO_H

#include <QString>
#include <QDateTime>

/**
  This class holds information about a topic.
*/
class TopicInfo
{
  public:
    /**
      Create topic info.
    */
    TopicInfo();
    ~TopicInfo();

    /**
      Set date and time of last modification of topic.
    */
    void setLastModified( const QDateTime & );
    /**
      Return date and time of last modification topic.
    */
    QDateTime lastModified() const;

  private:
    QDateTime mLastModified;
};

#endif
