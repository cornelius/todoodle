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

#include "todoodle.h"

#include "hypertextedit.h"
#include "dbg.h"
#include "topicmanager.h"
#include "topiclist.h"
#include "topicmap.h"
#include "scratchpad.h"
#include "textformats.h"
#include "wordhandler.h"
#include "formatplaintext.h"
#include "prefs.h"

#include <qaction.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qfontdatabase.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qpainter.h>
#include <qprintdialog.h>
#include <qprinter.h>
#include <qtabwidget.h>
#include <qtextcodec.h>
#include <qtextedit.h>
#include <qtextdocumentfragment.h>
#include <qtextformat.h>
#include <qtoolbar.h>
#include <QMessageBox>
#include <QTextStream>
#include <QTextFrame>
#include <QTextCursor>
#include <QTextList>
#include <QInputDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QSplitter>
#include <QTimer>
#include <QProcess>
#include <QDateTime>

#include <limits.h>

/**
  This class provides automatic linking to other topics. If a typed word is the
  name of a known topic it's converted into a link to that topic.
*/
class TopicHandler : public WordHandler
{
  public:
    TopicHandler( TopicManager *topicManager )
      : mTopicManager( topicManager )
    {
    }

    bool process( const QString &word, QTextCursor &cursor )
    {
      if ( !mTopicManager->topics().contains( word ) ) return false;

      cursor.mergeCharFormat( TextFormats::topicLinkCharFormat( word ) );

      return true;
    }

  private:
    TopicManager *mTopicManager;
};


Todoodle::Todoodle( TopicManager *topicManager,
  QWidget *parent )
  : QMainWindow( parent ), mTopicManager( topicManager )
{
  qDebug( "Todoodle() %s", mTopic.toUtf8().constData() );

  setAttribute( Qt::WA_DeleteOnClose, true );

  mToolBar = new QToolBar( this );
  mToolBar->setWindowTitle( tr("File Actions") );
  addToolBar( mToolBar );

  setupFileActions();
  setupEditActions();
  setupNavigationActions();
  setupTextActions();
  setupInsertActions();
  setupHelpActions();

  mSplitter = new QSplitter( Qt::Horizontal, this );
  setCentralWidget( mSplitter );

  mEditor = new HyperTextEdit( mSplitter );
  mEditor->setFocus();
  connect( mEditor, SIGNAL( anchorClicked( const QString & ) ),
    SLOT( slotAnchorClicked( const QString & ) ) );

  mEditor->addHandler( new TopicHandler( mTopicManager ) );

  mScratchPad = new ScratchPad( mSplitter );
  mScratchPad->hide();

  connect( mEditor->document(), SIGNAL( undoAvailable( bool ) ),
    actionUndo, SLOT( setEnabled( bool ) ) );
  connect( mEditor->document(), SIGNAL( redoAvailable( bool ) ),
    actionRedo, SLOT( setEnabled( bool ) ) );

  actionUndo->setEnabled( mEditor->document()->isUndoAvailable() );
  actionRedo->setEnabled( mEditor->document()->isRedoAvailable() );

  connect( actionUndo, SIGNAL( triggered() ),
    mEditor->document(), SLOT( undo() ) );
  connect( actionRedo, SIGNAL( triggered() ),
    mEditor->document(), SLOT( redo() ) );

  connect( actionCut, SIGNAL( triggered() ),
    mEditor, SLOT( cut() ) );
  connect( actionCopy, SIGNAL( triggered() ),
    mEditor, SLOT( copy() ) );
  connect( actionPaste, SIGNAL( triggered() ),
    mEditor, SLOT( paste() ) );

  connect( mEditor, SIGNAL( copyAvailable( bool ) ),
    actionCopy, SLOT( setEnabled( bool ) ) );
  connect( mEditor, SIGNAL( copyAvailable( bool ) ),
    actionCut, SLOT( setEnabled( bool ) ) );

  connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
    this, SLOT( clipboardDataChanged() ) );

  resize( 300, 450 );
}

Todoodle::~Todoodle()
{
  dbg() << "~Todoodle() " << mTopic << endl;

  if ( mTopicManager->windowMode() == TopicManager::Multiple ) {
    mTopicManager->prefs()->setStartTopic( mTopic );
  }
  
  mTopicManager->removeEditor( mTopic );

  dbg() << "~Todoodle() done " << mTopic << endl;
}

