#ifndef TIME_SIGNATURE_HPP
#define TIME_SIGNATURE_HPP

#include <iostream>
#include <stdexcept>


class TimeSignature
{
public:
    TimeSignature();
    TimeSignature(int numBeats, int beatType);

    int getBeatUnits() { return this->beatUnits; }

protected:
    int numBeats;
    int beatType;
    int beatUnits;
};

#endif
