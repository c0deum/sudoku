#include <QApplication>

#include <QDateTime>

#include <iostream>
#include <fstream>

#include "field.h"

#include "qgameview.h"


int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    QGameView gameView;

    gameView.show();

    /*
    quint64 start = QDateTime::currentMSecsSinceEpoch();
    qsrand( start );

    Field * field = Field::generate( 3 );

    std::cout << "Sudoku field:" << std::endl;

    std::cout << *field;

    if ( field->resolve() )
    {
        std::cout << "Resolve:" << std::endl;
    }
    else
    {
        std::cout << "Error:" << std::endl;        
    }

    std::cout << *field;

    std::cout << ( QDateTime::currentMSecsSinceEpoch() - start ) / 1000.0 << "sec" << std::endl;

    */

    return app.exec();
}
