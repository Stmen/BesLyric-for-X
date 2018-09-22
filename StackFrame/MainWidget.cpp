﻿#include "MainWidget.h"
#include <QVBoxLayout>
#include <QKeyEvent>
#include "MusicPlayer/musicPlayer.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),topWidget(nullptr),middleWidget(nullptr),bottomWidget(nullptr)
{
    this->setMouseTracking(true);//详见 BesFramelessWidget.h 注释

    initEntity();
    initLayout();
    initConnection();
}

MainWidget::~MainWidget()
{

}

void MainWidget::initEntity()
{
}

void MainWidget::initLayout()
{
    topWidget = new TopWidget(this);
    middleWidget = new MiddleWidget(this);
    bottomWidget = new BottomWidget(this);
    topWidget->setObjectName("topWidget");
    middleWidget->setObjectName("middleWidget");
    bottomWidget->setObjectName("bottomWidget");

    topWidget->setMinimumHeight(65);
    topWidget->setMaximumHeight(65);
    topWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    bottomWidget->setMinimumHeight(65);
    bottomWidget->setMaximumHeight(65);
    bottomWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget(topWidget);
    vLayout->addWidget(middleWidget);
    vLayout->addWidget(bottomWidget);
}

void MainWidget::initConnection()
{
    //页面切换响应
    connect(topWidget->btnIcon, &QPushButton::clicked, [=](){
        if(middleWidget->currentPage == 1)
            middleWidget->switchPage(1,0);
        if(middleWidget->currentPage == 2)
            middleWidget->switchPage(2,0);
        if(middleWidget->currentPage == 4)
            middleWidget->switchPage(4,0);
    });

    connect(topWidget->btnSetting, &QPushButton::clicked, [=](){
        if(middleWidget->currentPage == 1)
            middleWidget->switchPage(1,0);
        if(middleWidget->currentPage == 2)
            middleWidget->switchPage(2,0);

        if(middleWidget->currentPage != 4)
            middleWidget->switchPage(0,4);
    });

    connect(middleWidget->pageSetting->btnPackupSetting, &QPushButton::clicked, [=](){
            middleWidget->switchPage(4,0);});

    connect(middleWidget->pageLyricList->btnPackupLyricList,&QPushButton::clicked, [=](){
        middleWidget->switchPage(2,0);});

    //制作歌词页面信号响应
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onReloadMusic(QString)),
                                                 this, SLOT(onUnloadLyricFromPreviewPage()));

    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onReloadMusic(QString)),
                                                 middleWidget->pagePreviewLyric, SLOT(setToDefaultAlbumImage()));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onReloadMusic(QString)),
                                                 middleWidget->pagePreviewLyric->lyricViewer, SLOT(setMusicPath(QString)));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onReloadMusic(QString)),
                                                 middleWidget->pageMain->boxPagePreviewLyric, SLOT(setToDefaultPic()));


    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onReloadMusic(QString)),
                                                        bottomWidget, SLOT(reloadMusic(QString)));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onStartMaking()),
                                                        bottomWidget, SLOT(playFromBegin()));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onSeekBackward(quint64)),
                                                        bottomWidget, SLOT(seekBackward(quint64)));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onPlayOrPauseMusic()),
                                                        bottomWidget, SLOT(autoPlayOrPause()));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onStopMusic()),
                                                        bottomWidget, SLOT(stop()));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(loadLrcLyricAndSwitchToPreview(QString))
                                                        ,this, SLOT(onLoadLrcLyricAndSwitchToPreview(QString)));

    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onEnterMakingMode())
                                                        ,bottomWidget, SLOT(enterMakingMode()));
    connect(middleWidget->pageMain->subPageMaking,SIGNAL(onExitMakingMode())
                                                        ,bottomWidget, SLOT(exitMakingMode()));

    //歌词单页面
    connect(middleWidget->pageLyricList->tableLrcList,&BesLListTableView::sig_playSongAndLyric,
                    [=](){
                            middleWidget->pageMain->subPageMaking->btnStartMaking->setEnabled(false);
                    }); //重新载入了音乐，就不能直接点击【开始制作】了
    connect(middleWidget->pageLyricList->tableLrcList,SIGNAL(sig_playSongAndLyric(QString,QString)),
            this,SLOT(OnPlayNewMusicAndLyric(QString,QString)));

    //底部播放器信号响应
    connect(bottomWidget->musicPlayer, SIGNAL(positionChanged(int)),this, SLOT(musicPositionChanged(int)));

    connect(bottomWidget->musicPlayer, SIGNAL(titleFound(QString)),
                                            middleWidget->pagePreviewLyric->lyricViewer, SLOT(setMusicTitle(QString)));
    connect(bottomWidget->musicPlayer, SIGNAL(pictureFound(QPixmap)),
                                            middleWidget->pageMain->boxPagePreviewLyric, SLOT(changePic(QPixmap)));
    connect(bottomWidget->musicPlayer, SIGNAL(pictureFound(QPixmap)),
                                            middleWidget->pagePreviewLyric, SLOT(AlbumImageChanged(QPixmap)));
    connect(bottomWidget->musicPlayer, SIGNAL(audioPlay()),
                                            middleWidget->pagePreviewLyric, SLOT(playPhonagraph()));
    connect(bottomWidget->musicPlayer, SIGNAL(audioPause()),
                                            middleWidget->pagePreviewLyric, SLOT(stopPhonagraph()));
    connect(bottomWidget->musicPlayer, SIGNAL(audioFinish()),
                                            middleWidget->pagePreviewLyric, SLOT(stopPhonagraph()));

    connect(bottomWidget->musicPlayer, SIGNAL(audioFinish()),
                                            middleWidget->pageMain->subPageMaking, SLOT(finishMaking()));


	connect(bottomWidget->musicPlayer, SIGNAL(sig_playThreadFinished()),
											this, SLOT(onAudioTheadFinished()));  //这里放在 finishMaking 前执行
}


