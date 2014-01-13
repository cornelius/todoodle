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
#ifndef FORMAT_H
#define FORMAT_H

#include <QString>
#include <QtXml/QDomElement>
#include <QTextCursor>

class QTextDocument;
class QTextFrame;

/**
  This class provides a storage format for topic data. The format is an XML
  representation of the data of a topic.
*/
class Format
{
  public:
    /**
      Create a Format object opration on the given QTextDocument.
      
      \param d QTextDocument to load or save.
    */
    Format( QTextDocument *d );

    /**
      Load data from given file and put it into the QTextDocument this Format
      object operates on.
      
      \param filename name of file
      \return \c true on success, \c false on failure
    */
    bool load( const QString &filename );
    /**
      Save data to given file from the QTextDocument this Format
      object operates on.
      
      \param filename name of file
      \return \c true on success, \c false on failure
    */
    bool save( const QString &filename );

    /**
      Return string representation of the data of the QTextDocument this Format
      object operates on.
      
      \return XML representation as string
    */
    QString toString();
    /**
      Load data given as string into the QTextDocument this Format object
      operates on.
      
      \param xml XML representation as string
      \return \c true on success, \c false on failure
    */
    bool fromString( const QString &xml );

  protected:
    QString frameToString( QTextFrame * );

    void parseFrame( QTextCursor &cursor, const QDomElement &element );
    void parseBlock( QTextCursor &, const QDomElement & );

    QString escape( const QString & );

  private:
    QTextDocument *mDocument;
};

#endif
