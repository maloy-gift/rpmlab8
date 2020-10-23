/**
  *@file mainwindow.h
  *@brief mainwindow
  *@author Kharchenko Daniil
  */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QStandardItemModel>
#include <QSlider>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QMediaPlaylist *playlist;
    QStandardItemModel  *playListModel;
    qint64 duration;
    QMap <QUrl, qint64> map;
private slots:
    /**
     * @brief add click
     */
    void addClick();
    /**
     * @brief delete click
     */
    void delClick();
    /**
     * @brief next media
     */
    void next();
    /**
     * @brief previous media
     */
    void prev();
    /**
     * @brief play
     */
    void play();
    /**
     * @brief pause
     */
    void pause();
    /**
     * @brief stop
     */
    void stop();
    /**
     * @brief setVolume
     * @param value
     */
    void setVolume(int value);
private:
    /**
     * @brief setTimeline
     * @param value
     */
    void setTimeline(int value);
    /**
     * @brief durationChanged
     * @param duration
     */
    void durationChanged(qint64 duration);
    /**
     * @brief positionChanged
     * @param progress
     */
    void positionChanged(qint64 progress);
    /**
     * @brief seek
     * @param seconds
     */
    void seek(int seconds);
    /**
     * @brief updateDurationInfo
     * @param currentInfo
     */
    void updateDurationInfo(qint64 currentInfo);
    /**
     * @brief setDuration
     */
    void setDuration();
    /**
     * @brief savePosition timeline
     */
    void savePosition();
    /**
     * @brief readPosition timeline
     */
    void readPosition();
    /**
     * @brief saveMap
     */
    void saveMap();
    /**
     * @brief readMap
     */
    void readMap();
signals:
    /**
     * @brief valueChanged
     * @param value
     */
    void valueChanged(int value);
};
#endif // MAINWINDOW_H
