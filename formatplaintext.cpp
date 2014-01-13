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

#include "formatplaintext.h"

#include "dbg.h"
#include "textformats.h"

#include <QFile>
#include <QTextDocument>
#include <QTextStream>
#include <QTextFrame>
#include <QTextCursor>
#include <QTextList>
#include <QTextCodec>

FormatPlainText::FormatPlainText( QTextDocument *document )
  : mDocument( document )
{
}

bool FormatPlainText::save( const QString &f )
{
  QFile file( f );
  if ( !file.open( QIODevice::WriteOnly ) ) return false;
  QTextStream ts( &file );
  ts.setCodec( QTextCodec::codecForName( "utf8" ) );
  ts << toString();
  return true;
}

QString FormatPlainText::toString()
{
  QTextFrame *rootFrame = mDocument->rootFrame();

  return frameToString( rootFrame );
}

QString FormatPlainText::frameToString( QTextFrame *frame )
{
  QString out;

  QTextFrame::iterator it;
  for( it = frame->begin(); it != frame->end(); ++it ) {
    QTextBlock block = it.currentBlock();
    if ( block.isValid() ) {
      bool isTitle = false;
      bool isSubTitle = false;
      int titleIndent = 0;

      QTextCursor c( block );
      if ( TextFormats::isTitle( c ) ) {
        isTitle = true;
      } else if ( TextFormats::isSubTitle( c ) ) {
        isSubTitle = true;
      }

      QTextBlockFormat blockFormat = block.blockFormat();
      if ( blockFormat.isValid() ) {
        QTextList *list = block.textList();
        if ( list ) {
          QTextListFormat f = list->format();

          out += indent( f.indent() * 4 );
          titleIndent += f.indent() * 4;
        
          switch( f.style() ) {
            default:
            case QTextListFormat::ListDisc:
              out += "- ";
              break;
            case QTextListFormat::ListDecimal:
              out += "1 ";
              break;
          }
        } else {
          out += indent( blockFormat.indent() * 4 );
          titleIndent += blockFormat.indent() * 4;
        }
      }

      QString outText;

      QTextBlock::iterator it2;
      for( it2 = block.begin(); it2 != block.end(); ++it2 ) {
        QTextFragment fragment = it2.fragment();
        if ( !fragment.isValid() ) continue;

        QString text = fragment.text();

        for( int i = 0; i < text.size(); ++i ) {
          if ( text.at( i ) == 0xfffc ) {

            QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();
            if ( imageFormat.isValid() ) {
              if ( imageFormat.name().contains( "done" ) ) outText += "DONE: ";
              else outText += "TODO: ";
            } else {
              dbg() << "NO IMAGE FORMAT" << endl;
            }
          } else {
            outText += text.at( i );
          }
        }
      }

      out += outText + "\n";
      
      if ( isTitle ) {
        out += indent( titleIndent ) + indent( outText.length(), "=" ) + "\n";
      } else if ( isSubTitle ) {
        out += indent( titleIndent ) + indent( outText.length(), "-" ) + "\n";
      }
    }
    if ( it.currentFrame() ) {
      out += frameToString( it.currentFrame() );
    }
  }

  return out;
}

QString FormatPlainText::indent( int indent, const QString &symbol )
{
  QString out;

  for( int i = 0; i < indent; ++i ) {
    out.append( symbol );
  }

  return out;
}
