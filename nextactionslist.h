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
#ifndef NEXTACTIONSLIST_H
#define NEXTACTIONSLIST_H

#include <QWidget>

class QTextEdit;
class HyperTextEdit;

/**
  This class provides a view on "next actions" extracted from topic data.

  It's work in progress.
*/
class NextActionsList : public QWidget
{
    Q_OBJECT
  public:
    NextActionsList();

    void addEditor( HyperTextEdit * );

  public slots:
    void refreshList();

  private:
    QTextEdit *mTextEdit;
    
    QList<HyperTextEdit *> mEditors;
};

#endif
