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

#include "prefs.h"

#include <QSettings>

Prefs::Prefs( const QString &topicDir )
{
  mSettings = new QSettings( "kde.org", "todoodle" );
  mSettings->beginGroup( topicDir );

}

Prefs::~Prefs()
{
  delete mSettings;
}

void Prefs::setStartTopic( const QString &topic )
{
  mSettings->setValue( "startTopic", topic );
}

QString Prefs::startTopic() const
{
  return mSettings->value( "startTopic" ).toString();
}
