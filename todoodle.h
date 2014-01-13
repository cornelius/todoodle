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
#ifndef TODOODLE_H
#define TODOODLE_H

#include <qmainwindow.h>
#include <qmap.h>
#include <qpointer.h>

#include <QTextCursor>

class QAction;
class QComboBox;
class QTabWidget;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QTextFrame;
class QSettings;
class QSplitter;

class TopicManager;
class ScratchPad;
class HyperTextEdit;

/**
  This class is the main window class of Todoodle. Each topic has its own main
  window.
*/
class Todoodle : public QMainWindow
{
    Q_OBJECT
  public:
    /**
      Create new topic main window.
      
      \param tm TopicManager managing the topic data
      \param parent Parent widget
    */
    Todoodle( TopicManager *tm, QWidget *parent = 0 );
    ~Todoodle();

    /**
      Open main window for given topic.
      
      \param topic name of topic
    */
    void openTopic( const QString &topic );

    /**
      Insert text at current cursor position in editing area.
    
      \param text text to be inserted
    */
    void insertText( const QString & );
    /**
      Return text of editing area.
      
      \return Text
    */
    QString text();

    /**
      Return text cursor of editing area.
      
      \return Current text cursor
    */
    QTextCursor textCursor();

    /**
      Load topic data from disk and show it in main window.
      
      \param topic name of topic
    */
    void loadTopic( const QString &topic );

    /**
      Return name of topic shown in this window.
      
      \return name of topic
    */
    QString topic() const { return mTopic; }

    /**
      Return pointer to editor widget.
      
      \return editing area widget
    */
    HyperTextEdit *editor() const { return mEditor; }

  protected:
    void saveTopic();

    void readConfig();
    void writeConfig();

    void closeEvent( QCloseEvent * );

  private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    void setupInsertActions();
    void setupNavigationActions();
    void setupHelpActions();

  private slots:
    void fileNew();
    void fileLink();
    void fileExtract();
    void fileInline();
    void filePrint();
    void dumpStructure();
    void exportHtml();
    void exportPlainText();
    void fileClose();
    void fileExit();

    void textTitle();
    void textSubtitle();
    void textNormal();
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);

    void insertFrame();
    void insertTable();
    void insertLink();
    void insertListDisc();
    void insertListDecimal();
    void insertTodo();
    void insertCurrentDate();
    void insertCode();

    void goStart();
    void goManual();
    void goBack();
    void goForward();

    void currentCharFormatChanged(const QTextCharFormat &format);

    void clipboardDataChanged();

    void slotAnchorClicked( const QString & );

    void showTopicList();
    void showTopicMap();
    void showNextActionsList();

    void showScratchPad();

    void readSplitterConfig();

  private:
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    void indent();

    void printFrame( QTextFrame * );

    QAction *actionTextBold,
	*actionTextUnderline,
	*actionTextItalic,
	*actionTextColor,
	*actionAlignLeft,
	*actionAlignCenter,
	*actionAlignRight,
	*actionAlignJustify,
        *actionUndo,
        *actionRedo,
        *actionCut,
        *actionCopy,
        *actionPaste;

    QAction *mActionScratchPad;

    QToolBar *mToolBar;
    
    TopicManager *mTopicManager;
    QString mTopic;

    QComboBox *comboStyle,
	*comboFont,
	*comboSize;

    QString mFilename;

    HyperTextEdit *mEditor;
    ScratchPad *mScratchPad;
    QSplitter *mSplitter;

    int mIndent;
};


#endif
