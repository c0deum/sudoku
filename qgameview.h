#ifndef QGAMEVIEW_H
#define QGAMEVIEW_H

#include "game.h"

#include <QWidget>

class QGameView : public QWidget
{
    Q_OBJECT
public:
    explicit QGameView( QWidget * parent = nullptr );
protected:
    virtual void paintEvent( QPaintEvent * event );
    virtual void resizeEvent( QResizeEvent * event );
private:
    void drawField();


signals:

public slots:
    void onGameCreated();
    void onGameFinished();
    void onGameFieldChanged( std::size_t row, std::size_t col, bool success );

private:
    Game * game_ = {nullptr};
    bool lastCellResult_;
    QPair< std::size_t, std::size_t > lastCell_;

};

#endif // QGAMEVIEW_H
