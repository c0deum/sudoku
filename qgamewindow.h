#ifndef QGAMEWINDOW_H
#define QGAMEWINDOW_H

#include <QMap>

#include <QWidget>


class QGameView;
class QListWidget;

class QGameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QGameWindow( QWidget * parent = nullptr );
protected:
    void resizeEvent( QResizeEvent * event );
private:

signals:

public slots:

private slots:
    void onGameCreated();
    void onGameFinished();
    void onGameFieldChanged();

private:
    QGameView * gameView_;
    QListWidget * gameStatList_;
    QListWidget * gamesStatList_;
    QListWidget * stepsList_;

    QMap< QString, int > gamesStat_;
};

#endif // QGAMEWINDOW_H
