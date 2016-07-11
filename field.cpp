#include <QDebug>
#include <QList>

#include "field.h"

Field::Field( std::size_t dim )
    : dim_( dim )
    , sqrDim_( dim * dim )
{
    frontField_.resize( sqrDim_ );
    backField_.resize( sqrDim_ );

    for( std::size_t i = 0; i < sqrDim_; ++i )
    {
        frontField_[ i ].resize( sqrDim_ );
        backField_[ i ].resize( sqrDim_ );
    }

    reset();
}

Field::~Field()
{
}

void Field::reset()
{
    for( std::size_t row = 0; row < sqrDim_; ++row )
    {
        for( std::size_t col = 0; col < sqrDim_; ++col )
        {
            frontField_[ row ][ col ] = 0;

            backField_[ row ][ col ].clear();

            for( std::size_t val = 1; val <= sqrDim_; ++val )
            {
                backField_[ row ][ col ] << val;
            }
        }
    }
}

bool Field::isReady() const
{    
    /*
    for( std::size_t row = 0; row < sqrDim_; ++row )
    {
        for( std::size_t col = 0; col < sqrDim_; ++col )
        {
            if( ( frontField_[ row ][ col ] == 0 ) && ( backField_[ row ][ col ].size() == 1 ) )
                return true;
        }
    }
    */
    return false;
}

bool Field::isResolved() const
{   
    for( std::size_t row = 0; row < sqrDim_; ++row )
    {
        for( std::size_t col = 0; col < sqrDim_ ; ++col )
        {
            if( frontField_[ row ][ col ] == 0 )
            {
                return false;
            }
        }
    }

    //todo test for rows, lines, quads

    return true;
}

bool Field::set( std::size_t row, std::size_t col, std::size_t val )
{
    bool result = ( frontField_[ row ][ col ] == 0 ) && backField_[ row ][ col ].contains( val );

    if( !result )
        return result;

    frontField_[ row ][ col ] = val;

    //std::cout << row << "\t" << col << "\t" << val << std::endl;

    for( std::size_t index = 0; index < sqrDim_; ++index )
    {
        backField_[ row ][ index ] -= val;
        backField_[ index ][ col ] -= val;
    }

    for( std::size_t quadRow = row / dim_ * dim_; quadRow < row / dim_ * dim_ + dim_; ++quadRow )
    {
        for( std::size_t quadCol = col / dim_ * dim_; quadCol < col / dim_ * dim_ + dim_; ++quadCol )
        {
            backField_[ quadRow][ quadCol ] -= val;
        }
    }

    return result;
}

void Field::unset( std::size_t row, std::size_t col )
{
    if( frontField_[ row ][ col ] == 0 )
        return;

    for( std::size_t index = 0; index < sqrDim_; ++index )
    {
        backField_[ row ][ index ] += frontField_[ row ][ col ];
        backField_[ index ][ col ] += frontField_[ row ][ col ];
    }

    for( std::size_t quadRow = row / dim_ * dim_; quadRow < row / dim_ * dim_ + dim_; ++quadRow )
    {
        for( std::size_t quadCol = col / dim_ * dim_; quadCol < col / dim_ * dim_ + dim_; ++quadCol )
        {
            backField_[ quadRow][ quadCol ] += frontField_[ row ][ col ];
        }
    }

    frontField_[ row ][ col ] = 0;

}

std::size_t Field::cell(std::size_t row, std::size_t col) const
{
    return frontField_[ row ][ col ];
}

const QSet< std::size_t > & Field::cellVariants( std::size_t row, std::size_t col ) const
{
    return backField_[ row ][ col ];
}

bool Field::resolveNext( QPair<std::size_t, std::size_t> &pos ) const
{    
    int min = 0;

    bool minSaved = false;

    for( std::size_t row = 0; !minSaved && ( row < sqrDim_ ); ++row )
    {
        for( std::size_t col = 0; !minSaved && ( col < sqrDim_ ); ++col )
        {
            if( ( frontField_[ row ][ col ] == 0 ) && ( backField_[ row ][ col ].size() > 0 ) )
            {
                pos.first = row;
                pos.second = col;                
                min = backField_[ row ][ col ].size();
                minSaved = true;
            }
        }
    }

    for( std::size_t row = 0; row < sqrDim_; ++row )
    {
        for( std::size_t col = 0; col < sqrDim_; ++col )
        {
            if( ( frontField_[ row ][ col ] == 0 ) && ( backField_[ row ][ col ].size() > 0 ) && ( backField_[ row ][ col ].size() < min ) )
            {
                pos.first = row;
                pos.second = col;
                min = backField_[ row ][ col ].size();
            }
        }
    }

    return min > 0;
}

