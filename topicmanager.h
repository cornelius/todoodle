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
#ifndef TOPICMANAGER_H
#define TOPICMANAGER_H

#include <QMap>
#include <QString>
#include <QObject>
#include <QStringList>

class Prefs;

class HyperTextEdit;
class Todoodle;
class VersionControl;
class TopicInfo;
class TopicMap;
class NextActionsList;

/**
  This class manages the data of all topics. It is the central class holding the
  data for display or editing by the different views, e.g. the main window and
  editor Todoodle or the list or map views TopicList and TopicMapWidget.
*/
class TopicManager : public QObject
{
    Q_OBJECT
  public:
    /**
      Todoodle can be operated in two modes.
      
      \param Online Connected to the network
      \param Offline Without network access
    */
    enum Mode { Online, Offline };
    /**
      This enum represents the different window modes Todoodle can be operated
      in.
      
      \param Single There is only one window. When navigating to another item
        the content of the window is replaced by the data of the new topic.
      \param Multiple Each topic is opened in a separate window
    */
    enum WindowMode { Single, Multiple };
  
    /**
      Create topic manager.
      
      \param dirName Directory storing the data.
      \param Mode Online of Offline mode
      \param WindowMode window mode
    */
    TopicManager( const QString &dirName = QString::null, Mode mode = Online,
      WindowMode windowMode = Multiple );
    ~TopicManager();

    /**
      Return preferences object used to manage the preferences of the topics
      managed by this topic manager.
    */
    Prefs *prefs() const { return mPrefs; }

    /**
      Return current window mode.
    */
    WindowMode windowMode() const { return mWindowMode; }
    
    /**
      Return main window for given topic.
      
      \param topic name of topic
      \return Pointer to Todoodle main window.
    */
    Todoodle *editor( const QString &topic );

    /**
      Return info about given topic.
      
      \param topic name of topic
      \return pointer to topic info object
    */
    TopicInfo *info( const QString &topic );

    /**
      Remove editor for topic from list of managed editors.
      
      \param topic name of topic
    */
    void removeEditor( const QString &topic );

    /**
      Load data of topic from disk into editor widget.
      
      \param topic name of topic
      \param editor text editing widget
      \return \c true on success, \c false on error
    */
    bool load( const QString &topic, HyperTextEdit *editor );
    /**
      Save data of topic from editor widget to disk.
      
      \param topic name of topic
      \param editor text editing widget
      \return \c true on success, \c false on error
    */
    bool save( const QString &topic, HyperTextEdit * );

    /**
      Return, if the topic exists.
      
      \return \c true, if the topic exists, \c false otherwise.
    */
    bool topicExists( const QString &topic );
    /**
      Return text of topic.
      
      \param topic name of topic
      \return text of topic
    */
    QString topicText( const QString &topic );

    /**
      Return list of all existing topics.
      
      \return List of topic names
    */
    QStringList topics();

    /**
      Close all topic windows.
    */
    void closeAll();

    /**
      Load start topic.
    */
    void loadStart();

    /**
      Return name of file to save scratch pad data.
      
      \param topic name of topic
      \return name of scratch pad file
    */
    QString scratchPadFilename( const QString &topic );

    /**
      Return name of directory where topic data is stored.
      
      \return name of directory
    */
    QString topicDir();

    /**
      Show topic map.
    */
    void showTopicMap();
    /**
      Show list of "next actions"
    */
    void showNextActionsList();

  public slots:
    /**
      Finish saving data and then quit application.
    */
    void finishSave();

  protected slots:
    void slotSaveFinished();
    void slotLoadStartFinished();

  protected:
    QString topicFilename( const QString &topic );
    
  private:
    QMap<QString, Todoodle *> mEditors;
    QMap<QString, TopicInfo *> mInfos;
    
    Todoodle *mSingleEditor;
    
    VersionControl *mVersionControl;

    QString mTopicDir;

    QStringList mTopics;
    
    TopicMap *mTopicMap;
    NextActionsList *mNextActionsList;

    WindowMode mWindowMode;

    Prefs *mPrefs;
    
    QString mCurrentTopic;
};

#endif
