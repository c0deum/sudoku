#include <QCoreApplication>

#include <QDateTime>

#include <iostream>
#include <fstream>

#include "field.h"

int main( int argc, char * argv[] )
{           
    qsrand( QDateTime::currentMSecsSinceEpoch() );
    /*
//    std::cout << "Sudoku dimension?" << std::endl;

    int dim;

    std::fstream fin;
    fin.open( "/home/c0deum/test.txt" );

    //std::cin >> dim;

    fin >> dim;

    Field field( dim );

//    std::cout << "Sudoku field(" << field.sqrDim() << "x" << field.sqrDim() << ")" << std::endl;

    //std::cin >> field;

    fin >> field;

    fin.close();

    /*
    if( !field.isReady() )
    {
        std::cout << "Not valid task" << std::endl;
        return -1;
    }    
    */

    Field field = Field::generate( 3 );

    std::cout << "Sudoku field:" << std::endl;

    std::cout << field;

    field.resolve();

    if( field.isResolved() )
    {
        std::cout << "okay" << std::endl;
    }

    /*
    std::cout << "Sudoku back sizes:" << std::endl;

    for( int i = 0; i < field.sqrDim(); ++i )
    {
        for( int j = 0; j < field.sqrDim(); ++j )
        {
            std::cout << field.cellVariants( i, j ).size() << " ";
        }

        std::cout << std::endl;
    }
    */

    /*
    if ( field.resolve() )
    {

        std::cout << "Resolve:" << std::endl;

        std::cout << field;
    }
    else
    {
        std::cout << "Error:" << std::endl;

        std::cout << field;
    }
    */

    return 0;
}
