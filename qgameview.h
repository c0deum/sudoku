#ifndef QGAMEVIEW_H
#define QGAMEVIEW_H

#include "game.h"

#include <QImage>

#include <QWidget>


class QGameView : public QWidget
{
    Q_OBJECT
public:
    explicit QGameView( QWidget * parent = nullptr );

    const Game * game() const;

protected:
    virtual void paintEvent( QPaintEvent * event );    
    virtual void timerEvent( QTimerEvent * event );
private:
    void loadTheme();

signals:

public slots:
    void onGameCreated();
    void onGameFinished();
    void onGameFieldChanged( std::size_t row, std::size_t col, bool success );

private:
    Game * game_ = {nullptr};
    bool lastCellResult_;
    QPair< std::size_t, std::size_t > lastCell_;
    //QStringList keywords_;
    QVector< QImage > images_;
    QVector< std::size_t > indexMap_;
    int startNewGameTimerId_ = -1;
    int startNewGameDelay = 5000;
};

#endif // QGAMEVIEW_H
