#include "Note.hpp"


Note::Note()
{
    this->attributes = NoteAttributes();
    this->beatUnits = 0;
    this->durationModifier = 1;
    this->rest = false;
    this->pedalOn = false;
    this->ignore = false;
}

Note::Note(NoteName name, Accidental accidental, NoteType type)
{
    this->name = name;
    this->accidental = accidental;
    this->type = type;
    this->freqs.push_back((A4_FREQ * pow(2.0f, (as_int(name) - 57.0f + as_int(accidental) - 2.0f) / 12.0f)));
    this->attributes = NoteAttributes();
    this->beatUnits = as_int(type);
    this->durationModifier = 1;
    this->rest = false;
    this->pedalOn = false;
    this->ignore = false;
}

Note::Note(NoteName name, Accidental accidental, NoteType type, NoteAttributes attributes)
{
    this->name = name;
    this->accidental = accidental;
    this->type = type;
    this->freqs.push_back((A4_FREQ * pow(2.0f, (as_int(name) - 57.0f + as_int(accidental) - 2.0f) / 12.0f)));
    this->attributes = attributes;
    this->beatUnits = as_int(type);
    this->durationModifier = 1;
    this->rest = false;
    this->pedalOn = false;
    this->ignore = false;
}

void Note::addChord(NoteName name, Accidental accidental)
{
    freqs.push_back((A4_FREQ * pow(2.0f, (as_int(name) - 57.0f + as_int(accidental) - 2.0f) / 12.0f)));
}

void Note::addDot()
{
    if ((beatUnits & (beatUnits - 1)) != 0) {
        std::cerr << "Cannot add dot to dotted note. Try addDoubleDot()" << std::endl;
        throw std::logic_error("Cannot add dot to dotted note. Try addDoubleDot()");
    }
    if (beatUnits < 2) {
        std::cerr << "Note duration too small" << std::endl;
        throw std::logic_error("Note duration too small");
    }
    beatUnits += (beatUnits / 2);
}

void Note::addDoubleDot()
{
    if ((beatUnits & (beatUnits - 1)) != 0) {
        std::cerr << "Cannot add dot to dotted note" << std::endl;
        throw std::logic_error("Cannot add dot to dotted note");
    }
    if (beatUnits < 4) {
        std::cerr << "Note duration too small" << std::endl;
        throw std::logic_error("Note duration too small");
    }
    beatUnits += (beatUnits / 2 + beatUnits / 4);
}

void Note::addTiedNote(NoteType type)
{

}
