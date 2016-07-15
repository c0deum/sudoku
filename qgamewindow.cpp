#include <cassert>


#include <QListWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QResizeEvent>

#include <QDateTime>


#include "qgameview.h"

#include "qgamewindow.h"

QGameWindow::QGameWindow(QWidget *parent)
    : QWidget(parent)
    , gameView_( new QGameView( this ) )
    , gameStatList_( new QListWidget( this ) )
    , gamesStatList_( new QListWidget( this ) )
    , stepsList_( new QListWidget( this ) )
{
    qsrand( QDateTime::currentMSecsSinceEpoch() );

    QObject::connect( gameView_->game(), SIGNAL( gameCreated() ), this, SLOT( onGameCreated() ) );
    QObject::connect( gameView_->game(), SIGNAL( gameFinished() ), this, SLOT( onGameFinished() ) );
    QObject::connect( gameView_->game(), SIGNAL( gameFieldChanged(std::size_t,std::size_t,bool) ), this, SLOT( onGameFieldChanged() ) );

    QHBoxLayout * layout = new QHBoxLayout( this );

    layout->addWidget( gameView_, 3 );

    QVBoxLayout * infoLayout = new QVBoxLayout();

    QHBoxLayout * statsLayout = new QHBoxLayout();

    statsLayout->addWidget( gameStatList_, 1 );
    statsLayout->addWidget( gamesStatList_, 1 );

    infoLayout->addLayout( statsLayout, 1 );

    infoLayout->addWidget( stepsList_, 1 );

    layout->addLayout( infoLayout, 2 );


    setLayout( layout );
}

void QGameWindow::onGameCreated()
{
    gameStatList_->clear();
    stepsList_->clear();
}

void QGameWindow::onGameFinished()
{

    const QMap< QString, int  > & statMap = gameView_->game()->stat();

    assert( !statMap.isEmpty() );

    QList< int > pointsList = statMap.values().toSet().toList();

    qSort( pointsList );

    QList< QString > nickNamesList = statMap.keys( pointsList.last() );

    for( QString & nickName : nickNamesList )
    {
        //gamesStatList_->addItem( nickName + "[" + QString::number( pointsList[ i ] ) + "]" );

        if( !gamesStat_.contains( nickName ) )
            gamesStat_[ nickName ] = 0;
        gamesStat_[ nickName ]++;
    }

    gamesStatList_->clear();

    QList< int > gamesPointsList = gamesStat_.values().toSet().toList();

    qSort( gamesPointsList );

    for( int i = gamesPointsList.size() - 1; i >= 0; --i )
    {
        QList< QString > nickNamesList = gamesStat_.keys( gamesPointsList[ i ] );

        qSort( nickNamesList );

        for( QString & nickName : nickNamesList )
        {
            gamesStatList_->addItem( nickName + "[" + QString::number( gamesPointsList[ i ] ) + "]" );
        }
    }

}

void QGameWindow::onGameFieldChanged()
{
    gameStatList_->clear();

    const QMap< QString, int  > & statMap = gameView_->game()->stat();

    QList< int > pointsList = statMap.values().toSet().toList();

    qSort( pointsList );

    for( int i = pointsList.size() - 1; i >= 0; --i )
    {
        QList< QString > nickNamesList = statMap.keys( pointsList[ i ] );

        qSort( nickNamesList );

        for( QString & nickName : nickNamesList )
        {
            gameStatList_->addItem( nickName + "[" + QString::number( pointsList[ i ] ) + "]" );
        }
    }
}

void QGameWindow::resizeEvent( QResizeEvent * event )
{
    QFont font = gameStatList_->font();

    font.setPixelSize( event->size().height() / 36 );

    gameStatList_->setFont( font );
    gamesStatList_->setFont( font );
}
