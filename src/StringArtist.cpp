#include "StringArtist.h"

#include <iostream>
#include <math.h>
#include "BresenhamLineIterator.h"

namespace {
    float CANVAS_LINE_OPACITY = 1.0f;
}

StringArtist::StringArtist(const Image& image, unsigned int numPins, float draftOpacity, float threshold, unsigned int skipped_neighbors, unsigned int scaleFactor, unsigned int numWindings) :
    m_imagePtr(&image),
    m_numPins(numPins),
    m_draftOpacity(draftOpacity),
    m_threshold(threshold),
    m_skippedNeighbors(skipped_neighbors),
    m_scaleFactor(scaleFactor),
    m_iteration(0), 
    m_windings(numWindings)
{
    m_canvas = StringArtImage(m_imagePtr->size() * m_scaleFactor, m_numPins);
    m_draft = StringArtImage(m_imagePtr->size(), m_numPins);
    m_adjacency.resize(m_imagePtr->size(), std::vector<bool>(m_imagePtr->size(), false));
    m_indices = std::vector<size_t>(); 
}

void StringArtist::windString()
{
    // Wind thread around pins until image can't be improved.
    size_t currentPinId = 0;
    std::cout << "start winding" << std::endl;
    for (unsigned int i = 0; i < m_windings; i++)
    {
        m_indices.push_back(currentPinId); 

        size_t nextPinId;
        if (!findNextPin(currentPinId, nextPinId))
            break;

        m_iteration++;
        //std::cout << m_iteration << std::endl;
        drawLine(m_draft, currentPinId, nextPinId, m_draftOpacity);
        drawLine(m_canvas, currentPinId, nextPinId, CANVAS_LINE_OPACITY);

        m_adjacency[currentPinId][nextPinId] = true;
        m_adjacency[nextPinId][currentPinId] = true;
        currentPinId = nextPinId;

    }
    std::cout << "Done after "<< m_iteration << " iterations" << std::endl;
}

bool StringArtist::findNextPin(const size_t currentPinId, size_t& bestPinId) const
{
    float bestScore = std::numeric_limits<float>::infinity();

    for (size_t nextPinId = 0; nextPinId < m_numPins; ++nextPinId)
    {
        int diff = static_cast<int>(nextPinId) - static_cast<int>(currentPinId); 
        int dist = std::min(diff % m_numPins, -diff % m_numPins);
        if (dist < m_skippedNeighbors || m_adjacency[currentPinId][nextPinId])
            continue;

        unsigned int pixelChanged;
        float score = lineScore(currentPinId, nextPinId, pixelChanged);
        
        if (pixelChanged > 0 && score < bestScore)
        {
            bestScore = score;
            bestPinId = nextPinId;
            // std::cout << bestScore << std::endl;
        }
    }
    return bestScore < m_threshold;
    // return true; 
}

float easeInOut(float x) {
    return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
}

// Or use a sigmoid contrast function:
float sigmoidContrast(float x, float midpoint = 128.0f, float factor = 10.0f) {
    float normalized = (x - midpoint) / 128.0f;
    float sigmoid = 1.0f / (1.0f + exp(-factor * normalized));
    return (sigmoid - 0.5f) * 255.0f + midpoint;
}

float StringArtist::lineScore(const size_t currentPinId, const size_t nextPinId, unsigned int& pixelChanged) const
{
    pixelChanged = 0;
    float score = 0.f;
    Point2D currentPin = m_draft.getPin(currentPinId);
    Point2D nextPin = m_draft.getPin(nextPinId);
    Point2D diff = nextPin - currentPin;
    int distance = std::max(std::abs(diff[0]), std::abs(diff[1]));

    for (const Point2D& pixel : BresenhamLineIterator(currentPin, nextPin))
    {
        float pixVal = m_imagePtr->getPixelValue(pixel); 
        float draftVal = (255 - m_draft.getPixelValue(pixel)); 

        // std::cout << pixVal  << " " <<  draftVal << std::endl; 

        
        float n = 0.4; 

        score += 
        // absval; 
        // sqrt(draftVal) + sqrt(pixVal); 
        // pow(pixVal, n) + pow(draftVal, n); 
        255*((pow(easeInOut(pixVal / 255), n) + pow(easeInOut(draftVal / 255), n)));
        // sigmoidContrast(pixVal) + sigmoidContrast(draftVal); 
        // 255 * (pow(pixVal / 255, n) + pow(draftVal / 255, n));
        // pixVal + draftVal;  
        // pow(pixVal, n) + pow(draftVal, n); 
        // pow(draftVal, 2) + pow(pixVal, 2);
        // sqrt(draftVal + pixVal); 
        ++pixelChanged;
    }
    return score / distance;
}

void StringArtist::drawLine(StringArtImage& image, const size_t currentPinId, const size_t nextPinId, const float opacity)
{
    for (const Point2D& pixel : BresenhamLineIterator(image.getPin(currentPinId), image.getPin(nextPinId)))
    {
        int value = 0;
        if (opacity < 1.0f)
        {
            value = image.getPixelValue(pixel) * (1 - opacity);
        }
        image.setPixelValue(pixel, value);
    }
}

void StringArtist::saveImage(std::FILE* outputFile)
{
    std::fprintf(outputFile, "P5\n%ld %ld\n255\n", m_canvas.size(), m_canvas.size());
    std::fwrite(m_canvas.getFirstPixelPointer(), m_canvas.size(), m_canvas.size(), outputFile);
    std::fclose(outputFile);
}

std::vector<size_t> StringArtist::getIndices() {
    return m_indices; 
}