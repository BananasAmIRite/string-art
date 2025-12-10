#pragma once

#include <vector>
#include "StringArtImage.h"
#include "Point2D.h"

class StringArtist
{
public:
    StringArtist(const Image& image, unsigned int numPins, float draftOpacity, float threshold, unsigned int skippedNeighbors, unsigned int scaleFactor, unsigned int numWindings, unsigned int method);
    ~StringArtist(){};
    void windString();
    bool findNextPin(const size_t currentPinId, size_t& bestPinId) const;
    float lineScore(const size_t currentPinId, const size_t nextPinId, unsigned int& pixelChanged) const;
    void drawLine(StringArtImage& image, const size_t currentPinId, const size_t nextPinId, const float opacity=1.0f);
    void saveImage(std::FILE* outputFile);
    std::vector<size_t> getIndices(); 
private:
    const Image* m_imagePtr;
    StringArtImage m_canvas;
    StringArtImage m_draft;
    std::vector<std::vector<bool>> m_adjacency;
    size_t m_numPins;
    float m_draftOpacity;
    float m_threshold;
    unsigned int m_skippedNeighbors;
    unsigned int m_scaleFactor;
    unsigned int m_iteration;
    unsigned int m_windings; 
    unsigned int m_method;
    std::vector<size_t> m_indices; 
};
