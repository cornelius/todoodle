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

#include "hypertextedit.h"

#include "dbg.h"
#include "textformats.h"
#include "wordhandler.h"

#include <QMouseEvent>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QToolTip>
#include <QScrollBar>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QDebug>

/**
  This class provides automatic linking to web addresses.
*/
class LinkHandler : public SequenceHandler
{
  public:
    bool process( const QString &word, QTextCursor &cursor )
    {
      if ( !word.startsWith( "www." ) && !word.startsWith( "http://" ) ) {
        return false;
      }

      cursor.mergeCharFormat(
        TextFormats::hyperLinkCharFormat( cursor.selectedText() ) );
    
      return true;
    }
};

/**
  This class provides automatic linking of email addresses.
*/
class EmailHandler : public SequenceHandler
{
  public:
    bool process( const QString &word, QTextCursor &cursor )
    {
      if ( !word.contains( QRegExp( "\\w+@[\\w\\-]+\\.\\w+" ) ) ) return false;

      cursor.mergeCharFormat(
        TextFormats::hyperLinkCharFormat( "mailto:" + cursor.selectedText() ) );

      return true;
    }
};

/**
  This class provides automatic insertion of todo markers.
*/
class TodoHandler : public SequenceHandler
{
  public:
    bool process( const QString &word, QTextCursor &cursor )
    {
      if ( word != "TODO:" && word != "[]" ) return false;

      cursor.removeSelectedText();
      QTextImageFormat f;
      f.setName( ":/images/todo.png" );
      cursor.insertImage( f );

      return true;
    }
};

HyperTextEdit::HyperTextEdit( QWidget *parent )
  : QTextEdit( parent )
{
  viewport()->setMouseTracking( true );
  
  init();
  
//  setAutoFormatting( QTextEdit::AutoAll );

  addHandler( new LinkHandler );
  addHandler( new EmailHandler );
  addHandler( new TodoHandler );
}

HyperTextEdit::~HyperTextEdit()
{
  qDeleteAll( mWordHandlers );
  qDeleteAll( mSequenceHandlers );
}

void HyperTextEdit::init()
{
  QTextFrameFormat f;
  f.setMargin( 8 );
  document()->rootFrame()->setFrameFormat( f );
}

void HyperTextEdit::addHandler( WordHandler *handler )
{
  mWordHandlers.append( handler );
}

void HyperTextEdit::addHandler( SequenceHandler *handler )
{
  mSequenceHandlers.append( handler );
}

bool HyperTextEdit::event( QEvent *ev )
{
  if ( ev->type() == QEvent::ToolTip ) {
    helpEvent( static_cast<QHelpEvent *>( ev ) );
  }
  
  return QTextEdit::event( ev );
}

void HyperTextEdit::helpEvent( QHelpEvent *ev )
{
  QString anchor = anchorAt( ev->pos() );
  if ( !anchor.isEmpty() ) {
    QString text = "This is a tool tip for <b>" + anchor + "</b>.<br/>"
      "<em>Here should be a rich-text preview of the topic being linked to.</em>";
    QToolTip::showText( ev->globalPos(), text, this );
  } else {
    QToolTip::showText( ev->globalPos(), QString::null, this );
  }
}

void HyperTextEdit::mouseMoveEvent( QMouseEvent *ev )
{
  QTextEdit::mouseMoveEvent( ev );

  QString anchor = anchorAt( ev->pos() );
  if ( anchor.isEmpty() ) {
    viewport()->setCursor( Qt::ArrowCursor );
  } else {
    viewport()->setCursor( Qt::PointingHandCursor );
  }
}

void HyperTextEdit::mouseReleaseEvent( QMouseEvent *ev )
{
  QTextEdit::mouseReleaseEvent( ev );

  QString anchor = anchorAt( ev->pos() );
  if ( !anchor.isEmpty() ) {
    emit anchorClicked( anchor );
  }

  int position = document()->documentLayout()->hitTest( ev->pos(),
    Qt::ExactHit );

  if ( position >= 0 ) {
    QTextCursor cursor( document() );
    cursor.setPosition( position );

    QTextCharFormat f = cursor.charFormat();
    if ( f.isImageFormat() ) {
      dbg() << "Clicked image" << endl;

      cursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor );
    
      QTextImageFormat imageFormat = f.toImageFormat();
      if ( imageFormat.name() == ":/images/todo.png" ) {
        dbg() << "DONE" << endl;
        imageFormat.setName( ":/images/tododone.png" );
      } else {
        dbg() << "TODO" << endl;
        imageFormat.setName( ":/images/todo.png" );
      }
      cursor.setCharFormat( imageFormat );
    }
  }
}