bool MainWidget::keyPress(QKeyEvent  *event)
{
    if( middleWidget->currentPage == 0 &&                    //在制作页面，且在制作歌词过程
            middleWidget->pageMain->subPageMaking->isMaking) //才响应以下按键
    {
        if(event->key() == Qt::Key_Up)  //推上一行
        {
            middleWidget->pageMain->subPageMaking->markOneLine();
        }
        else if(event->key() == Qt::Key_Right) //空出一行
        {
            middleWidget->pageMain->subPageMaking->markEmptyLine();
        }
        else if(event->key() == Qt::Key_B) //回退 5 秒
        {
            middleWidget->pageMain->subPageMaking->backBy5Second();
        }
        else if(event->key() == Qt::Key_Space) //暂停
        {
            middleWidget->pageMain->subPageMaking->playOrPause();
        }
        else if(event->key() == Qt::Key_Return) //结束制作
        {
            middleWidget->pageMain->subPageMaking->finishMaking();
        }
        return true;
    }

    return false;
}

void MainWidget::OnPlayNewMusicAndLyric(QString music, QString lrc)
{
    if(middleWidget->pageMain->subPageMaking->isMaking)
    {
        BesMessageBox::information(tr("提示"),tr("制作歌词过程中，不能播放其他音乐"));
        return;
    }

    //所有非自动播放结束导致的歌曲结束操作前，都需要 flagStopAutoPlaying 置 1防止自动播发行为，导致的程序错误
    //相关逻辑全局搜索：【flag_play_mode】
    //SettingManager::GetInstance().data().flagStopAutoPlaying = 1;

    //重载入音乐
    bottomWidget->reloadMusic(music);

    //载入歌词
    middleWidget->pagePreviewLyric->lyricViewer->setLrcLyricPath(lrc);

    //修改歌曲显示名称
    middleWidget->pagePreviewLyric->lyricViewer->setMusicPath(music);

    //播放歌曲
    bottomWidget->play();

//    //执行切换页面相关逻辑，如果当前正在歌词单页面，收起歌词单页面，展开预览页面
//    if(middleWidget->currentPage == 2)
//    {
//        middleWidget->switchPage(2,0);
//        middleWidget->switchPage(0,1);
//    }
}

