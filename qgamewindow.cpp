#include <QListWidget>

#include <QHBoxLayout>

#include <QResizeEvent>

#include <QDateTime>


#include "qgameview.h"

#include "qgamewindow.h"

QGameWindow::QGameWindow(QWidget *parent)
    : QWidget(parent)
    , gameView_( new QGameView( this ) )
    , statLis_( new QListWidget( this ) )
{
    qsrand( QDateTime::currentMSecsSinceEpoch() );

    QObject::connect( gameView_->game(), SIGNAL( gameFieldChanged(std::size_t,std::size_t,bool) ), this, SLOT( onStatChanged() ) );

    QHBoxLayout * layout = new QHBoxLayout( this );

    layout->addWidget( gameView_, 9 );
    layout->addWidget( statLis_, 7 );

    setLayout( layout );


}

void QGameWindow::onStatChanged()
{
    statLis_->clear();

    const QMap< QString, int  > & statMap = gameView_->game()->stat();

    QList< int > pointsList = statMap.values().toSet().toList();

    qSort( pointsList );

    for( int i = pointsList.size() - 1; i >= 0; --i )
    {
        QList< QString > nickNamesList = statMap.keys( pointsList[ i ] );

        qSort( nickNamesList );

        for( QString & nickName : nickNamesList )
        {
            statLis_->addItem( nickName + "[" + QString::number( pointsList[ i ] ) + "]" );
        }
    }
}

void QGameWindow::resizeEvent( QResizeEvent * event )
{
    QFont font = statLis_->font();

    font.setPixelSize( event->size().height() / 24 );

    statLis_->setFont( font );
}