void HyperTextEdit::keyPressEvent( QKeyEvent *ev )
{
  int key = ev->key();

  QTextCursor cursor = textCursor();

  if ( ev->modifiers() == Qt::ControlModifier ) {
    if ( key == Qt::Key_Up ) {
      updent();
      return;
    } else if ( key == Qt::Key_Down ) {
      downdent();
      return;
    } else if ( key == Qt::Key_Right ) {
      indent();
      return;
    } else if ( key == Qt::Key_Left ) {
      outdent();
      return;
    } else if ( key == Qt::Key_Backspace ) {
      removeBlock();
      return;
    }
  }

  if ( key == Qt::Key_Return && ev->modifiers() == Qt::ControlModifier ) {
    QTextCharFormat f = cursor.charFormat();
    if ( f.isAnchor() && !f.anchorHref().isEmpty() ) {
      emit anchorClicked( f.anchorHref() );
      return;
    }
  }

  if ( ( ev->text().startsWith( "-" ) || ev->text().startsWith( "*" ) ) &&
       cursor.atBlockStart() && !cursor.currentList() ) {
    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    QTextListFormat listFmt;
    listFmt.setStyle( QTextListFormat::ListDisc );
    listFmt.setIndent( blockFmt.indent() + 1 );

    blockFmt.setIndent( 0 );
    cursor.setBlockFormat( blockFmt );

    cursor.createList( listFmt );

    cursor.endEditBlock();

    cursor.insertText( ev->text().mid( 1 ) );

    return;
  }

  if ( ev->text().startsWith( "!" ) && cursor.atBlockStart() ) {
    cursor.insertImage( ":/images/todo.png" );
    cursor.insertText( ev->text().mid( 1 ) );
    
    return;
  }

  if ( key == Qt::Key_Space || key == Qt::Key_Return ) {
    if ( TextFormats::isHyperLink( cursor ) ||
         TextFormats::isTodoodleLink( cursor ) ) {
      cursor.setCharFormat( TextFormats::normalCharFormat() );
    }
  }

  bool hasList = cursor.currentList();

  setTextCursor( cursor );

  if ( key == Qt::Key_Return ) {
    QTextList *list = cursor.currentList();
    if ( list ) {
      QTextListFormat listFormat = list->format();
      cursor.insertList( listFormat );
    } else {
      QTextBlockFormat blockFormat = cursor.blockFormat();
      if ( TextFormats::isTitle( cursor ) ||
           TextFormats::isSubTitle( cursor ) ) {
        cursor.insertBlock( TextFormats::normalBlockFormat() );
        cursor.setCharFormat( TextFormats::normalCharFormat() );
        cursor.setBlockCharFormat( TextFormats::normalCharFormat() );
      } else {
        cursor.insertBlock( blockFormat );
      }

      QTextCursor previous( cursor );
      previous.movePosition( QTextCursor::PreviousBlock );
      if ( previous != cursor ) {
        if ( TextFormats::isTitle( previous ) ||
             TextFormats::isSubTitle( previous ) ) {
          cursor.setCharFormat( TextFormats::normalCharFormat() );
          cursor.setBlockFormat( TextFormats::normalBlockFormat() );
          cursor.setBlockCharFormat( TextFormats::normalCharFormat() );
        }
      
        previous.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor );

  //      dbg() << "PREVIOUS BLOCK: '" << c.selectedText() << "'" << endl;
  //      dbg() << "CHAR: " << c.selectedText().at( 0 ).unicode() << endl;

        if ( previous.selectedText().size() == 1 ) {
          if ( previous.selectedText().at( 0 ) ==
               QChar::ObjectReplacementCharacter ) {
            cursor.insertImage( ":/images/todo.png" );
          }
        }
      }
    }
    return;
  }
  
  QTextEdit::keyPressEvent( ev );

  cursor = textCursor();

  if ( cursor.hasSelection() ) return;

  QTextBlock block = cursor.block();

  QString text = block.text();

  QTextCursor wordCursor( cursor );
  wordCursor.movePosition( QTextCursor::StartOfWord );
  wordCursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );
  if ( wordCursor.hasSelection() ) {
    QString word = wordCursor.selectedText();

//    dbg() << "WORD: " << word << endl;

    QTextCharFormat currentFormat = currentCharFormat();

    // hide the magic highlighting from the user visible undo by
    // putting it into existing undo blocks
    wordCursor.joinPreviousEditBlock();

    foreach( WordHandler *handler, mWordHandlers ) {
      if ( handler->process( word, wordCursor ) ) break;
    }

    wordCursor.endEditBlock();

    setCurrentCharFormat( currentFormat );
  }

  int sequenceEnd = cursor.position();  
  QTextCursor sequenceCursor( cursor );
  sequenceCursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor );
  if ( sequenceCursor.selectedText() == " " ) {
    sequenceEnd = sequenceCursor.position();
  }
  sequenceCursor.setPosition( sequenceEnd );
  while( !sequenceCursor.atBlockStart() ) {
    sequenceCursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor );
    if ( sequenceCursor.selectedText().contains( QRegExp( "^\\s" ) ) ) {
      sequenceCursor.movePosition( QTextCursor::Right );
      break;
    }
  }
  sequenceCursor.clearSelection();
  sequenceCursor.setPosition( sequenceEnd, QTextCursor::KeepAnchor );
  while( !sequenceCursor.atBlockEnd() ) {
    sequenceCursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor );
    if ( sequenceCursor.selectedText().contains( QRegExp( "\\s$" ) ) ) {
      sequenceCursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor );
      break;
    }
  }
  if ( sequenceCursor.hasSelection() ) {
    QString sequence = sequenceCursor.selectedText();

//    dbg() << "SEQUENCE: " << sequence << endl;

    QTextCharFormat currentFormat = currentCharFormat();

    // hide the magic highlighting from the user visible undo by
    // putting it into existing undo blocks
    sequenceCursor.joinPreviousEditBlock();

    foreach( SequenceHandler *handler, mSequenceHandlers ) {
      if ( handler->process( sequence, sequenceCursor ) ) break;
    }

    sequenceCursor.endEditBlock();

    setCurrentCharFormat( currentFormat );
  }
  
  if ( key == Qt::Key_Backspace ) {
    if ( cursor.atBlockStart() && hasList && !cursor.currentList() ) {
      outdent();
    }
  }

  if ( !ev->text().isEmpty() ) {
    TextFormats::setLastModified( cursor );
  }
  
  setTextCursor( cursor );
}

