#include "TimeSignature.hpp"


TimeSignature::TimeSignature()
{
    this->numBeats = 4;
    this->beatType = 4;
    this->beatUnits = 1024;
}

TimeSignature::TimeSignature(int numBeats, int beatType)
{
    if (numBeats < 1 || beatType < 1) {
        std::cerr << "TimeSignature arguments must be greater than 0" << std::endl;
        throw std::invalid_argument("TimeSignature arguments must be greater than 0");
    }
    if ((beatType & (beatType - 1)) != 0) {
        std::cerr << "TimeSignature beatType must be a power of 2" << std::endl;
        throw std::invalid_argument("TimeSignature beatType must be a power of 2");
    }
    this->numBeats = numBeats;
    this->beatType = beatType;
    this->beatUnits = static_cast<int>((4.0f / beatType) * numBeats * 256);
}
