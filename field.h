#ifndef FIELD_H
#define FIELD_H

#include <cstddef>
#include <iostream>
#include <QVector>
#include <QSet>


class Field
{
public:
    explicit Field( std::size_t dim );
    Field( const QVector< QVector< std::size_t > > & matrix, std::size_t dim );

    ~Field();
public:
    //one step, refill cell, row, column and quad
    bool set( std::size_t row, std::size_t col, std::size_t val );

    bool isResolved() const;

    bool isResolvable() const;

    bool resolve();

    //random field
    static Field * generate( std::size_t dim );

    std::size_t cell( std::size_t row, std::size_t col ) const;

    const QSet< std::size_t > & cellVariants( std::size_t row, std::size_t col ) const;

    bool resolveNext( QPair< std::size_t, std::size_t > & pos ) const;
    bool resolveNextUniqueness( QPair< std::size_t, std::size_t > & pos ) const;

    std::size_t sqrDim() const;
    std::size_t dim() const;

    void reset();

    bool isStepAvailable( std::size_t row, std::size_t col, std::size_t val ) const;

    const QVector< QVector< std::size_t > > & frontField() const;


private:
    static void mixList( QList< std::size_t > & list );
private:
    std::size_t dim_;
    std::size_t sqrDim_;
    QVector< QVector< std::size_t > > frontField_;
    QVector< QVector< QSet< std::size_t > > > backField_;
};

std::istream & operator >> ( std::istream & is, Field & field );
std::ostream & operator << ( std::ostream & os, const Field & field );

#endif // FIELD_H
