#ifndef GAME_H
#define GAME_H

#include <cstddef>

#include <QObject>

class Field;

class QWebSocket;

class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game( QObject * parent = nullptr );
    virtual ~Game();
public:
    void createNewGame( std::size_t dim );

    const QVector< QVector< std::size_t > > & field() const;

protected:
    virtual void timerEvent( QTimerEvent * event );
private:
    void connectToWebSocket();

    void startUniqueTimer( int & id, int interval );
    void resetTimer( int & id );

signals:
    void gameCreated();
    void gameFieldChanged( std::size_t, std::size_t, bool success );
    //void gameStarted();
    void gameFinished();

public slots:
    void onWebSocketConnected();
    void onWebSocketError();
    void onTextMessageReceived( const QString & message );

private:
    Field * field_ = {nullptr};
    QWebSocket * socket_ = {nullptr};
    int socketPort_ = 15619;
    int saveConnectionTimerId_ = {-1};
    int saveConnectionInterval_ = {25000};
    int reconnectTimerId_ = {-1};
    int reconnectInterval_ = {1000};
};

#endif // GAME_H