//载入lrc歌词，并且播放当前歌曲来预览
void MainWidget::onLoadLrcLyricAndSwitchToPreview(QString lycLyricPath)
{
    //为了防止逻辑上的不明确，必须等到没有歌曲在播放的时候，才响应歌词的载入与预览
    if(bottomWidget->musicPlayer->state() != MusicPlayer::StoppedState)
    {
        //没有有效歌词时尝试载入歌词
        if(!middleWidget->pagePreviewLyric->lyricViewer->isLyricValid())
            middleWidget->pagePreviewLyric->lyricViewer->setLrcLyricPath(lycLyricPath);

        //切换页面(从主页面 0，到预览歌词页面1 切换 )
        middleWidget->switchPage(0,1);  //已经有歌词在播放，直接切换页面
        return ;
    }

    //载入歌词
    middleWidget->pagePreviewLyric->lyricViewer->setLrcLyricPath(lycLyricPath);

    //切换页面(从主页面 0，到预览歌词页面1 切换 )
    middleWidget->switchPage(0,1);

    //播放歌曲
    bottomWidget->play();
}


//从预览页面卸下歌词
void MainWidget::onUnloadLyricFromPreviewPage()
{
     middleWidget->pagePreviewLyric->lyricViewer->setEmptyLyric();
}

//音乐位置发生改变
void MainWidget::musicPositionChanged(int pos)
{
    //改变下方音乐条
    bottomWidget->positionChanged(pos);

    //改变制作歌词时的音乐时间
    if(middleWidget->pageMain->subPageMaking->isMaking
            && middleWidget->currentPage == 0)
        middleWidget->pageMain->subPageMaking->updatePos(pos);

    //改变预览歌词时的滚动状态
    if( middleWidget->pagePreviewLyric->lyricViewer->isLyricValid()
            && middleWidget->currentPage == 1)
        middleWidget->pagePreviewLyric->lyricViewer->scrollLyricPanel->lyricPanel->higthLineLyricAtPos(pos);
}

void MainWidget::onAudioTheadFinished()
{
    if(middleWidget->pageMain->subPageMaking->isMaking)
        return; // 制作歌词模式，永远只播放一首歌

    //相关逻辑全局搜索：【flag_play_mode】
    //【特别注意】以下的逻辑为自动播放下一首的逻辑，这里处理的情况仅仅是自动播放结束的情况。
    // 由于这里 播放下一首需要保证上一首的线程完全退出再播放
    // 所以会调用到 bottomWidget->stop(); 一直到线程结束后返回。
    // 然而，所有用户界面主动直接触发的播放，由于结束了上一首歌触发本函数，在本函数执行之前都有可能已经结束了歌曲，并重新执行了下一首的播放
    // 这个时候 本函数调用的 bottomWidget->stop(); 尝试结束的就不是本来预期的自动播放结束的歌曲，而是接下来要播放的歌曲
    // 进而会导致 卡死在 bottomWidget->stop() 中，而且这种逻辑本身就是不合理的，所以，所有用户主动触发的播放操作，都需要
    // 置 SettingManager::GetInstance().data().flagStopAutoPlaying = 1，避免这种情况

    bool bStopNextPlay = SettingManager::GetInstance().data().flagStopAutoPlaying == 1;
    SettingManager::GetInstance().data().flagStopAutoPlaying = 0; //每次访问后都重置该标志
    if(bStopNextPlay)
        return;

    int nPlayMode = SettingManager::GetInstance().data().playMode;
    if(nPlayMode == 0)  //单曲播放模式，也不继续播放
        return;


    if(nPlayMode == 1) //单曲循环模式，直接重新播放
    {
        //bottomWidget->stop();
        //播放歌曲
        //bottomWidget->play();
    }
}

