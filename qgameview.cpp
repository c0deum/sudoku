#include <cassert>

#include <QApplication>

#include <QDir>

#include <QPainter>
#include <QPen>

#include <QResizeEvent>

#include "qgameview.h"

const std::size_t GAME_DIM = 3;

QGameView::QGameView( QWidget * parent )
    : QWidget( parent )
    , game_( new Game( this ) )
{
    QObject::connect( game_, SIGNAL( gameCreated() ), this, SLOT( onGameCreated() ) );
    QObject::connect( game_, SIGNAL( gameFieldChanged( std::size_t, std::size_t, bool ) ), this, SLOT( onGameFieldChanged( std::size_t, std::size_t, bool ) ) );
    QObject::connect( game_, SIGNAL( gameFinished() ), this, SLOT( onGameFinished() ) );

    loadTheme();

    game_->createNewGame( GAME_DIM );



    //onGameFinished();
}

void QGameView::paintEvent( QPaintEvent * event )
{
    //drawField();

    QPainter painter( this );

    assert( game_ );

    //painter.begin( this );

    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
    painter.setRenderHint( QPainter::TextAntialiasing, true );


    painter.eraseRect( this->rect() );

    QFont font( painter.font() );

    font.setPixelSize( qMin( rect().width() / ( game_->field().size() + 1 ), rect().height() / ( game_->field().size() + 1 )  ) * 3 / 4 );

    painter.setFont( font );

    float rowSide = 1.0f * rect().width() / ( game_->field().size() + 1 );
    float colSide = 1.0f * rect().height() / ( game_->field().size() + 2 );

    for( int index = 1; index <= game_->field().size(); ++index )
    {
        painter.fillRect( QRectF( index * rowSide, 0, rowSide, colSide ), QBrush( Qt::yellow ) );
        painter.fillRect( QRectF( 0, index * colSide, rowSide, colSide ), QBrush( Qt::yellow ) );                

        painter.fillRect( QRectF( index * rowSide, ( game_->field().size() + 1 ) * colSide , rowSide, colSide ), QBrush( Qt::yellow ) );

        painter.drawText( QRectF( index * rowSide, 0, rowSide, colSide ), Qt::AlignCenter, QString::number( index ) );
        painter.drawText( QRectF( 0, index * colSide, rowSide, colSide ), Qt::AlignCenter, QString::number( index ) );

        painter.drawImage( QRectF( index * rowSide, ( game_->field().size() + 1 ) * colSide , rowSide, colSide ), images_[ indexMap_[ index - 1 ] ] );

        painter.drawRect( QRectF( index * rowSide, 0, rowSide, colSide ) );
        painter.drawRect( QRectF( 0, index * colSide, rowSide, colSide ) );

        painter.drawRect( QRectF( index * rowSide, ( game_->field().size() + 1 ) * colSide , rowSide, colSide ) );
    }

    for( int row = 1; row <= game_->field().size(); ++row )
    {
        for( int col = 1; col <= game_->field().size(); ++col )
        {
            if( ( col - 1 == lastCell_.first ) && ( row - 1 == lastCell_.second ) )
            {
                painter.fillRect( QRectF( row * rowSide, col * colSide, rowSide, colSide ), lastCellResult_? QBrush( Qt::green ): QBrush( Qt::red ) );
            }

            if( game_->field()[ col - 1 ][ row - 1 ] > 0 )
            {
                //painter.drawText( QRectF( row * rowSide, col * colSide, rowSide, colSide ), Qt::AlignCenter, \
                                  QString::number( game_->field()[ col - 1 ][ row - 1 ] ) );
                painter.drawImage( QRectF( row * rowSide, col * colSide, rowSide, colSide ), images_[ indexMap_[ game_->field()[ col - 1 ][ row - 1 ] - 1 ] ] );
            }

            painter.drawRect( QRectF( row * rowSide, col * colSide, rowSide, colSide ) );

        }
    }

    for( int index = 1; index <= game_->field().size() + 1; index += GAME_DIM )
    {
        QPen oldPen = painter.pen();

        painter.setPen( QPen( Qt::black, 5 ) );

        painter.drawLine( 0, index * colSide, rect().width(), index * colSide );
        painter.drawLine( index * rowSide, 0, index * rowSide, rect().height() );

        painter.setPen( oldPen );
    }


    if( lastCell_.first < game_->field().size() && lastCell_.second < game_->field().size()  )
    {
        QPen oldPen = painter.pen();

        painter.setPen( QPen( lastCellResult_? QBrush( Qt::green ): QBrush( Qt::red ), 9 ) );

        painter.drawRect( QRectF( ( lastCell_.second + 1 ) * rowSide, ( lastCell_.first + 1 ) * colSide, rowSide, colSide ) );

        painter.setPen( oldPen );
    }
}



void QGameView::onGameCreated()
{
    lastCell_.first = 1000;
    lastCell_.second = 1000;

    lastCellResult_ = false;        

    update();
}

void QGameView::onGameFinished()
{
    if( startNewGameTimerId_ == -1 )
        startNewGameTimerId_ = startTimer( startNewGameDelay );
}

void QGameView::onGameFieldChanged( std::size_t row, std::size_t col, bool success )
{
    //drawField();

    lastCell_.first = row;
    lastCell_.second = col;

    lastCellResult_ = success;

    update();
}

const Game * QGameView::game() const
{
    return game_;
}

void QGameView::loadTheme()
{
    const QString THEME_PATH = QApplication::applicationDirPath() + "/themes/pokemons";

    QDir themesDir( THEME_PATH );

    QStringList themesFiles = themesDir.entryList( QStringList( "*.png" ), QDir::Files | QDir::NoSymLinks );

    images_.clear();
    indexMap_.clear();
    //keywords_.clear();

    //for( const QString & name : themesFiles )
    for( std::size_t index = 0; index < themesFiles.size(); ++index )
    {
        //images_.push_back( QImage( THEME_PATH + "/" + name ) );

        //keywords_.push_back( ":" + name.left( name.length() - 4 ) + ":" );

        images_.push_back( QImage( THEME_PATH + "/" + themesFiles[ index ] ) );
        indexMap_.push_back( index );
    }

    for( int i = 0; i < indexMap_.size(); ++i )
    {
        int first = qrand() % indexMap_.size();
        int second = qrand() % indexMap_.size();

        if( first != second )
        {
            //swap( indexMap_[ first ], indexMap_[ second ] );
            std::size_t tmp = indexMap_[ first ];
            indexMap_[ first ] = indexMap_[ second ];
            indexMap_[ second ] = tmp;

        }
    }

}

void QGameView::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == startNewGameTimerId_ )
    {
        assert( game_ );
        game_->createNewGame( GAME_DIM/*, keywords_*/ );
        killTimer( startNewGameTimerId_ );
        startNewGameTimerId_ = -1;
    }
}
