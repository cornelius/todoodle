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

#include "scratchwidget.h"

#include "scratchpad.h"
#include "dbg.h"

#include "qlayout.h"

#include <QPushButton>

ScratchPad::ScratchPad( QWidget *parent )
  : QWidget( parent )
{
  QBoxLayout *topLayout = new QVBoxLayout( this );
  topLayout->setSpacing( 0 );
  topLayout->setMargin( 0 );
  
  QBoxLayout *buttonLayout = new QHBoxLayout;
  topLayout->addLayout( buttonLayout );
  
  QPushButton *button;
  
  int l = 24;
  
  button = new QPushButton(
    QIcon( QPixmap( ":/images/pencil_black_thick.png" ) ), "", this );
  buttonLayout->addWidget( button );
  button->setCheckable( true );
  button->setFixedSize( l, l );
  connect( button, SIGNAL( clicked() ), SLOT( setPencilBlackThick() ) );
  mButtonBlackThick = button;

  button = new QPushButton(
    QIcon( QPixmap( ":/images/pencil_black_thin.png" ) ), "", this );
  buttonLayout->addWidget( button );
  button->setCheckable( true );
  button->setFixedSize( l, l );
  connect( button, SIGNAL( clicked() ), SLOT( setPencilBlackThin() ) );
  mButtonBlackThin = button;

  button = new QPushButton(
    QIcon( QPixmap( ":/images/pencil_blue_thin.png" ) ), "", this );
  buttonLayout->addWidget( button );
  button->setCheckable( true );
  button->setFixedSize( l, l );
  connect( button, SIGNAL( clicked() ), SLOT( setPencilBlueThin() ) );
  mButtonBlueThin = button;

  button = new QPushButton(
    QIcon( QPixmap( ":/images/pencil_red_thin.png" ) ), "", this );
  buttonLayout->addWidget( button );
  button->setCheckable( true );
  button->setFixedSize( l, l );
  connect( button, SIGNAL( clicked() ), SLOT( setPencilRedThin() ) );
  mButtonRedThin = button;

  button = new QPushButton(
    QIcon( QPixmap( ":/images/pencil_green_thin.png" ) ), "", this );
  buttonLayout->addWidget( button );
  button->setCheckable( true );
  button->setFixedSize( l, l );
  connect( button, SIGNAL( clicked() ), SLOT( setPencilGreenThin() ) );
  mButtonGreenThin = button;

  button = new QPushButton(
    QIcon( QPixmap( ":/images/pencil_rubber.png" ) ), "", this );
  buttonLayout->addWidget( button );
  button->setCheckable( true );
  button->setFixedSize( l, l );
  connect( button, SIGNAL( clicked() ), SLOT( setPencilRubber() ) );
  mButtonRubber = button;

  buttonLayout->addSpacing( 4 );

  button = new QPushButton( "Clear", this );
  buttonLayout->addWidget( button );
  connect( button, SIGNAL( clicked() ), SLOT( clear() ) );
  
  
  mScratchWidget = new ScratchWidget( this );
  topLayout->addWidget( mScratchWidget );

  mButtonBlackThin->setChecked( true );
  setPencilBlackThin();
}

void ScratchPad::load( const QString &filename )
{
  mScratchWidget->load( filename );
}

void ScratchPad::save( const QString &filename )
{
  mScratchWidget->save( filename );
}

void ScratchPad::clear()
{
  mScratchWidget->clearScreen();
}

void ScratchPad::setPencilBlackThick()
{
  mScratchWidget->setPenWidth( 5 );
  mScratchWidget->setPenColor( Qt::black );

  unsetOtherButtons( mButtonBlackThick );
}

void ScratchPad::setPencilBlackThin()
{
  mScratchWidget->setPenWidth( 2 );
  mScratchWidget->setPenColor( Qt::black );

  unsetOtherButtons( mButtonBlackThin );
}

void ScratchPad::setPencilBlueThin()
{
  mScratchWidget->setPenWidth( 2 );
  mScratchWidget->setPenColor( Qt::blue );

  unsetOtherButtons( mButtonBlueThin );
}

void ScratchPad::setPencilGreenThin()
{
  mScratchWidget->setPenWidth( 2 );
  mScratchWidget->setPenColor( Qt::green );

  unsetOtherButtons( mButtonGreenThin );
}

void ScratchPad::setPencilRedThin()
{
  mScratchWidget->setPenWidth( 2 );
  mScratchWidget->setPenColor( Qt::red );

  unsetOtherButtons( mButtonRedThin );
}

void ScratchPad::setPencilRubber()
{
  mScratchWidget->setPenWidth( 10 );
  mScratchWidget->setPenColor( Qt::white );

  unsetOtherButtons( mButtonRubber );
}

void ScratchPad::unsetOtherButtons( QPushButton *button )
{
  if ( button != mButtonBlackThick ) mButtonBlackThick->setChecked( false );
  if ( button != mButtonBlackThin ) mButtonBlackThin->setChecked( false );
  if ( button != mButtonBlueThin ) mButtonBlueThin->setChecked( false );
  if ( button != mButtonGreenThin ) mButtonGreenThin->setChecked( false );
  if ( button != mButtonRedThin ) mButtonRedThin->setChecked( false );
  if ( button != mButtonRubber ) mButtonRubber->setChecked( false );
}
