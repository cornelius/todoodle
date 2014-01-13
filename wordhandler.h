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
#ifndef WORDHANLDER_H
#define WORDHANLDER_H

#include <QTextCursor>

/**
  This class provides an interface for handlers working on words. A word is a
  consecutive sequence of alphanumeric characters. The HyperTextEdit class uses
  word handlers to process input and possibly manipulate it while the user is
  typing.
*/
class WordHandler
{
  public:
    virtual ~WordHandler() {}

    /**
      Process the word the cursor is positioned at. The selection of the cursor
      spans the word to be processed.
      
      Return true, if the handler processed the word, returns false if the
      handler left the word alone.
    */
    virtual bool process( const QString &word, QTextCursor & ) = 0;
};

/**
  This class provides an interface for handlers working on sequences. A sequence
  is a consecutive sequence of non-whitespace characters. The HyperTextEdit
  class uses sequence handlers to process input and possibly manipulate it while
  the user is typing.
*/
class SequenceHandler
{
  public:
    virtual ~SequenceHandler() {}

    /**
      Process the sequence the cursor is positioned at. The selection of the
      cursor spans the sequence to be processed.
      
      Return true, if the handler processed the sequence, returns false if the
      handler left the sequence alone.
    */
    virtual bool process( const QString &word, QTextCursor & ) = 0;    
};

#endif
