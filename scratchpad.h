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
#ifndef SCRATCHPAD_H
#define SCRATCHPAD_H

#include <QWidget>

class ScratchWidget;

class QPushButton;

/**
  This class provides a scratch pad widget where users can create simple
  drawings and sketches. It includes controls to handle pen attributes.
*/
class ScratchPad : public QWidget
{
    Q_OBJECT
  public:
    /**
      Create scratch pad.
      
      \param parent parent widget
    */
    ScratchPad( QWidget *parent = 0 );

    /**
      Load scratch pad data from file.
      
      \param fielname name of file
    */
    void load( const QString &filename );
    /**
      Load scratch pad data from file.
      
      \param fielname name of file
    */
    void save( const QString &filename );

  public slots:
    /**
      Clear scratch pad.
    */
    void clear();
    void setPencilBlackThick();
    void setPencilBlackThin();
    void setPencilBlueThin();
    void setPencilGreenThin();
    void setPencilRedThin();
    void setPencilRubber();

  protected:
    void unsetOtherButtons( QPushButton * );

  private:
    ScratchWidget *mScratchWidget;

    QPushButton *mButtonBlackThick;
    QPushButton *mButtonBlackThin;
    QPushButton *mButtonBlueThin;
    QPushButton *mButtonGreenThin;
    QPushButton *mButtonRedThin;
    QPushButton *mButtonRubber;
};

#endif