void Todoodle::closeEvent( QCloseEvent *e )
{
  dbg() << "Todoodle::closeEvent" << endl;

  writeConfig();
  saveTopic();

  e->accept();
}

void Todoodle::setupFileActions()
{
  QMenu *menu = new QMenu(tr("&Topic"), this);
  menuBar()->addMenu(menu);

  QAction *a;

  a = new QAction(QPixmap(":/images/filenew.png"), tr("&Clear"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
  menu->addAction(a);

  a = new QAction( "Link", this );
  a->setShortcut(Qt::CTRL + Qt::Key_H);
  connect( a, SIGNAL( triggered() ), SLOT( fileLink() ) );
  menu->addAction( a );

  a = new QAction( "Extract Topic", this );
  a->setShortcut(Qt::CTRL + Qt::Key_E);
  connect( a, SIGNAL( triggered() ), SLOT( fileExtract() ) );
  menu->addAction( a );

  a = new QAction( "Inline Topic", this );
  a->setShortcut(Qt::CTRL + Qt::Key_L);
  connect( a, SIGNAL( triggered() ), SLOT( fileInline() ) );
  menu->addAction( a );

  menu->addSeparator();

  a = new QAction(QPixmap(":/images/fileprint.png"),
    tr("Print..."), this);
  a->setShortcut(Qt::CTRL + Qt::Key_P);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
  menu->addAction(a);

#if 0
  a = new QAction( tr("&Dump Structure (debug)"), this );
  connect( a, SIGNAL( triggered() ), this, SLOT( dumpStructure() ) );
  menu->addAction( a );
#endif

  menu->addSeparator();

  a = new QAction( "Export to HTML...", this );
  connect( a, SIGNAL( triggered() ), SLOT( exportHtml() ) );
  menu->addAction( a );

  a = new QAction( "Export to Plain Text...", this );
  connect( a, SIGNAL( triggered() ), SLOT( exportPlainText() ) );
  menu->addAction( a );

  menu->addSeparator();

  a = new QAction( "Topic List...", this );
  connect( a, SIGNAL( triggered() ), SLOT( showTopicList() ) );
  menu->addAction( a );

  a = new QAction( "Topic Map...", this );
  connect( a, SIGNAL( triggered() ), SLOT( showTopicMap() ) );
  menu->addAction( a );

  a = new QAction( "Next Actions List...", this );
  connect( a, SIGNAL( triggered() ), SLOT( showNextActionsList() ) );
  menu->addAction( a );

  menu->addSeparator();

  mActionScratchPad = new QAction( QPixmap( ":/images/scratchpad.png" ), "Scratch Pad", this );
  connect( mActionScratchPad, SIGNAL( triggered() ), SLOT( showScratchPad() ) );
  mActionScratchPad->setCheckable( true );
  menu->addAction( mActionScratchPad );
  mToolBar->addAction( mActionScratchPad );
  
  menu->addSeparator();

  a = new QAction(tr("&Close"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_W);
  connect(a, SIGNAL(triggered()), this, SLOT(fileClose()));
  menu->addAction(a);

  a = new QAction(tr("E&xit"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_Q);
  connect(a, SIGNAL(triggered()), this, SLOT(fileExit()));
  menu->addAction(a);
}

void Todoodle::setupEditActions()
{
  QToolBar *tb = new QToolBar(this);
  tb->setWindowTitle(tr("Edit Actions"));
  addToolBar(tb);

  QMenu *menu = new QMenu(tr("&Edit"), this);
  menuBar()->addMenu(menu);

  QAction *a;
  a = actionUndo = new QAction(QPixmap(":/images/editundo.png"), tr("&Undo"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_Z);
  tb->addAction(a);
  menu->addAction(a);
  a = actionRedo = new QAction(QPixmap(":/images/editredo.png"), tr("&Redo"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_Y);
  tb->addAction(a);
  menu->addAction(a);
  menu->addSeparator();
  a = actionCut = new QAction(QPixmap(":/images/editcut.png"), tr("Cu&t"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_X);
  tb->addAction(a);
  menu->addAction(a);
  a = actionCopy = new QAction(QPixmap(":/images/editcopy.png"), tr("&Copy"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_C);
  tb->addAction(a);
  menu->addAction(a);
  a = actionPaste = new QAction(QPixmap(":/images/editpaste.png"), tr("&Paste"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_V);
  tb->addAction(a);
  menu->addAction(a);
  actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void Todoodle::setupTextActions()
{
  QToolBar *tb = new QToolBar(this);
  tb->setWindowTitle(tr("Format Actions"));
  addToolBar(tb);

  QMenu *menu = new QMenu(tr("F&ormat"), this);
  menuBar()->addMenu(menu);

#if 0
  comboStyle = new QComboBox(tb);
  tb->addWidget(comboStyle);
  comboStyle->insertItem("Standard");
  comboStyle->insertItem("Bullet List (Disc)");
  comboStyle->insertItem("Bullet List (Circle)");
  comboStyle->insertItem("Bullet List (Square)");
  comboStyle->insertItem("Ordered List (Decimal)");
  comboStyle->insertItem("Ordered List (Alpha lower)");
  comboStyle->insertItem("Ordered List (Alpha upper)");
  connect(comboStyle, SIGNAL(activated(int)),
          this, SLOT(textStyle(int)));

  comboFont = new QComboBox(tb);
  tb->addWidget(comboFont);
  comboFont->setEditable(true);
  QFontDatabase db;
  comboFont->insertStringList(db.families());
  connect(comboFont, SIGNAL(activated(const QString &)),
          this, SLOT(textFamily(const QString &)));
  comboFont->setCurrentText(QApplication::font().family());

  comboSize = new QComboBox(tb);
  tb->addWidget(comboSize);
  comboSize->setEditable(true);

  foreach(int size, db.standardSizes())
      comboSize->insertItem(QString::number(size));

  connect(comboSize, SIGNAL(activated(const QString &)),
          this, SLOT(textSize(const QString &)));
  comboSize->setCurrentText(QString::number(QApplication::font().pointSize()));
#endif

  QAction *a;
  
  a = new QAction( "Title", this );
  connect( a, SIGNAL( triggered() ), SLOT( textTitle() ) );
  a->setShortcut( Qt::CTRL + Qt::Key_T );
  menu->addAction( a );

  a = new QAction( "Subtitle", this );
  connect( a, SIGNAL( triggered() ), SLOT( textSubtitle() ) );
  a->setShortcut( Qt::CTRL + Qt::Key_S );
  menu->addAction( a );

  a = new QAction( "Normal", this );
  connect( a, SIGNAL( triggered() ), SLOT( textNormal() ) );
  a->setShortcut( Qt::CTRL + Qt::Key_N );
  menu->addAction( a );

  actionTextBold = new QAction(QPixmap(":/images/textbold.png"), tr("&Bold"), this);
  actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
  QFont bold;
  bold.setBold(true);
  actionTextBold->setFont(bold);
  connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
  tb->addAction(actionTextBold);
  menu->addAction(actionTextBold);
  actionTextBold->setCheckable(true);

  actionTextItalic = new QAction(QPixmap(":/images/textitalic.png"), tr("&Italic"), this);
  actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
  QFont italic;
  italic.setItalic(true);
  actionTextItalic->setFont(italic);
  connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
  tb->addAction(actionTextItalic);
  menu->addAction(actionTextItalic);
  actionTextItalic->setCheckable(true);

#if 0
  actionTextUnderline = new QAction(QPixmap(":/images/textunder.png"), tr("&Underline"), this);
  actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
  QFont underline;
  underline.setUnderline(true);
  actionTextUnderline->setFont(underline);
  connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
  tb->addAction(actionTextUnderline);
  menu->addAction(actionTextUnderline);
  actionTextUnderline->setCheckable(true);

  menu->addSeparator();

  QActionGroup *grp = new QActionGroup(this);
  connect(grp, SIGNAL(triggered(QAction *)), this, SLOT(textAlign(QAction *)));

  actionAlignLeft = new QAction(QPixmap(":/images/textleft.png"), tr("&Left"), grp);
  actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
  actionAlignLeft->setCheckable(true);
  actionAlignCenter = new QAction(QPixmap(":/images/textcenter.png"), tr("C&enter"), grp);
  actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
  actionAlignCenter->setCheckable(true);
  actionAlignRight = new QAction(QPixmap(":/images/textright.png"), tr("&Right"), grp);
  actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
  actionAlignRight->setCheckable(true);
  actionAlignJustify = new QAction(QPixmap(":/images/textjustify.png"), tr("&Justify"), grp);
  actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
  actionAlignJustify->setCheckable(true);

  tb->addActions(grp->actions());
  menu->addActions(grp->actions());

  menu->addSeparator();

  QPixmap pix(16, 16);
  pix.fill(Qt::black);
  actionTextColor = new QAction(pix, tr("&Color..."), this);
  connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
  tb->addAction(actionTextColor);
  menu->addAction(actionTextColor);
#endif
}

void Todoodle::setupInsertActions()
{
  QMenu *menu = new QMenu( tr("Insert"), this );
  menuBar()->addMenu( menu );

  QAction *a;

  a = new QAction( "Bullet List", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertListDisc() ) );
  menu->addAction( a );

#if 0
  a = new QAction( "Numbered List", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertListDecimal() ) );
  menu->addAction( a );
#endif

  a = new QAction( "Todo", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertTodo() ) );
  menu->addAction( a );

#if 0
  a = new QAction( "Frame", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertFrame() ) );
  menu->addAction( a );

  a = new QAction( "Table", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertTable() ) );
  menu->addAction( a );
  
  a = new QAction( "Link", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertLink() ) );
  menu->addAction( a );
#endif

  a = new QAction( "Current Date", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertCurrentDate() ) );
  menu->addAction( a );

  a = new QAction( "Code Snippet", this );
  connect( a, SIGNAL( triggered() ), SLOT( insertCode() ) );
  menu->addAction( a );
}

void Todoodle::setupNavigationActions()
{
  QMenu *menu = new QMenu( tr("Go"), this );
  menuBar()->addMenu( menu );

  QAction *a;

  a = new QAction( QPixmap(":/images/gohome.png"), "Start Page", this );
  connect( a, SIGNAL( triggered() ), SLOT( goStart() ) );
  menu->addAction( a );
  mToolBar->addAction( a );

#if 0  
  a = new QAction( "Back", this );
  connect( a, SIGNAL( triggered() ), SLOT( goBack() ) );
  menu->addAction( a );

  a = new QAction( "Forward", this );
  connect( a, SIGNAL( triggered() ), SLOT( goForward() ) );
  menu->addAction( a );
#endif
}

void Todoodle::setupHelpActions()
{
  QMenu *menu = new QMenu( tr("Help"), this );
  menuBar()->addMenu( menu );
  
  QAction *a;
  
  a = new QAction( "Manual", this );
  connect( a, SIGNAL( triggered() ), SLOT( goManual() ) );
  menu->addAction( a );
}

void Todoodle::goStart()
{
  openTopic( "Start" );
}

void Todoodle::goManual()
{
  openTopic("Manual");
}

void Todoodle::goBack()
{
}

void Todoodle::goForward()
{
}

void Todoodle::fileNew()
{
  mFilename.clear();
  mEditor->clear();
}

void Todoodle::fileLink()
{
  QTextCursor cursor = mEditor->textCursor();
  
  if ( !cursor.hasSelection() ) {
    QMessageBox::warning( this, "No Selection", "There is no text selected" );
  } else {
    QString topic = cursor.selectedText();
  
//    QMessageBox::information( this, "Selected Text", topic );
    QTextCharFormat format = TextFormats::topicLinkCharFormat( topic );
    cursor.mergeCharFormat( format );
  
    openTopic( topic );

    cursor.clearSelection();
  }
}

QTextCursor Todoodle::textCursor()
{
  return mEditor->textCursor();
}

void Todoodle::fileExtract()
{
  QTextCursor cursor = mEditor->textCursor();
  
  if ( !cursor.hasSelection() ) {
    QMessageBox::warning( this, "No Selection", "There is no text selected" );
  } else {
    QTextCursor topicCursor( cursor );
    if ( cursor.anchor() > cursor.position() ) {
      topicCursor.setPosition( cursor.position() );
    } else {
      topicCursor.setPosition( cursor.anchor() );
    }
    topicCursor.movePosition( QTextCursor::EndOfBlock,
      QTextCursor::KeepAnchor );
    QString defaultTopic = topicCursor.selectedText();
  
    QString topic = QInputDialog::getText( this, "Topic",
      "Please put in topic.", QLineEdit::Normal, defaultTopic );

    if ( topic.isEmpty() ) {
      QMessageBox::warning( this, "No Topic", "No topic was given" );
    } else if ( mTopicManager->topicExists( topic ) ) {
      QMessageBox::warning( this, "Topic Exists", "Topic " + topic +
        " already exists." );
    } else {
      Todoodle *editor = mTopicManager->editor( topic );
      editor->textCursor().insertFragment( QTextDocumentFragment( cursor ) );
      editor->show();
      editor->raise();
      
      cursor.removeSelectedText();

      QTextCharFormat format = TextFormats::topicLinkCharFormat( topic );

      cursor.insertText( topic, format );

      cursor.clearSelection();
    }
  }
}

void Todoodle::fileInline()
{
  QTextCursor cursor = mEditor->textCursor();
  
  QString anchor = cursor.charFormat().anchorHref();
  if ( !anchor.isEmpty() && anchor.startsWith( "todoodle:" ) ) {
#if 0
    QString text = topicText( anchor.mid( 9 ) );
#else
    Todoodle *editor = mTopicManager->editor( anchor.mid( 9 ) );
    QTextDocumentFragment fragment;
    if ( editor ) {
      QTextCursor c = editor->textCursor();
      c.movePosition( QTextCursor::Start );
      c.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
      fragment = QTextDocumentFragment( c );
    }
#endif
    if ( fragment.isEmpty() ) {
      QMessageBox::warning( this, "No Topic", "No text for topic '" + anchor +
        "'found." );
    } else {
      while( !cursor.atStart() && ( cursor.charFormat().anchorHref() == anchor ) ) {
        cursor.movePosition( QTextCursor::Left );
      }
      cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor );
      while( !cursor.atEnd() && ( cursor.charFormat().anchorHref() == anchor ) ) {
        cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor );
      }
      cursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor );
      dbg() << "Link to inline: " << cursor.selectedText() << endl;
      cursor.removeSelectedText();
      cursor.insertFragment( fragment );
      editor->close();
    }
  }
}

