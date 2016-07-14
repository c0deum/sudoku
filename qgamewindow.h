#ifndef QGAMEWINDOW_H
#define QGAMEWINDOW_H

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
    void onStatChanged();


private:
    QGameView * gameView_;
    QListWidget * statLis_;
};

#endif // QGAMEWINDOW_H
