/**
  *@file mainwindow.cpp
  *@brief mainwindow
  *@author Kharchenko Daniil
  */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QTime>
#include <QSpinBox>
#include <QDataStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    playListModel = new QStandardItemModel(this);
    ui->playlist->setModel(playListModel);
    ui->playlist->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlist->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlist->horizontalHeader()->setStretchLastSection(true);

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(player);
    player->setPlaylist(playlist);
    player->setVolume(0);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    this->readMap();
    this->setDuration();

    connect(ui->playlist, &QTableView::doubleClicked, [this](const QModelIndex &index){
            playlist->setCurrentIndex(index.row());});
    connect(playlist, &QMediaPlaylist::currentIndexChanged, [this](int in-dex){ui->title->setText(playListModel->
            data(playListModel->index(index, 0)).toString());});
    connect(ui->prev,     &QPushButton::clicked,          playlist, &QMediaPlaylist::previous);
    connect(ui->next,     &QPushButton::clicked,          playlist, &QMediaPlaylist::next);
    connect(ui->play,     &QPushButton::clicked,          this,     &MainWindow::play);
    connect(ui->pause,    &QPushButton::clicked,          this,     &MainWindow::pause);
    connect(ui->stop,     &QPushButton::clicked,          this,     &MainWindow::stop);
    connect(ui->sound,    SIGNAL(valueChanged(int)),      this,     SLOT(setVolume(int)));
    connect(ui->timeline, SIGNAL(valueChanged(int)),      this,     SLOT(setTimeline(int)));
    connect(ui->add,      &QPushButton::clicked,          this,     &MainWindow::addClick);
    connect(ui->del,      &QPushButton::clicked,          this,     &MainWindow::delClick);
    ui->next->setToolTip("Go to next media.");
    ui->prev->setToolTip("Go to previous media.");
    ui->add->setToolTip("Add some media resources.");
    ui->del->setToolTip("Only the selected row will be delete!");
    QMessageBox::information(NULL,QObject::tr("Information"), tr("Please pay attention to the button tips."));
}

MainWindow::~MainWindow()
{
    this->savePosition();
    this->saveMap();
    delete ui;
    delete playListModel;
    delete playlist;
    delete player;
    delete videoWidget;
}

void MainWindow::addClick()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("Open files"), QString(), tr("Audio Files (*.mp3 *.wav);; "
                                                      "Video files (*.avi *.webm)"));
    foreach (QString filePath, files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));
        items.append(new QStandardItem(filePath));
        playListModel->appendRow(items);
        playlist->addMedia(QUrl(filePath));
        videoWidget = new QVideoWidget;
    }
}

void MainWindow::delClick()
{
    savePosition();
    playListModel->removeRow(ui->playlist->currentIndex().row());
    playlist->removeMedia(ui->playlist->currentIndex().row()+1);
    if(playlist->isEmpty()){
        ui->title->setText("Current Track");
        player->stop();
    }
    else{
        ui->title->setText(playListModel->data(playListModel->index(playlist->currentIndex(), 0)).toString());
    }
}

void MainWindow::next()
{
    savePosition();
    playlist->next();
    player->play();
}

void MainWindow::prev()
{
    savePosition();
    playlist->previous();
    player->play();
}

void MainWindow::play()
{
    videoWidget = new QVideoWidget(ui->video);
    player->setVideoOutput(videoWidget);
    videoWidget->show();
    ui->video->show();
    player->setVideoOutput(videoWidget);
    videoWidget->resize(ui->video->size());
    videoWidget->show();
    player->play();
    readPosition();
}

void MainWindow::pause()
{
    savePosition();
    player->pause();
}

void MainWindow::stop()
{
    map.remove(playlist->currentMedia().request().url());
    player->stop();
}

void MainWindow::setVolume(int value)
{
    player->setVolume(value);
}

void MainWindow::setTimeline(int value)
{
    player->setPosition(value);
}

void MainWindow::durationChanged(qint64 dur)
{
    duration = dur / 1000;
    ui->timeline->setMaximum(dur / 1000);
}

void MainWindow::positionChanged(qint64 progress)
{
    if (!ui->timeline->isSliderDown()){
        ui->timeline->setValue(progress / 1000);}
    updateDurationInfo(progress / 1000);
}

void MainWindow::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((duration / 3600) % 60, (duration / 60) % 60,
            duration % 60, (duration * 1000) % 1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + to-talTime.toString(format);
    }
    ui->time->setText(tStr);
}

void MainWindow::readMap()
{
    QFile f ("position.txt");
    if (!f.open(QIODevice::ReadOnly)) return;
    QDataStream str(&f);
    map.clear();
    str >> map;
    f.remove();
}

void MainWindow::saveMap()
{
    QFile f ("position.txt");
    if (!f.open(QIODevice::WriteOnly)) return;
    QDataStream str(&f);
    str << map;
}

void MainWindow::savePosition()
{
    map.insert(playlist->currentMedia().request().url(), player->position());
}

void MainWindow::readPosition()
{
    foreach(QUrl key, map.keys()){
        if(key.toString().compare(playlist->currentMedia().request().url().toString()) == 0){
            qint64 value = map.value(key);
            player->setPosition(value);
        }
    }
}

void MainWindow::setDuration()
{
    ui->timeline->setRange(0, player->duration()/1000);
    connect(ui->timeline, &QSlider::sliderMoved,
            this, &MainWindow::seek);
    connect(player, &QMediaPlayer::durationChanged,
            this, &MainWindow::durationChanged);
    connect(player, &QMediaPlayer::positionChanged,
            this, &MainWindow::positionChanged);
} ?