void Todoodle::indent()
{
  QTextStream out( stdout );
  for( int i = 0; i < mIndent; ++i ) out << " ";
}

void Todoodle::dumpStructure()
{
  QTextStream out( stdout );

  mIndent = 0;

  out << "DOCUMENT:" << endl;

  QTextDocument *doc = mEditor->document();
  QTextFrame *rootFrame = doc->rootFrame();

  printFrame( rootFrame );
}

void Todoodle::printFrame( QTextFrame *frame )
{
  QTextStream out( stdout );

  mIndent += 2;
  QTextFrame::iterator it;
  for( it = frame->begin(); it != frame->end(); ++it ) {
    QTextBlock block = it.currentBlock();
    if ( block.isValid() ) {
      indent();
      out << "BLOCK" << endl;
      mIndent += 2;
      indent();
      out << "TEXT: '" << block.text() << "'" << endl;
      
      QTextBlockFormat blockFormat = block.blockFormat();
      if ( blockFormat.isValid() ) {
        QTextObject *object = mEditor->document()->objectForFormat( blockFormat );
        if ( QTextList *list = qobject_cast<QTextList*>( object ) ) {
          indent();
          out << "LIST: " << list->objectIndex() << endl;
        }
      }
      
      QTextBlock::iterator it2;
      for( it2 = block.begin(); it2 != block.end(); ++it2 ) {
        QTextFragment fragment = it2.fragment();
        if ( !fragment.isValid() ) continue;
        indent();
        out << "FRAGMENT: '" << fragment.text() << "'" << endl;
        QTextCharFormat format = fragment.charFormat();
        if ( !format.anchorHref().isEmpty() ) {
          mIndent += 2;
          indent();
          out << "HREF: " << format.anchorHref() << endl;
          mIndent -= 2;
        }
      }
      mIndent -= 2;
    }
    if ( it.currentFrame() ) {
      indent();
      out << "FRAME" << endl;
      printFrame( it.currentFrame() );
    }
  }
  mIndent -= 2;
}

