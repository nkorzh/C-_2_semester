#include "Image.h"

Image::Image(std::vector<std::vector<Image::Pixel>> table)
    : m_width(table.size()), m_height(table.at(0).size()), m_table(std::move(table))
{}

Image::Pixel::Pixel(int red, int green, int blue)
    : m_red(red)
    , m_green(green)
    , m_blue(blue)
{}

const Image::Pixel & Image::GetPixel(size_t columnId, size_t rowId) const
{
    return m_table[columnId][rowId];
}

const Image::Pixel &Image::GetTopPixel( size_t columnId, size_t rowId ) const {
    rowId = rowId > 0 ? rowId - 1 : m_height - 1;
    return GetPixel(columnId, rowId);
}

const Image::Pixel &Image::GetBottomPixel( size_t columnId, size_t rowId ) const {
    rowId = rowId < m_height - 1 ? rowId + 1 : 0;
    return GetPixel(columnId, rowId);
}

const Image::Pixel &Image::GetRightPixel( size_t columnId, size_t rowId ) const {
    columnId = columnId < m_width - 1 ? columnId + 1 : 0;
    return GetPixel(columnId, rowId);
}

const Image::Pixel &Image::GetLeftPixel( size_t columnId, size_t rowId ) const {
    columnId = columnId > 0 ? columnId - 1 : m_width - 1;
    return GetPixel(columnId, rowId);
}

double Image::GetRedDif( const Image::Pixel &a, const Image::Pixel &b ) {
    return static_cast<double>(a.m_red - b.m_red);
}

double Image::GetGreenDif( const Image::Pixel &a, const Image::Pixel &b ) {
    return static_cast<double>(a.m_green - b.m_green);
}

double Image::GetBlueDif( const Image::Pixel &a, const Image::Pixel &b ) {
    return static_cast<double>(a.m_blue - b.m_blue);
}

void Image::rewriteRowFrom( size_t columnId, size_t rowId ) {
    for (size_t x = columnId; x < m_width - 1; ++x) {
        m_table[x][rowId] = m_table[x + 1][rowId];
    }
}

void Image::rewriteColumnFrom( size_t columnId, size_t rowId ) {
    for (size_t y = rowId; y < m_height - 1; ++y) {
        m_table[columnId][y] = m_table[columnId][y + 1];
    }
}
