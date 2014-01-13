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

#include "textformats.h"

#include "dbg.h"

#include <QTextCursor>
#include <QVariant>
#include <QTextFrame>

bool TextFormats::isTitle( const QTextCursor &cursor )
{
  QTextBlockFormat format = cursor.blockFormat();

  if ( format.topMargin() == 8 ) return true;

  return false;
}

bool TextFormats::isSubTitle( const QTextCursor &cursor )
{
  QTextBlockFormat format = cursor.blockFormat();

  if ( format.topMargin() == 6 ) return true;

  return false;
}

bool TextFormats::isTodoodleLink( const QTextCursor &cursor )
{
  QTextCharFormat format = cursor.charFormat();
  return format.hasProperty( LinkType ) &&
    format.property( LinkType ).toInt() == TodoodleLink;
}

bool TextFormats::isHyperLink( const QTextCursor &cursor )
{
  QTextCharFormat format = cursor.charFormat();
  return format.hasProperty( LinkType ) &&
    format.property( LinkType ).toInt() == HyperLink;
}


QTextCharFormat TextFormats::titleCharFormat()
{
  QTextCharFormat f;
  f.setFontPointSize( 15 );
  f.setFontWeight( QFont::Bold );

#if 0
  f.setFontUnderline( false );
  f.setForeground( Qt::black );
  f.setAnchor( false );
  f.setAnchorHref( QString::null );
#endif

  return f;
}

QTextBlockFormat TextFormats::titleBlockFormat()
{
  QTextBlockFormat f;
  f.setBottomMargin( 4 );
  f.setTopMargin( 8 );
  return f;
}


QTextCharFormat TextFormats::subTitleCharFormat()
{
  QTextCharFormat f;
  f.setFontPointSize( 12 );
  f.setFontWeight( QFont::Bold );

#if 0
  f.setFontUnderline( false );
  f.setForeground( Qt::black );
  f.setAnchor( false );
  f.setAnchorHref( QString::null );
#endif

  return f;
}

QTextBlockFormat TextFormats::subTitleBlockFormat()
{
  QTextBlockFormat f;
  f.setBottomMargin( 4 );
  f.setTopMargin( 6 );
  return f;
}


QTextCharFormat TextFormats::normalCharFormat()
{
  QTextCharFormat f;
  f.setFontFamily( "Times" );
  f.setFontPointSize( 10 );
  f.setFontWeight( QFont::Normal );
  f.setFontUnderline( false );
  f.setForeground( Qt::black );
  f.setAnchor( false );
  f.setAnchorHref( QString::null );
  return f;
}

QTextBlockFormat TextFormats::normalBlockFormat()
{
  QTextBlockFormat f;
  f.setBottomMargin( 0 );
  f.setTopMargin( 0 );
  return f;
}

QTextCharFormat TextFormats::topicLinkCharFormat( const QString &topic )
{
  QTextCharFormat format;

  format.setAnchor( true );
  QString href;
  if ( topic.startsWith( "todoodle:" ) ) href = topic;
  else href = "todoodle:" + topic;
  format.setAnchorHref( href );
  format.setFontUnderline( true );
  format.setForeground( Qt::red );
  format.setProperty( LinkType, TodoodleLink );

  return format;
}

QTextCharFormat TextFormats::hyperLinkCharFormat( const QString &link )
{
  QTextCharFormat format;

  format.setAnchor( true );
  format.setAnchorHref( link );
  format.setFontUnderline( true );
  format.setForeground( Qt::blue );
  format.setProperty( LinkType, HyperLink );

  return format;
}

QTextCharFormat TextFormats::dateTimeFormat()
{
  QTextCharFormat format;
  
  format.setFontItalic( true );
  
  return format;
}

QTextBlockFormat TextFormats::codeBlockFormat()
{
  QTextBlockFormat format;
  
  format.setBackground( QColor( "#d0d0d0" ) );

  return format;
}

QTextFrameFormat TextFormats::codeFrameFormat()
{
  QTextFrameFormat format;
  
  format.setBorder( 1 );
  format.setPadding( 4 );
  format.setProperty( FrameType, CodeFrame );

  return format;
}

QTextCharFormat TextFormats::codeCharFormat()
{
  QTextCharFormat format;
  
  format.setFontFamily( "courier" );
  
  return format;
}

void TextFormats::setCodeFrameFormats( QTextFrame *frame )
{
  QTextCursor frameCursor = frame->firstCursorPosition();
  frameCursor.setPosition( frame->lastPosition(), QTextCursor::KeepAnchor );

  frameCursor.mergeBlockFormat( TextFormats::codeBlockFormat() );
  frameCursor.mergeCharFormat( TextFormats::codeCharFormat() );
  frameCursor.mergeBlockCharFormat( TextFormats::codeCharFormat() );
}

void TextFormats::setLastModified( QTextCursor &cursor, const QDateTime &dt )
{
  QTextBlockFormat f = cursor.blockFormat();
  f.setProperty( LastModified, dt );
  cursor.setBlockFormat( f );
}

void TextFormats::setLastModified( QTextCursor &cursor )
{
  setLastModified( cursor, QDateTime::currentDateTime() );
}

QDateTime TextFormats::lastModified( const QTextCursor &cursor )
{
  QTextBlockFormat f = cursor.blockFormat();
  QDateTime lastModified = f.property( LastModified ).toDateTime();
  return lastModified;
}