void Todoodle::fileClose()
{
  close();
}

void Todoodle::fileExit()
{
  mTopicManager->closeAll();
}

void Todoodle::textTitle()
{
  mEditor->textCursor().setBlockFormat( TextFormats::titleBlockFormat() );

  mEditor->mergeCurrentCharFormat( TextFormats::titleCharFormat() );
}

void Todoodle::textSubtitle()
{
  mEditor->textCursor().setBlockFormat( TextFormats::subTitleBlockFormat() );

  mEditor->mergeCurrentCharFormat( TextFormats::subTitleCharFormat() );
}

void Todoodle::textNormal()
{
  mEditor->textCursor().mergeBlockFormat( TextFormats::normalBlockFormat() );
  mEditor->mergeCurrentCharFormat( TextFormats::normalCharFormat() );
}

void Todoodle::textBold()
{
  mEditor->setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
}

void Todoodle::textUnderline()
{
  mEditor->setFontUnderline(actionTextUnderline->isChecked());
}

void Todoodle::textItalic()
{
  mEditor->setFontItalic(actionTextItalic->isChecked());
}

void Todoodle::textFamily(const QString &f)
{
  mEditor->setFontFamily(f);
}

void Todoodle::textSize(const QString &p)
{
  mEditor->setFontPointSize(p.toFloat());
}

