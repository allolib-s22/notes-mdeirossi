#ifndef MEASURE_HPP
#define MEASURE_HPP

#include "TimeSignature.hpp"
#include "Note.hpp"


class Measure
{
public:
    Measure();
    Measure(TimeSignature timeSignature);
    void addNote(NoteName name, Accidental accidental, NoteType type);
    void addRest(NoteType type);
    void addChord(NoteName name, Accidental accidental);
    void addDot();
    void addDoubleDot();
    void fillWithRests();
    void addTuplet(int numNotes, NoteType type, bool dotted);
    void addTiedNote(NoteName name, Accidental accidental, NoteType type);
    void addTiedChord(NoteName name, Accidental accidental);

    void setDynamic(Dynamic dynamic);
    void setBeatUnitsPerSecond(float beatUnitsPerSecond);
    
    TimeSignature& getTimeSignature() { return this->timeSignature; }
    std::vector<Note>& getNotes() { return this->notes; }
    int getBeatUnitsRemaining() { return this->beatUnitsRemaining; }
    int getTupletBeatsRemaining() { return this->tupletBeatsRemaining; }
    float getTupletDurationModifier() { return this->tupletDurationModifier; }
    float getNewTempo() { return this->newTempo; }
    bool isEmpty() { return this->notes.empty(); }

protected:
    float newTempo;
    TimeSignature timeSignature;
    int beatUnitsRemaining;
    int tupletBeatsRemaining;
    float tupletDurationModifier;
    std::vector<Note> notes;
};

#endif
