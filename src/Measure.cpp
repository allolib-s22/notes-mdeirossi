#include "Measure.hpp"


Measure::Measure()
{
    this->timeSignature = TimeSignature();
    this->beatUnitsRemaining = 1024;
    this->tupletBeatsRemaining = 0;
    this->tupletDurationModifier = 1;
    this->newTempo = 0;
}

Measure::Measure(TimeSignature timeSignature)
{
    this->timeSignature = timeSignature;
    this->beatUnitsRemaining = timeSignature.getBeatUnits();
    this->tupletBeatsRemaining = 0;
    this->tupletDurationModifier = 1;
    this->newTempo = 0;
}

void Measure::addNote(NoteName name, Accidental accidental, NoteType type)
{
    Note note = Note(name, accidental, type);
    if (tupletBeatsRemaining > 0) {
        if (note.getBeatUnits() > tupletBeatsRemaining) {
            std::cerr << "Note length exceeds tuplet" << std::endl;
            throw std::logic_error("Note length exceeds tuplet");
        }
        tupletBeatsRemaining -= note.getBeatUnits();
        note.setBeatUnits(note.getBeatUnits() * tupletDurationModifier);
    }
    else {
        if (note.getBeatUnits() > beatUnitsRemaining) {
            std::cerr << "Note length exceeds measure" << std::endl;
            throw std::logic_error("Note length exceeds measure");
        }
        beatUnitsRemaining -= note.getBeatUnits();
    }
    notes.push_back(note);
}

void Measure::addRest(NoteType type)
{
    Rest rest = Rest(type);
    if (tupletBeatsRemaining > 0) {
        if (rest.getBeatUnits() > tupletBeatsRemaining) {
            std::cerr << "Note length exceeds tuplet" << std::endl;
            throw std::logic_error("Note length exceeds tuplet");
        }
        tupletBeatsRemaining -= rest.getBeatUnits();
        rest.setBeatUnits(rest.getBeatUnits() * tupletDurationModifier);
    }
    else {
        if (rest.getBeatUnits() > beatUnitsRemaining) {
            std::cerr << "Note length exceeds measure" << std::endl;
            throw std::logic_error("Note length exceeds measure");
        }
        beatUnitsRemaining -= rest.getBeatUnits();
    }
    notes.push_back(rest);
}

void Measure::addChord(NoteName name, Accidental accidental)
{
    if (notes.size() < 1) {
        std::cerr << "No existing note to make a chord" << std::endl;
        throw std::logic_error("No existing note to make a chord");
    }
    notes.back().addChord(name, accidental);
}

void Measure::addDot()
{
    if ((notes.back().getBeatUnits() / 2) > beatUnitsRemaining) {
        std::cerr << "Note length exceeds measure" << std::endl;
        throw std::logic_error("Note length exceeds measure");
    }
    if (tupletBeatsRemaining > 0) {
        tupletBeatsRemaining -= (notes.back().getBeatUnits() / 2);
    }
    else {
        beatUnitsRemaining -= (notes.back().getBeatUnits() / 2);
    }
    
    // Decrement beatUnits before Note::addDot() modifies the note length
    notes.back().addDot();
}

void Measure::addDoubleDot()
{
    if ((notes.back().getBeatUnits() / 2 + notes.back().getBeatUnits() / 4) > beatUnitsRemaining) {
        std::cerr << "Note length exceeds measure" << std::endl;
        throw std::logic_error("Note length exceeds measure");
    }
    if (tupletBeatsRemaining > 0) {
        tupletBeatsRemaining -= (notes.back().getBeatUnits() / 2 + notes.back().getBeatUnits() / 4);
    }
    else {
        beatUnitsRemaining -= (notes.back().getBeatUnits() / 2 + notes.back().getBeatUnits() / 4);
    }
    // Decrement beatUnits before Note::addDoubleDot() modifies the note length
    notes.back().addDoubleDot();
}

void Measure::fillWithRests()
{
    if (tupletBeatsRemaining > 0) {
        std::cerr << "Cannot fill tuplet with rests automatically" << std::endl;
        throw std::logic_error("Cannot fill tuplet with rests automatically");
    }
    int n;
    while (beatUnitsRemaining > 0) {
        n = 1;
        while (n * 2 <= beatUnitsRemaining) {
            n *= 2;
        } 
        beatUnitsRemaining -= n;
        notes.push_back(Rest(n));
    }
}

void Measure::addTuplet(int numNotes, NoteType type, bool dotted)
{
    if (tupletBeatsRemaining > 0) {
        std::cerr << "Nested tuplets not supported" << std::endl;
        throw std::logic_error("Nested tuplets not supported");
    }
    if (as_int(type) > beatUnitsRemaining) {
        std::cerr << "Tuplet length exceeds measure" << std::endl;
        throw std::logic_error("Tuplet length exceeds measure");
    }
    tupletBeatsRemaining = numNotes * (as_int(type) / static_cast<int>(std::pow(2, std::ceil(numNotes / 4.0))));
    tupletDurationModifier = static_cast<float>(as_int(type)) / static_cast<float>(tupletBeatsRemaining);
    beatUnitsRemaining -= as_int(type);

}

void Measure::addTiedNote(NoteName name, Accidental accidental, NoteType type)
{
    addNote(name, accidental, type);
    notes.back().getEndTieVect().push_back(std::make_pair(name, accidental));
    notes.back().setIgnore(true);
}

void Measure::addTiedChord(NoteName name, Accidental accidental)
{
    addChord(name, accidental);
}

void Measure::setDynamic(Dynamic dynamic)
{
    if (notes.size() < 1) {
        std::cerr << "No existing note to attach dynamic to" << std::endl;
        throw std::logic_error("No existing note to attach dynamic to");
    }
    notes.back().getAttributes().dynamic = dynamic;
}

void Measure::setBeatUnitsPerSecond(float beatUnitsPerSecond)
{
    if (notes.size() < 1) {
        this->newTempo = beatUnitsPerSecond;
    }
    else {
        notes.back().getAttributes().beatUnitsPerSecond = beatUnitsPerSecond;
    }
}