QTextCursor getIncludingBlocks( const QTextCursor cursor )
{
  int pos = cursor.position();
  int anchor = cursor.anchor();

  int selectionStart, selectionEnd;
  if ( pos < anchor ) {
    selectionStart = pos;
    selectionEnd = anchor;
  } else {
    selectionStart = anchor;
    selectionEnd = pos;
  }

  QTextCursor moveCursor( cursor );

  // FIXME: The complicated cursor movement reflects the fact that to keep
  // list formatting, it's not enough to copy the text from start to end
  // of a block, but the preceding return also has to be copied. This is
  // a strange behavior of Qt, if not a bug.
  // simon: Solved in Qt 4.1 :-)
  // cs: Doesn't seem to be solved when selecting multiple blocks.
  moveCursor.setPosition( selectionStart );
  moveCursor.movePosition( QTextCursor::PreviousBlock );
  moveCursor.movePosition( QTextCursor::EndOfBlock );
  moveCursor.setPosition( selectionEnd, QTextCursor::KeepAnchor );
  moveCursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );

  return moveCursor;
}

void HyperTextEdit::updent()
{
  QTextCursor cursor = textCursor();

  if ( cursor.hasSelection() ) {
    int pos = cursor.position();
    int anchor = cursor.anchor();

    QTextCursor moveCursor = getIncludingBlocks( cursor );

    QTextCursor insertCursor( cursor );
    insertCursor.setPosition( moveCursor.anchor() );
    insertCursor.movePosition( QTextCursor::PreviousBlock );
    insertCursor.movePosition( QTextCursor::EndOfBlock );

    int offset = insertCursor.anchor() - moveCursor.anchor();

    QTextDocumentFragment f( moveCursor );
    moveCursor.removeSelectedText();
    insertCursor.insertFragment( f );

    insertCursor.setPosition( anchor + offset );
    insertCursor.setPosition( pos + offset, QTextCursor::KeepAnchor );

    setTextCursor( insertCursor );
  } else {
    QTextCursor moveCursor( cursor );
    int pos = moveCursor.position();
    moveCursor.movePosition( QTextCursor::StartOfBlock );
    int offset = pos - moveCursor.position();
    moveCursor.select(QTextCursor::BlockUnderCursor);

    QTextCursor insertCursor( cursor );
    insertCursor.movePosition( QTextCursor::PreviousBlock );
    insertCursor.movePosition( QTextCursor::PreviousBlock );
    insertCursor.movePosition( QTextCursor::EndOfBlock );

    QTextDocumentFragment f( moveCursor );
    moveCursor.removeSelectedText();
    insertCursor.insertFragment( f );

    insertCursor.movePosition( QTextCursor::StartOfBlock );
    insertCursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor,
      offset );

    setTextCursor( insertCursor );
  }
}

