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

#include "format.h"

#include "dbg.h"
#include "textformats.h"

#include <QFile>
#include <QTextDocument>
#include <QTextStream>
#include <QTextFrame>
#include <QTextCursor>
#include <QTextList>
#include <QTextCodec>
#include <QVariant>

 // REMEMBER TO BUMP VERSION WHEN CHANGING FORMAT
const int currentFormatVersion = 1;

Format::Format( QTextDocument *document )
  : mDocument( document )
{
}

bool Format::load( const QString &f )
{
  if ( !QFile::exists( f ) ) return false;

  QFile file( f );
  if ( !file.open( QIODevice::ReadOnly ) ) return false;

  QByteArray data = file.readAll();
  QString str = QString::fromUtf8( data );

  return fromString( str );
}

bool Format::save( const QString &f )
{
  QFile file( f );
  if ( !file.open( QIODevice::WriteOnly ) ) return false;
  QTextStream ts( &file );
  ts.setCodec( QTextCodec::codecForName( "utf8" ) );
  ts << toString();
  return true;
}

QString Format::toString()
{
  QTextFrame *rootFrame = mDocument->rootFrame();

  QString str;
  
  str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  str += "<!DOCTYPE todoodle SYSTEM \"todoodle.dtd\">\n";
  str += "<todoodle version=\"" + QString::number( currentFormatVersion ) + "\">\n";
  
  str += frameToString( rootFrame );

  str += "</todoodle>\n";

  return str;
}

bool Format::fromString( const QString &str )
{
  QDomDocument doc;
  if ( !doc.setContent( str ) ) {
    dbg() << "Error loading " << str << endl;
    return false;
  }

  mDocument->setPlainText( "" );

  QTextCursor cursor( mDocument );

#if 0
  cursor.movePosition( QTextCursor::Start );
  cursor.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
  cursor.removeSelectedText();
#endif

  cursor.movePosition( QTextCursor::Start );

  QDomElement docElement = doc.documentElement();

  const int version = docElement.attribute( "version", "1" ).toInt();
  if ( version != currentFormatVersion ) {
      qWarning( "Error loading format: found version %d, expected %d.", version, currentFormatVersion );
      return false;
  }

  parseFrame( cursor, docElement );

  return true;
}

void Format::parseFrame( QTextCursor &cursor, const QDomElement &element )
{
  QTextBlock extraBlock;
  bool hasExtraBlock = false;

  QDomNode n;
  for( n = element.firstChild(); !n.isNull(); n = n.nextSibling() ) {
    QDomElement e = n.toElement();
    if ( e.tagName() == "block" ) {
      if ( e.hasAttribute( "liststyle" ) ) {
        cursor.setCharFormat( TextFormats::normalCharFormat() );
        QTextListFormat f;
        f.setIndent( e.attribute( "listindent" ).toInt() );
        if ( e.attribute( "liststyle" ) == "decimal" ) {
          f.setStyle( QTextListFormat::ListDecimal );
        } else {
          f.setStyle( QTextListFormat::ListDisc );
        }
        cursor.mergeBlockFormat( TextFormats::normalBlockFormat() );
        cursor.insertList( f );
      } else if ( n != element.firstChild() ) {
        QTextBlockFormat f;
        if ( e.hasAttribute( "blockindent" ) ) {
          f.setIndent( e.attribute( "blockindent" ).toInt() );
        } else {
          f.setIndent( 0 );
        }
        if ( hasExtraBlock ) {
          QTextCursor c( extraBlock );
          c.setBlockFormat( f );
          hasExtraBlock = false;
        } else {
          cursor.insertBlock( f );
        }
      }
      if ( e.hasAttribute( "lastmodified" ) ) {
        QString str = e.attribute( "lastmodified" );
        QDateTime dt = QDateTime::fromString( str, Qt::ISODate );
        TextFormats::setLastModified( cursor, dt );
      }
      parseBlock( cursor, e );

      if ( e.hasAttribute( "titlestyle" ) ) {
        if ( e.attribute( "titlestyle" ) == "title" ) {
          cursor.mergeBlockFormat( TextFormats::titleBlockFormat() );
        } else if ( e.attribute( "titlestyle" ) == "subtitle" ) {
          cursor.mergeBlockFormat( TextFormats::subTitleBlockFormat() );
        }
      } else {
        cursor.mergeBlockFormat( TextFormats::normalBlockFormat() );
      }
    } else if ( e.tagName() == "frame" ) {
      QTextFrame *parentFrame = cursor.currentFrame();

      QTextFrame *frame = cursor.insertFrame( TextFormats::codeFrameFormat() );
      parseFrame( cursor, e );
      if ( e.attribute( "type" ) == "code" ) {
        TextFormats::setCodeFrameFormats( frame );
      }

      cursor = parentFrame->lastCursorPosition();
      extraBlock = cursor.block();
      hasExtraBlock = true;
    }
  }
}