void Todoodle::textStyle(int styleIndex)
{
  QTextCursor cursor = mEditor->textCursor();

  if (styleIndex != 0) {
    QTextListFormat::Style style = QTextListFormat::ListDisc;

    switch (styleIndex) {
        default:
        case 1:
            style = QTextListFormat::ListDisc;
            break;
        case 2:
            style = QTextListFormat::ListCircle;
            break;
        case 3:
            style = QTextListFormat::ListSquare;
            break;
        case 4:
            style = QTextListFormat::ListDecimal;
            break;
        case 5:
            style = QTextListFormat::ListLowerAlpha;
            break;
        case 6:
            style = QTextListFormat::ListUpperAlpha;
            break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    QTextListFormat listFmt;
    listFmt.setStyle(style);
    listFmt.setIndent(blockFmt.indent() + 1);

    blockFmt.setIndent(0);
    cursor.setBlockFormat(blockFmt);

    cursor.createList(listFmt);

    cursor.endEditBlock();
  } else {
    // ####
    QTextBlockFormat bfmt;
    bfmt.setObjectIndex(-1);
    cursor.mergeBlockFormat(bfmt);
  }
}

void Todoodle::textColor()
{
  QColor col = QColorDialog::getColor( mEditor->textColor(), this );
  if (!col.isValid())
      return;
  mEditor->setTextColor(col);
  colorChanged(col);
}

void Todoodle::textAlign(QAction *a)
{
  if (a == actionAlignLeft)
    mEditor->setAlignment(Qt::AlignLeft);
  else if (a == actionAlignCenter)
    mEditor->setAlignment(Qt::AlignHCenter);
  else if (a == actionAlignRight)
    mEditor->setAlignment(Qt::AlignRight);
  else if (a == actionAlignJustify)
    mEditor->setAlignment(Qt::AlignJustify);
}

void Todoodle::currentCharFormatChanged(const QTextCharFormat &format)
{
  fontChanged(format.font());
//  colorChanged(format.textColor());
  alignmentChanged(mEditor->alignment());
}

void Todoodle::clipboardDataChanged()
{
  actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void Todoodle::fontChanged(const QFont &f)
{
  comboFont->setEditText(f.family());
  comboSize->setEditText(QString::number(f.pointSize()));
  actionTextBold->setChecked(f.bold());
  actionTextItalic->setChecked(f.italic());
  actionTextUnderline->setChecked(f.underline());
}

void Todoodle::colorChanged(const QColor &c)
{
  QPixmap pix(16, 16);
  pix.fill(c);
  actionTextColor->setIcon(pix);
}

void Todoodle::alignmentChanged(Qt::Alignment a)
{
  if (a & Qt::AlignLeft)
      actionAlignLeft->setChecked(true);
  else if ((a & Qt::AlignHCenter))
      actionAlignCenter->setChecked(true);
  else if ((a & Qt::AlignRight))
      actionAlignRight->setChecked(true);
  else if ((a & Qt::AlignJustify))
      actionAlignJustify->setChecked(true);
}

void Todoodle::insertFrame()
{
  QTextCursor cursor = mEditor->textCursor();
  
  QTextFrameFormat format;
//  format.setWidth( QTextLength( QTextLength::FixedLength, 4 ) );
  format.setBorder( 2 );
  
  cursor.insertFrame( format );
}

void Todoodle::insertTable()
{
  QTextCursor cursor = mEditor->textCursor();

  QTextTableFormat format;
  format.setBackground( Qt::yellow );
  format.setBorder( 2 );

  cursor.insertTable( 2, 2, format );
}

void Todoodle::insertLink()
{
  QTextCursor cursor = mEditor->textCursor();

  cursor.insertFragment( QTextDocumentFragment::fromHtml(
    QString( "<a href=\"argl\">Link</a>" ) ) );
}

void Todoodle::insertCurrentDate()
{
  QTextCursor cursor = mEditor->textCursor();
  
  cursor.insertText( QDateTime::currentDateTime().toString() + " ",
    TextFormats::dateTimeFormat() );

  mEditor->setCurrentCharFormat( TextFormats::normalCharFormat() );
}

void Todoodle::insertCode()
{
  QTextCursor cursor = mEditor->textCursor();
  cursor.beginEditBlock();
  
  QTextFrame *frame = cursor.insertFrame( TextFormats::codeFrameFormat() );
  
  TextFormats::setCodeFrameFormats( frame );

  cursor.endEditBlock();
}

void Todoodle::insertListDisc()
{
  textStyle( 1 );
}

void Todoodle::insertListDecimal()
{
  textStyle( 4 );
}

void Todoodle::insertTodo()
{
  QTextCursor cursor = mEditor->textCursor();
  
//  cursor.insertBlock();

  QTextImageFormat f;
#if 0
  f.setWidth( 10 );
  f.setHeight( 10 );
#endif
  f.setName( ":/images/todo.png" );

  cursor.insertImage( f );
}

void Todoodle::slotAnchorClicked( const QString &anchor )
{
  if ( anchor.startsWith( "todoodle:" ) ) {
    openTopic( anchor.mid( 9 ) );
  } else {
    QString url = anchor;
    if ( url.startsWith( "www." ) )
        url.prepend( "http://" );
    else if ( url.startsWith( "ftp." ) )
        url.prepend( "ftp://" );

    QProcess *proc = new QProcess;
    connect( proc, SIGNAL( finished( int ) ), proc, SLOT( deleteLater() ) );
    proc->start( "kfmclient", QStringList() << "exec" << url );
    if ( !proc->waitForStarted() )
        delete proc;
  }
}

void Todoodle::loadTopic( const QString &topic )
{
  if ( !mTopic.isEmpty() ) {
    saveTopic();
    writeConfig();
  }

  mTopic = topic;

  dbg() << "Todoodle::loadTopic: " << mTopic << endl;

  if ( mTopic.isEmpty() ) return;

  mEditor->document()->setUndoRedoEnabled( false );
  mTopicManager->load( mTopic, mEditor );
  mScratchPad->load( mTopicManager->scratchPadFilename( mTopic ) );
  mEditor->document()->setUndoRedoEnabled( true );

  QString title = "Todoodle";
  if ( !mTopic.isEmpty() ) title.prepend( mTopic + " - " );
  setWindowTitle( title );

  readConfig();
}

void Todoodle::saveTopic()
{
  dbg() << "saveTopic: " << mTopic << endl;

  if ( mTopic.isEmpty() ) return;

  mTopicManager->save( mTopic, mEditor );
  mScratchPad->save( mTopicManager->scratchPadFilename( mTopic ) );
}

void Todoodle::openTopic( const QString &topic )
{
  Todoodle *editor = mTopicManager->editor( topic );
  editor->show();
  editor->raise();
}

void Todoodle::insertText( const QString &text )
{
  QTextCursor cursor = mEditor->textCursor();
  
  cursor.insertText( text );
}

QString Todoodle::text()
{
  return mEditor->toPlainText();
}

void Todoodle::showTopicList()
{
  TopicList *w = new TopicList( mTopicManager, 0 );
  w->show();
}

void Todoodle::showTopicMap()
{
  mTopicManager->showTopicMap();
}

void Todoodle::showNextActionsList()
{
  mTopicManager->showNextActionsList();
}

void Todoodle::readConfig()
{
  QSettings *settings = mTopicManager->prefs()->settings();

  if ( mTopicManager->windowMode() == TopicManager::Single ) {
    if ( settings->contains( "singlewindowmode/pos" ) ) {
      QPoint pos = settings->value( "singlewindowmode/pos" ).toPoint();
      move( pos );
    }
    if ( settings->contains( "singlewindowmode/size" ) ) {
      QSize size = settings->value( "singlewindowmode/size" ).toSize();
      resize( size );
    }
  } else {
    if ( settings->contains( mTopic + "/pos" ) ) {
      QPoint pos = settings->value( mTopic + "/pos" ).toPoint();
      move( pos );
    }
    if ( settings->contains( mTopic + "/size" ) ) {
      QSize size = settings->value( mTopic + "/size" ).toSize();
      resize( size );
    }
  }

  if ( settings->value( mTopic + "/scratchpad", false ).toBool() ) {
    mEditor->show();
    mScratchPad->show();
    mActionScratchPad->setChecked( true );
  }

  if ( settings->contains( mTopic + "/cursor/position" ) ) {
    QTextCursor cursor = mEditor->textCursor();
    int pos = settings->value( mTopic + "/cursor/position" ).toInt();
    QTextCursor end( cursor );
    end.movePosition( QTextCursor::End );
    if ( pos <= end.position() ) {
      cursor.setPosition( pos );
    }
    mEditor->setTextCursor( cursor );
  }

  QTimer::singleShot( 0, this, SLOT( readSplitterConfig() ) );
}

void Todoodle::readSplitterConfig()
{
  QSettings *settings = mTopicManager->prefs()->settings();

  if ( settings->contains( mTopic + "/splitter/left" ) ) {
    QList<int> sizes;
    sizes.append( settings->value( mTopic + "/splitter/left" ).toInt() );
    sizes.append( settings->value( mTopic + "/splitter/right" ).toInt() );
    mSplitter->setSizes( sizes );
    dbg() << "SIZE left: " << sizes.at( 0 ) << "  right: " << sizes.at( 1 )
      << endl;
  }
}

void Todoodle::writeConfig()
{
  QSettings *settings = mTopicManager->prefs()->settings();

  if ( mTopicManager->windowMode() == TopicManager::Single ) {
    settings->setValue( "singlewindowmode/pos", pos() );
    settings->setValue( "singlewindowmode/size", size() );
  } else {
    settings->setValue( mTopic + "/pos", pos() );
    settings->setValue( mTopic + "/size", size() );
  }

  settings->setValue( mTopic + "/scratchpad", mActionScratchPad->isChecked() );

  QList<int> sizes = mSplitter->sizes();
  if ( sizes.count() == 2 ) {
    settings->setValue( mTopic + "/splitter/left", sizes.at( 0 ) );
    settings->setValue( mTopic + "/splitter/right", sizes.at( 1 ) );
  }
  
  settings->setValue( mTopic + "/cursor/position",
    mEditor->textCursor().position() );
}

void Todoodle::showScratchPad()
{
  if ( mActionScratchPad->isChecked() ) {
    mScratchPad->show();
  } else {
    mScratchPad->hide();
  }
}

void Todoodle::exportHtml()
{
  QString saveFile = QFileDialog::getSaveFileName( this, "HTML Export File" );
  
  if ( !saveFile.isEmpty() ) {
    QFile file( saveFile );
    if ( !file.open( QIODevice::WriteOnly ) ) {
      QMessageBox::warning( this, "Export HTML",
        QString( "Unable to open file '%1' for writing." ).arg( saveFile ) );
    } else {
      QString html = mEditor->document()->toHtml();
      QTextStream ts( &file );
      ts << html;
    }
  }
}

void Todoodle::exportPlainText()
{
  QString saveFile = QFileDialog::getSaveFileName( this, "Plain Text Export File" );
  
  if ( !saveFile.isEmpty() ) {
    FormatPlainText format( mEditor->document() );
    if ( !format.save( saveFile ) ) {
      QMessageBox::warning( this, "Export Plain Text",
        QString( "Unable to open file '%1' for writing." ).arg( saveFile ) );
    }
  }
}

void Todoodle::filePrint()
{
  // ### change back to highres, when it works
  QPrinter printer( QPrinter::ScreenResolution );
  printer.setFullPage( true );

  QPrintDialog dlg( &printer, this );
  if ( dlg.exec() == QDialog::Accepted ) {
    mEditor->document()->print( &printer );
  }
}