void HyperTextEdit::downdent()
{
  QTextCursor cursor = textCursor();

  if ( cursor.hasSelection() ) {
    int pos = cursor.position();
    int anchor = cursor.anchor();

    QTextCursor moveCursor = getIncludingBlocks( cursor );

    QTextCursor insertCursor( moveCursor );
    insertCursor.movePosition( QTextCursor::NextBlock );
    insertCursor.movePosition( QTextCursor::EndOfBlock );

    int offset = insertCursor.position() - moveCursor.position();

    QTextDocumentFragment f( moveCursor );
    moveCursor.removeSelectedText();
    insertCursor.insertFragment( f );

    insertCursor.setPosition( anchor + offset );
    insertCursor.setPosition( pos + offset, QTextCursor::KeepAnchor );

    setTextCursor( insertCursor );
  } else {
    QTextCursor moveCursor( cursor );
    int pos = moveCursor.position();
    moveCursor.movePosition( QTextCursor::StartOfBlock );
    int offset = pos - moveCursor.position();
    moveCursor.select( QTextCursor::BlockUnderCursor );

    QTextCursor insertCursor( moveCursor );
    insertCursor.movePosition( QTextCursor::NextBlock );
    insertCursor.movePosition( QTextCursor::EndOfBlock );

    QTextDocumentFragment f( moveCursor );
    insertCursor.insertFragment( f );
    moveCursor.removeSelectedText();

    moveCursor.movePosition( QTextCursor::NextBlock );
    moveCursor.movePosition( QTextCursor::NextBlock );
    moveCursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor,
      offset );

    setTextCursor( moveCursor );
  }
}

void HyperTextEdit::indent()
{
  QTextCursor cursor = textCursor();

  QTextBlockFormat blockFmt = cursor.blockFormat();

  QTextList *list = cursor.currentList();
  if ( list ) {
    QTextListFormat format = list->format();
    format.setIndent(format.indent() + 1);

    if (list->itemNumber(cursor.block()) == 1)
      list->setFormat(format);
    else
      cursor.createList(format);
  } else {
    QTextBlockFormat modifier;
    modifier.setIndent(blockFmt.indent() + 1);
    cursor.mergeBlockFormat(modifier);
  }
}

void HyperTextEdit::outdent()
{
  QTextCursor cursor = textCursor();

  QTextBlockFormat blockFmt = cursor.blockFormat();

  QTextList *list = cursor.currentList();

  if ( list ) {
    QTextListFormat listFmt = list->format();
    listFmt.setIndent(listFmt.indent() - 1);
    list->setFormat(listFmt);
  } else {
    QTextBlockFormat modifier;
    modifier.setIndent(blockFmt.indent() - 1);
    cursor.mergeBlockFormat(modifier);
  }
}

void HyperTextEdit::removeBlock()
{
  QTextCursor beginCursor = textCursor();
  beginCursor.movePosition( QTextCursor::PreviousBlock );
  beginCursor.movePosition( QTextCursor::EndOfBlock );
  
  QTextCursor endCursor = textCursor();
  endCursor.movePosition( QTextCursor::EndOfBlock );
  
  beginCursor.setPosition( endCursor.position(), QTextCursor::KeepAnchor );
  
  beginCursor.removeSelectedText();
}
