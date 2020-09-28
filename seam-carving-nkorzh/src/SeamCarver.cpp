#include <cmath>

#include "SeamCarver.h"


SeamCarver::SeamCarver(Image image)
    : m_image(std::move(image))
{}

const Image& SeamCarver::GetImage() const
{
    return m_image;
}

size_t SeamCarver::GetImageWidth() const
{
    return m_image.m_width;
}

size_t SeamCarver::GetImageHeight() const
{
    return m_image.m_height;
}

double SeamCarver::GetPixelEnergy(size_t columnId, size_t rowId) const
{
    auto SQR = [] (double value) { return value * value; };
    const Image::Pixel &
        Left = m_image.GetLeftPixel(columnId, rowId),
        Right = m_image.GetRightPixel(columnId, rowId),
        Top = m_image.GetTopPixel(columnId, rowId),
        Bottom = m_image.GetBottomPixel(columnId, rowId);
    double
        Rx = Image::GetRedDif(Right, Left),
        Gx = Image::GetGreenDif(Right, Left),
        Bx = Image::GetBlueDif(Right, Left),
        Ry = Image::GetRedDif(Bottom, Top),
        Gy = Image::GetGreenDif(Bottom, Top),
        By = Image::GetBlueDif(Bottom, Top);
    double
        deltaX = SQR(Rx) + SQR(Gx) + SQR(Bx),
        deltaY = SQR(Ry) + SQR(Gy) + SQR(By);
    /* better not to count sqrt for performance
     * max energy in cell is 255^2 * 3 * 2 which is < MAX_DOUBLE
     * in the lowest/rightest cell this value is 255^2 * 3 * 2 * m_height or m_width
     * which is 3 901 500 000 < MAX_DOUBLE for longest side of 80 MP photo
     */
    return sqrt(deltaX + deltaY);
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    /// start from the left, going right
    std::vector<std::vector<SmartPixel>> energy;
    getEnergy(energy);

    size_t
            rightest = GetImageWidth() - 1,
            lowest = GetImageHeight() - 1;

    for (size_t y = 0; y <= lowest; y++)
        energy[0][y].second = y;

    for (size_t column = 1; column <= rightest; ++column) {
        setPixelAncestor(energy[column][0], energy[column - 1][0], 0,
                         energy[column - 1][1], 1,
                         energy[column - 1][1], 1);

        for (size_t y = 1; y < lowest; y++)
            setPixelAncestor(energy[column][y], energy[column - 1][y - 1], y - 1,
                             energy[column - 1][y], y,
                             energy[column - 1][y + 1], y + 1);
        if (lowest >= 1)
            setPixelAncestor(energy[column][lowest], energy[column - 1][lowest], lowest,
                    energy[column - 1][lowest - 1], lowest - 1,
                    energy[column - 1][lowest - 1], lowest - 1);
    }

    double minSum = 400000.0;
    size_t minInd = 0;
    for (size_t y = 0; y <= lowest; ++y) {
        if (energy[rightest][y].first < minSum) {
            minSum = energy[rightest][y].first;
            minInd = y;
        }
    }
    Seam seam(GetImageWidth());
    seam[rightest] = minInd;
    for (size_t x = rightest; x > 0 ; --x) {
        seam[x - 1] = energy[x][seam[x]].second;
    }
    return seam;
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    /// start from top, going down
    std::vector<std::vector<SmartPixel>> energy;
    getEnergy(energy);

    size_t
        rightest = GetImageWidth() - 1,
        lowest = GetImageHeight() - 1;

    for (size_t x = 0; x <= rightest; x++)
        energy[x][0].second = x;

    for (size_t row = 1; row <= lowest; ++row) {
        setPixelAncestor(energy[0][row], energy[1][row - 1], 1,
                energy[1][row - 1], 1, energy[0][row - 1], 0);

        for (size_t x = 1; x < rightest; x++)
            setPixelAncestor(energy[x][row], energy[x + 1][row - 1], x + 1,
                    energy[x][row - 1], x, energy[x - 1][row - 1], x - 1);

        if (row >= 1)
            setPixelAncestor(energy[rightest][row], energy[rightest][row - 1], rightest,
                         energy[rightest][row - 1], rightest,
                         energy[rightest - 1][row - 1],rightest - 1);
    }

    double minSum = std::numeric_limits<double>::max();
    size_t minInd = 0;
    for (size_t x = 0; x <= rightest; ++x) {
        if (energy[x][lowest].first < minSum) {
            minSum = energy[x][lowest].first;
            minInd = x;
        }
    }
    Seam seam(GetImageHeight());
    seam[lowest] = minInd;
    for (size_t y = lowest; y > 0 ; --y) {
        seam[y - 1] = energy[seam[y]][y].second;
    }
    return seam;
}

void SeamCarver::RemoveHorizontalSeam(const Seam& seam)
{
    for (size_t x = 0; x < m_image.m_width; x++) {
        m_image.rewriteColumnFrom(x, seam[x]);
        m_image.m_table[x].pop_back();
    }
    m_image.m_height--;
}

void SeamCarver::RemoveVerticalSeam(const Seam& seam)
{
    for (size_t y = 0; y < m_image.m_height; y++)
        m_image.rewriteRowFrom(seam[y], y);
    m_image.m_table.pop_back();
    m_image.m_width--;
}

void SeamCarver::getEnergy(std::vector<std::vector<SmartPixel>> & energy) const {
    energy.resize(GetImageWidth());
    for (size_t column = 0; column < GetImageWidth(); column++) {
        energy[column].reserve(GetImageHeight());
        for (size_t row = 0; row < GetImageHeight(); row++) {
            energy[column].emplace_back(GetPixelEnergy(column, row), 0);
        }
    }
}

void SeamCarver::setPixelAncestor( SeamCarver::SmartPixel &Pixel, const SeamCarver::SmartPixel &left, size_t leftInd,
                                   const SeamCarver::SmartPixel &mid, size_t midInd, const SeamCarver::SmartPixel &right, size_t rightInd ) const {
    SmartPixel minPixel;
    if (left.first <= mid.first)
        minPixel = {left.first, leftInd};
    else
        minPixel = {mid.first, midInd};

    if (minPixel.first <= right.first) {
        Pixel.first += minPixel.first;
        Pixel.second = minPixel.second;
    } else {
        Pixel.first += right.first;
        Pixel.second = rightInd;
    }
}