bool Field::resolveNextUniqueness( QPair<std::size_t, std::size_t> &pos ) const
{
    for( std::size_t row = 0; row < sqrDim_; ++row )
    {
        for( std::size_t col = 0; col < sqrDim_; ++col )
        {
            if( ( frontField_[ row ][ col ] == 0 ) && ( backField_[ row ][ col ].size() == 1 ) )
            {
                pos.first = row;
                pos.second = col;
                return true;
            }
        }
    }
    return false;
}

bool Field::resolve()
{    
    QPair< std::size_t, std::size_t > pos;    

    while( !isResolved() && resolveNextUniqueness( pos ) )
    {
        set( pos.first, pos.second, backField_[ pos.first][ pos.second ].values().first() );
    }
    return isResolved();
}

std::size_t Field::sqrDim() const
{
    return sqrDim_;
}

std::size_t Field::dim() const
{
    return dim_;
}

std::istream & operator >>( std::istream & is, Field & field )
{
    field.reset();

    for( std::size_t row = 0; row < field.sqrDim(); ++row )
    {
        for( std::size_t col = 0; col < field.sqrDim(); ++col )
        {
            std::size_t val;
            is >> val;
            if( val > 0 )
            {
                field.set( row, col, val );
            }
        }
    }
    return is;
}

std::ostream & operator <<( std::ostream & os, const Field & field )
{
    for( std::size_t row = 0; row < field.sqrDim(); ++row )
    {
        if( row % field.dim() == 0 )
        {
            for( std::size_t col = 0; col < field.sqrDim() + field.dim() + 1; ++col )
            {
                os << '-';
            }
            os << std::endl;
        }

        for( std::size_t col = 0; col < field.sqrDim(); ++col )
        {
            if( col % field.dim() == 0 )
            {
                os << '|';
            }
            os << field.cell( row, col );
        }

        os << "|" << std::endl;
    }

    for( std::size_t col = 0; col < field.sqrDim() + field.dim() + 1; ++col )
    {
        os << '-';
    }
    os << std::endl;

    return os;
}

