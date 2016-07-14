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

    //game_->createNewGame( GAME_DIM );    

    onGameFinished();
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

        painter.drawImage( QRectF( index * rowSide, ( game_->field().size() + 1 ) * colSide , rowSide, colSide ), images_[ index - 1 ] );

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
            painter.drawRect( QRectF( row * rowSide, col * colSide, rowSide, colSide ) );
            if( game_->field()[ col - 1 ][ row - 1 ] > 0 )
            {
                //painter.drawText( QRectF( row * rowSide, col * colSide, rowSide, colSide ), Qt::AlignCenter, \
                                  QString::number( game_->field()[ col - 1 ][ row - 1 ] ) );
                painter.drawImage( QRectF( row * rowSide, col * colSide, rowSide, colSide ), images_[ game_->field()[ col - 1 ][ row - 1 ] - 1 ]  );
            }

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

    //painter.end();
}

void QGameView::resizeEvent( QResizeEvent * event )
{
    //drawField();
}

void QGameView::drawField()
{
    //QPainter painter( this);


}

void QGameView::onGameCreated()
{
//    drawField();
    lastCell_.first = 1000;
    lastCell_.second = 1000;

    lastCellResult_ = false;

    update();
}

void QGameView::onGameFinished()
{
    assert( game_ );
    
    const QString THEME_PATH = QApplication::applicationDirPath() + "/themes/sc2tvsmiles";
        
    QDir themesDir( THEME_PATH );

    QStringList themesFiles = themesDir.entryList( QStringList( "*.png" ), QDir::Files | QDir::NoSymLinks );

    for( int i = 0; i < themesFiles.size(); ++i )
    {
        int first = qrand() % themesFiles.size();
        int second = qrand() % themesFiles.size();

        if( first != second )
            swap( themesFiles[ first ], themesFiles[ second ] );
    }
    
    images_.clear();
    keywords_.clear();

    for( const QString & name : themesFiles )
    {
        images_.push_back( QImage( THEME_PATH + "/" + name ) );
        keywords_.push_back( ":" + name.left( name.length() - 4 ) + ":" );
    }    
    
    game_->createNewGame( GAME_DIM, keywords_ );
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
