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
#ifndef FORMATPLAINTEXT_H
#define FORMATPLAINTEXT_H

#include <QString>
#include <QtXml/QDomElement>
#include <QTextCursor>

class QTextDocument;
class QTextFrame;

/**
  This class provides a write-only plain text storage format for topic data.
*/
class FormatPlainText
{
  public:
    /**
      Create a Format object opration on the given QTextDocument.
      
      \param d QTextDocument to load or save.
    */
    FormatPlainText( QTextDocument *d );

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
      
      \return plain text representation as string
    */
    QString toString();

  protected:
    QString frameToString( QTextFrame * );

    QString indent( int, const QString &symbol = " " );

  private:
    QTextDocument *mDocument;
};

#endif