Field Field::generate( std::size_t dim )
{
    /*
    Field field( dim );

    //generate random row
    for( std::size_t col = 0; col < field.sqrDim(); ++col )
    {
        field.set( 0, col, field.cellVariants( 0, col ).values()[ qrand() % field.cellVariants( 0, col ).values().size() ] );
    }

    //generate first dim rows
    for( std::size_t row = 1; row < field.dim(); ++row )
    {
        for( std::size_t col = field.dim(); col < field.sqrDim(); ++col )
        {
            field.set( row, col - field.dim(), field.cell( row - 1, col ) );
        }

        for( std::size_t col = 0; col < field.dim(); ++col )
        {
            field.set( row, field.sqrDim() - field.dim() + col, field.cell( row - 1, col ) );
        }
    }

    //generate all rows
    for( std::size_t row = field.dim(); row < field.sqrDim(); ++row )
    {
        for( std::size_t col = 1; col < field.sqrDim(); ++col )
        {
            field.set( row, col - 1, field.cell( row - field.dim(), col ) );
        }
        field.set( row, field.sqrDim() - 1, field.cell( row - field.dim(), 0 ) );
    }


    //mix

    for( std::size_t step = 0; step < field.sqrDim() * field.sqrDim(); ++step )
    {
        int variant = rand() % 5;
        //transpose field
        if( variant == 0 )
        {
        }
        //swap two rows
        else if( variant == 1 )
        {
        }
        //swap two columns
        else if( variant == 2 )
        {
        }
        //swap two block rows
        else if( variant == 3 )
        {
        }
        //swap two block columns
        else if( variant == 4)
        {
        }
    }

    */
    Field field( dim );

    QSet< std::size_t > baseRow;

    for( std::size_t val = 1; val <= field.sqrDim(); ++val )
    {
        baseRow << val;
    }

    QVector< QVector< std::size_t > > matrix;

    matrix.resize( field.sqrDim() );

    for( std::size_t row = 0; row < field.sqrDim(); ++row )
    {
        matrix[ row ].resize( field.sqrDim() );
    }


    //generate random row
    for( std::size_t col = 0; col < field.sqrDim(); ++col )
    {

        std::size_t val = baseRow.values()[ qrand() % baseRow.size() ];
        matrix[ 0 ][ col ] = val;
        baseRow -= val;
    }

    //generate first dim rows
    for( std::size_t row = 1; row < field.dim(); ++row )
    {
        for( std::size_t col = field.dim(); col < field.sqrDim(); ++col )
        {
            matrix[ row ][ col - field.dim() ] = matrix[ row - 1 ][ col ];
        }

        for( std::size_t col = 0; col < field.dim(); ++col )
        {
            matrix[ row ][ field.sqrDim() - field.dim() + col ] = matrix[ row - 1 ][ col ];
        }
    }

    //generate all rows
    for( std::size_t row = field.dim(); row < field.sqrDim(); ++row )
    {
        for( std::size_t col = 1; col < field.sqrDim(); ++col )
        {
            matrix[ row ][ col - 1 ] = matrix[ row - field.dim() ][ col ];
        }
        matrix[ row ][ field.sqrDim() - 1 ] = matrix[ row - field.dim() ][ 0 ];
    }

    qDebug() << matrix;

    //mix

    QList< std::size_t > shiftList;
    for( std::size_t val = 0; val < field.dim(); ++val )
    {
        shiftList << val;
    }

    for( std::size_t step = 0; step < field.sqrDim() * field.sqrDim(); ++step )
    {
        int variant = rand() % 5;
        //transpose field
        if( variant == 0 )
        {
            for( std::size_t row = 0; row < field.sqrDim() - 1; ++row  )
            {
                for( std::size_t col = row; col < field.sqrDim(); ++col )
                {
                    std::size_t tmp = matrix[ row ][ col ];
                    matrix[ row ][ col ] = matrix[ col ][ row ];
                    matrix[ col ][ row ] = tmp;
                }
            }
        }
        //swap two rows in block
        else if( variant == 1 )
        {
            std::size_t block = qrand() % field.dim();
            mixList( shiftList );

            for( std::size_t col = 0; col < field.sqrDim(); ++col )
            {
                qSwap( matrix[ block * field.dim() + shiftList[ 0 ] ][ col ], matrix[ block * field.dim() + shiftList[ 1 ] ][ col ] );
            }
        }
        //swap two columns in block
        else if( variant == 2 )
        {
            std::size_t block = qrand() % field.dim();
            mixList( shiftList );

            for( std::size_t row = 0; row < field.sqrDim(); ++row )
            {
                qSwap( matrix[ row ][ block * field.dim() + shiftList[ 0 ] ], matrix[ row ][ block * field.dim() + shiftList[ 1 ] ] );
            }
        }
        //swap two block rows
        else if( variant == 3 )
        {
            mixList( shiftList );

            for( std::size_t row = 0; row < field.dim(); ++row )
            {
                for( std::size_t col = 0; col < field.sqrDim(); ++col )
                {
                    qSwap( matrix[ row + shiftList[ 0 ] * field.dim() ][ col ], matrix[ row + shiftList[ 1 ] * field.dim() ][ col ] );
                }
            }
        }
        //swap two block columns
        else if( variant == 4)
        {
            mixList( shiftList );

            for( std::size_t col = 0; col < field.dim(); ++col )
            {
                for( std::size_t row = 0; row < field.sqrDim(); ++row )
                {
                    qSwap( matrix[ row ][ col + shiftList[ 0 ] * field.dim() ], matrix[ row ][ col + shiftList[ 1 ] * field.dim() ] );
                }
            }
        }
    }

    //save matrix to field
    for( std::size_t row = 0; row < field.sqrDim(); ++row )
    {
        for( std::size_t col = 0; col < field.sqrDim(); ++col )
        {
            field.set( row, col, matrix[ row ][ col ] );
        }
    }


    std::cout << "Base Field:" << std::endl;

    std::cout << field;

    //substruct cells

    QSet< QPair< std::size_t, std::size_t > > cellsSet;

    for( std::size_t row = 0; row < field.sqrDim(); ++row )
    {
        for( std::size_t col = 0; col < field.sqrDim(); ++col )
        {
            cellsSet << qMakePair( row, col );
        }
    }

    while( !cellsSet.isEmpty() )
    {
        QPair< std::size_t, std::size_t > pos = cellsSet.values()[ qrand() % cellsSet.size() ];

        std::size_t val = field.cell( pos.first, pos.second );

        field.unset( pos.first, pos.second );

        if( !field.isResolvable() )
        {
            field.set( pos.first, pos.second, val );
        }
        cellsSet -= pos;
    }

    //QSet< QPair< std::size_t, std::size_t > > cellsSet;


    return field;
}

bool Field::isResolvable() const
{
    Field field = *this;
    return field.resolve();
}

void Field::mixList( QList< std::size_t > &list )
{
    for( std::size_t step = 0; step < list.size() * list.size(); ++step )
    {
        qSwap( list[ 0 ], list[ 1 + qrand() % ( list.size() - 1 ) ] );
    }
}
