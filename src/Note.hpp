#ifndef NOTE_HPP
#define NOTE_HPP

#include <vector>
#include <iostream>
#include <stdexcept>

#include "Definitions.hpp"


struct NoteAttributes
{
    Dynamic dynamic;
    float beatUnitsPerSecond;
    std::vector<std::pair<NoteName, Accidental>> beginTie;
    std::vector<std::pair<NoteName, Accidental>> endTie;
};


class Note
{
public:
    Note();
    Note(NoteName name, Accidental accidental, NoteType type);
    Note(NoteName name, Accidental accidental, NoteType type, NoteAttributes attributes);

    void addChord(NoteName name, Accidental accidental);
    void addDot();
    void addDoubleDot();
    void addTiedNote(NoteType type);

    void setBeatUnits(int beatUnits) { this->beatUnits = beatUnits; }
    void setDynamic(Dynamic dynamic) { attributes.dynamic = dynamic; }
    void setDurationModifier(float percentage) { this->durationModifier = percentage; }
    void setIgnore(bool ignore) { this->ignore = ignore; }

    NoteName getNoteName() { return this->name; }
    Accidental getAccidental() { return this->accidental; }
    NoteType getNoteType() { return this->type; }
    float getDurationModifier() { return this->durationModifier; }

    std::vector<float>& getFreqs() { return this->freqs; }
    NoteAttributes& getAttributes() { return this->attributes; }
    std::vector<std::pair<NoteName, Accidental>>& getEndTieVect() { return this->attributes.endTie; }
    int getBeatUnits() { return this->beatUnits; }
    bool isRest() { return this->rest; }
    bool isPedalOn() { return this->pedalOn; }
    bool ignorePlayback() { return this->ignore; }

protected:
    NoteName name;
    Accidental accidental;
    NoteType type;
    std::vector<float> freqs;
    NoteAttributes attributes;
    int beatUnits;
    float durationModifier;
    bool rest;
    bool pedalOn;
    bool ignore;
};


class Rest : public Note
{
public:
    Rest() { rest = true; }
    Rest(NoteType type)
    {
        attributes = NoteAttributes();
        beatUnits = as_int(type);
        rest = true;
    }
    Rest(int beatUnits)
    {
        attributes = NoteAttributes();
        this->beatUnits = beatUnits;
        rest = true;
    }
};


#endif