void Format::parseBlock( QTextCursor &cursor, const QDomElement &element )
{
//  dbg() << "Format::parseBlock()" << endl;

  QDomNode n;
  for( n = element.firstChild(); !n.isNull(); n = n.nextSibling() ) {
    QDomElement e = n.toElement();
    if ( e.tagName() == "fragment" ) {
      QTextCharFormat format;
      if ( e.hasAttribute( "link" ) ) {
        format.setAnchor( true );
        QString href = e.attribute( "link" );
        format.setAnchorHref( href );
        format.setFontUnderline( true );
        if ( href.startsWith( "todoodle:" ) ) {
          format = TextFormats::topicLinkCharFormat( href );
        } else {
          format = TextFormats::hyperLinkCharFormat( href );
        }
      }
      if ( e.attribute( "bold" ) == "true" ) {
        format.setFontWeight( QFont::Bold );
      }
      if ( e.attribute( "italic" ) == "true" ) {
        format.setFontItalic( true );
      }
      int fontSize = 0;
      if ( e.hasAttribute( "fontsize" ) ) {
        fontSize = e.attribute( "fontsize" ).toInt();
      } else {
        fontSize = 10;
      }
      if ( fontSize > 0 ) format.setFontPointSize( fontSize );
      
      QDomNode n2;
      for( n2 = e.firstChild(); !n2.isNull(); n2 = n2.nextSibling() ) {
//        dbg() << "TICK" << endl;
        QDomText t = n2.toText();
        if ( !t.isNull() ) {
//          dbg() << "TEXT: '" << t.data() << "'" << endl;
          cursor.insertText( t.data(), format );
//          dbg() << "done" << endl;
        } else {
          QDomElement e2 = n2.toElement();
          if ( !e2.isNull() ) {
            if ( e2.tagName() == "todo" ) {
              QTextImageFormat f;
              if ( e2.attribute( "status" ) == "todo" ) {
                f.setName( ":/images/todo.png" );
              } else {
                f.setName( ":/images/tododone.png" );
              }
              cursor.insertImage( f );
            }
          }
        }
      }
    }
  }
}

QString Format::frameToString( QTextFrame *frame )
{
  QString out;

  QTextFrame::iterator it;
  for( it = frame->begin(); it != frame->end(); ++it ) {
    QTextBlock block = it.currentBlock();
    if ( block.isValid() ) {
      out += "<block";

      QTextCursor c( block );

      QDateTime dt = TextFormats::lastModified( c );
      if ( dt.isValid() ) {
        out += " lastmodified=\"" + dt.toString( Qt::ISODate ) + "\"";
      }

      if ( TextFormats::isTitle( c ) ) {
        out += " titlestyle=\"title\"";
      } else if ( TextFormats::isSubTitle( c ) ) {
        out += " titlestyle=\"subtitle\"";
      }

      QTextBlockFormat blockFormat = block.blockFormat();
      if ( blockFormat.isValid() ) {
        QTextList *list = block.textList();
        if ( list ) {
          QTextListFormat f = list->format();
          out += " liststyle=\"";
          switch( f.style() ) {
            default:
            case QTextListFormat::ListDisc:
              out += "disc";
              break;
            case QTextListFormat::ListDecimal:
              out += "decimal";
              break;
          }
          out += "\"";

          out += " listindent=\"" + QString::number( f.indent() ) + "\"";
        } else {
          if ( blockFormat.indent() != 0 ) {
            out += " blockindent=\"" + QString::number( blockFormat.indent() ) +
              "\"";
          }
        }
      }

      out += ">\n";
      
      QTextBlock::iterator it2;
      for( it2 = block.begin(); it2 != block.end(); ++it2 ) {
        QTextFragment fragment = it2.fragment();
        if ( !fragment.isValid() ) continue;

        QString text = fragment.text();

        QString outText;
        for( int i = 0; i < text.size(); ++i ) {
          if ( text.at( i ) == 0xfffc ) {
            outText += "<todo status=\"";

            QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();
            if ( imageFormat.isValid() ) {
              if ( imageFormat.name().contains( "done" ) ) outText += "done";
              else outText += "todo";
            } else {
              dbg() << "NO IMAGE FORMAT" << endl;
            }
            
            outText += "\"/>";
          } else {
            outText += escape( QString( text.at( i ) ) );
          }
        }

        out += "  <fragment";

        QTextCharFormat format = fragment.charFormat();
        if ( !format.anchorHref().isEmpty() ) {
          out += " link=\"" + escape( format.anchorHref() ) + "\"";
        }
        if ( format.fontWeight() == QFont::Bold ) {
          out += " bold=\"true\"";
        }
        if ( format.fontItalic() ) {
          out += " italic=\"true\"";
        }
        if ( format.hasProperty( QTextFormat::FontPointSize ) &&
             format.fontPointSize() != 10 ) {
          out += " fontsize=\"" + QString::number( format.fontPointSize() ) +
            "\"";
        }

        if ( outText.trimmed().isEmpty() ) outText.replace( " ", "[FIXME:space]" );

        out += ">" + outText + "</fragment>\n";
      }
      
      out += "</block>";

      out += "\n";
    }
    QTextFrame *f = it.currentFrame();
    if ( f ) {
      QTextFrameFormat format = f->frameFormat();
      out += "<frame";
      if ( format.hasProperty( TextFormats::FrameType ) ) {
        out += " type=";
        if ( format.property( TextFormats::FrameType ) == TextFormats::CodeFrame ) {
          out += "\"code\"";
        } else {
          out += "\"undefined\"";
        }
      }
      out += ">\n";
      out += frameToString( f );
      out += "</frame>\n";
    }
  }

  return out;
}

QString Format::escape( const QString &str )
{
  QString ret = str;
  
  ret.replace( "&", "&amp;" );
  ret.replace( "<", "&lt;" );
  ret.replace( ">", "&gt;" );

  return ret;
}
