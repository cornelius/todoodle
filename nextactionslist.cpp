/*
    This file is part of Todoodle.

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

#include "nextactionslist.h"

#include "hypertextedit.h"
#include "dbg.h"

#include <QTextEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocumentFragment>

NextActionsList::NextActionsList()
  : QWidget( 0 )
{
  QBoxLayout *topLayout = new QVBoxLayout( this );
  
  mTextEdit = new QTextEdit( this );
  topLayout->addWidget( mTextEdit );
  mTextEdit->setReadOnly( true );

  QBoxLayout *buttonLayout = new QHBoxLayout;
  topLayout->addLayout( buttonLayout );

  buttonLayout->addStretch( 1 );

  QPushButton *button = new QPushButton( "Refresh", this );
  buttonLayout->addWidget( button );
  connect( button, SIGNAL( clicked() ), SLOT( refreshList() ) );
  
  button = new QPushButton( "Close", this );
  buttonLayout->addWidget( button );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
}

void NextActionsList::addEditor( HyperTextEdit *editor )
{
  mEditors.append( editor );
}

void NextActionsList::refreshList()
{
  mTextEdit->clear();

  foreach( HyperTextEdit *editor, mEditors ) {
    QTextDocument *doc = editor->document();
    QTextBlock firstBlock = doc->begin();
    QTextCursor cursor( doc );
    cursor.setPosition( firstBlock.position() );
    cursor.select( QTextCursor::BlockUnderCursor );
    mTextEdit->textCursor().insertFragment( QTextDocumentFragment( cursor ) );
    
    QTextBlock block;
    for( block = firstBlock.next(); block.isValid(); block = block.next() ) {
      QString text = block.text();
      if ( text.contains( 0xfffc ) ) {
        cursor.setPosition( block.position() );
        cursor.select( QTextCursor::BlockUnderCursor );
        mTextEdit->textCursor().insertFragment(
          QTextDocumentFragment( cursor ) );
      }
    }
  }
}
