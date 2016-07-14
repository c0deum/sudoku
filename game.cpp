#include <cassert>

#include <QWebSocket>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include <QTimerEvent>

#include "field.h"

#include "game.h"

Game::Game( QObject *parent )
    : QObject(parent)
{
    connectToWebSocket();
}

Game::~Game()
{
    delete field_;
}

void Game::createNewGame( std::size_t dim, const QStringList & keywords )
{
    delete field_;

    field_ = Field::generate( dim );

    Field test( field_->frontField(), dim );

    std::cout << "Base Field:" << std::endl;

    std::cout << test;

    test.resolve();

    std::cout << "Resolved Field:" << std::endl;

    std::cout << test;

    keywords_ = keywords;

    emit gameCreated();
}

void Game::connectToWebSocket()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( connect ), this, SLOT( onWebSocketConnected() ) );
    QObject::connect( socket_, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived(QString) ), this, SLOT( onTextMessageReceived( QString ) ) );

    socket_->open( "ws://localhost:" + QString::number( socketPort_ ) );
}

void Game::onWebSocketConnected()
{
    startUniqueTimer( saveConnectionTimerId_, saveConnectionInterval_ );
}

void Game::onWebSocketError()
{
    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );
}

void Game::onTextMessageReceived( const QString & message )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( message.toUtf8(), &parseError );

    if( ( QJsonParseError::NoError == parseError.error ) && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        if( jsonObj[ "type" ].toString() == "message" )
        {
            QJsonObject jsonMessage = jsonObj[ "message" ].toObject();

            QString message = jsonMessage[ "message" ].toString();


            QString smile;

            const QString PREFIX = "<img class = \"smile\" alt=\"";


            int startSmilePos = message.indexOf( PREFIX );

            if( startSmilePos != -1 )
            {
                startSmilePos += PREFIX.length();
                int endSmilePos = message.indexOf( "\"", startSmilePos ) - 1;

                if( endSmilePos - startSmilePos + 1 > 0 )
                {
                    smile = message.mid( startSmilePos, endSmilePos - startSmilePos + 1 );
                }

            }

            message.remove( QRegExp( "<[^>]*>" ) );

            message += smile;

            message.replace( "  ", " ");

            QStringList tokens = message.split( QRegExp( "\\s" ) );

            if( tokens.size() == 3 )
            {
                bool bVal, bRow, bCol;

                int row = tokens[ 0 ].toInt( &bRow);
                int col = tokens[ 1 ].toInt( &bCol );
                //int val = tokens[ 2 ].toInt( &bVal );
                QString keyword = tokens[ 2 ];

                //if( !bVal || !bRow || !bCol )
                if( !bRow || !bCol )
                    return;

                if( /*val > 0 && val <= field_->sqrDim()*/ keywords_.contains( keyword )  && row > 0 && row <= field_->sqrDim() && col > 0 && col <= field_->sqrDim() )
                {
                    int val = keywords_.indexOf( keyword ) + 1;
                    row--;
                    col--;

                    QString nickName = jsonMessage[ "nick" ].toString();
                    nickName.remove( QRegExp( "<[^>]*>" ) );

                    if( !stat_.contains( nickName ) )
                    {
                        stat_[ nickName ] = 0;
                    }

                    if( field_->isStepAvailable( row, col, val ) )
                    {
                        stat_[ nickName ] += field_->freeCells();

                        field_->set( row, col, val );
                        emit gameFieldChanged( row, col, true );

                        if( field_->isResolved() )
                            emit gameFinished();
                    }
                    else if ( field_->cell( row, col ) == 0 )
                    {
                        //wrong step

                        stat_[ nickName ] -= field_->freeCells();

                        emit gameFieldChanged( row, col, false );
                    }

                }
            }
        }
    }
}

void Game::timerEvent( QTimerEvent * event )
{
    if( saveConnectionTimerId_ == event->timerId() )
    {
        socket_->ping();
    }
    else if( reconnectTimerId_ == event->timerId() )
    {
        resetTimer( saveConnectionTimerId_ );
        resetTimer( reconnectTimerId_ );

        if( socket_ )
        {
            socket_->abort();
            socket_->deleteLater();
        }

        socket_ = nullptr;

        connectToWebSocket();
    }
}

void Game::startUniqueTimer( int & id, int interval )
{
    if( -1 == id )
    {
        id = startTimer( interval );
    }
}

void Game::resetTimer( int & id )
{
    if( -1 != id )
    {
        killTimer( id );
        id = -1;
    }
}

const QVector< QVector< std::size_t > > & Game::field() const
{
    assert( field_ );
    return field_->frontField();
}

const QMap< QString, int > & Game::stat() const
{
    return stat_;
}
