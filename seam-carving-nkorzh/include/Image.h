#pragma once

#include <vector>

struct Image
{
    size_t m_width, m_height;
    struct Pixel
    {
        Pixel(int red, int green, int blue);
        int m_red{0};
        int m_green{0};
        int m_blue{0};
    };

    Image(std::vector<std::vector<Pixel>> table);

    const Pixel & GetPixel(size_t columnId, size_t rowId) const;
    const Pixel & GetTopPixel(size_t columnId, size_t rowId) const;
    const Pixel & GetBottomPixel(size_t columnId, size_t rowId) const;
    const Pixel & GetRightPixel(size_t columnId, size_t rowId) const;
    const Pixel & GetLeftPixel(size_t columnId, size_t rowId) const;
    static double GetRedDif(const Pixel & a, const Pixel & b);
    static double GetGreenDif(const Pixel & a, const Pixel & b);
    static double GetBlueDif(const Pixel & a, const Pixel & b);


    void rewriteRowFrom(size_t columnId, size_t rowId);
    void rewriteColumnFrom(size_t columnId, size_t rowId);
    std::vector<std::vector<Pixel>> m_table;
};
