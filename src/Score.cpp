#include "Score.hpp"


Score::Score()
{
    this->amplitude = 0.5f;
    this->beatUnitsPerSecond = 256.0f;
    this->maxNoteSeparation = 0.5f;
    this->currentStaff = nullptr;
}

void Score::addStaff(Instrument instrument, std::string name)
{
    if (std::find(staffNames.begin(), staffNames.end(), name) != staffNames.end()) {
        std::cerr << "Staff with name " << name << " already exists" << std::endl;
        throw std::invalid_argument("Staff with name " + name + " already exists");
    }
    staffNames.push_back(name);
    staves.push_back(Staff(instrument));
    currentStaff = &staves.back();
}

void Score::setStaff(std::string name)
{
    std::vector<std::string>::iterator it = std::find(staffNames.begin(), staffNames.end(), name);
    if (it == staffNames.end()) {
        std::cerr << "No staff with name " << name << std::endl;
        throw std::invalid_argument("No staff with name " + name);
    }
    currentStaff = &staves.at(it - staffNames.begin());
}

void Score::addMeasure(TimeSignature timeSignature)
{
    checkStaffExists();
    if (currentStaff->getMeasures().size() > 0) {
        if (currentStaff->getMeasures().back().getBeatUnitsRemaining() > 0) {
            std::cerr << "Current measure is not filled" << std::endl;
            throw std::logic_error("Current measure is not filled");
        }
    }
    currentStaff->addMeasure(timeSignature);
}

void Score::addMeasure()
{
    checkStaffExists();
    if (currentStaff->getMeasures().size() < 1) {
        std::cerr << "Time signature required in first measure" << std::endl;
        throw std::invalid_argument("Time signature required in first measure");
    }
    if (currentStaff->getMeasures().back().getBeatUnitsRemaining() > 0) {
        std::cerr << "Current measure is not filled" << std::endl;
        throw std::logic_error("Current measure is not filled");
    }
    currentStaff->addMeasure();
}

void Score::addNote(NoteName name, Accidental accidental, NoteType type)
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addNote(name, accidental, type);
}

void Score::addChord(NoteName name, Accidental accidental)
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addChord(name, accidental);
}

void Score::addRest(NoteType type)
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addRest(type);
}

void Score::addDot()
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addDot();
}

void Score::addDoubleDot()
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addDoubleDot();
}

void Score::fillWithRests()
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->fillWithRests();
}

void Score::addTuplet(int numNotes, NoteType type, bool dotted)
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addTuplet(numNotes, type, dotted);
}

void Score::addTiedNote(NoteName name, Accidental accidental, NoteType type)
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addTiedNote(name, accidental, type);
}

void Score::addTiedChord(NoteName name, Accidental accidental)
{
    checkStaffExists();
    checkMeasureExists();
    currentStaff->addTiedChord(name, accidental);
}

void Score::setDynamic(Dynamic dynamic)
{
    if (staffNames.size() < 1 || currentStaff->getMeasures().size() < 1) {
        this->amplitude = as_int(dynamic) / 100.0f;
    }
    else {
        currentStaff->setDynamic(dynamic);
    }
}

void Score::setTempo(NoteType type, float BPM, bool dotted = false)
{
    float typeValue = (dotted) ? (as_int(type) / 2.0f + as_int(type)) : as_int(type);
    float bps = (BPM / 60.0f) * (typeValue / 256.0f) * 256.0f;
    if (currentStaff->getMeasures().size() < 1) {
        this->beatUnitsPerSecond = bps;
    }
    else {
        currentStaff->setBeatUnitsPerSecond(bps);
    }
}

void Score::setMaxNoteSeparation(float maxNoteSeparation)
{
    this->maxNoteSeparation = maxNoteSeparation;
}

void Score::loadScore()
{
    SynthVoice* voice;
    Staff staff;
    Measure measure;
    Note note;
    float duration;
    float currentTime;

    for (size_t s = 0; s < staves.size(); ++s) {
        currentTime = 0.0f;
        staff = staves.at(s);

        for (size_t i = 0; i < staff.getMeasures().size(); ++i) {
            measure = staff.getMeasures().at(i);
            if (measure.getNewTempo() != 0) {
                beatUnitsPerSecond = measure.getNewTempo();
            }

            for (size_t j = 0; j < measure.getNotes().size(); ++j) {
                note = measure.getNotes().at(j);
                currentTime += loadNote(note, staff.getInstrument(), currentTime);
            }

        }
    }
}

float Score::loadNote(Note& note, Instrument instrument, float currentTime)
{
    // Update attributes
    if (as_int(note.getAttributes().dynamic) > 0) {
        amplitude = as_int(note.getAttributes().dynamic) / 100.0f;
    }
    if (note.getAttributes().beatUnitsPerSecond > 0) {
        beatUnitsPerSecond = note.getAttributes().beatUnitsPerSecond;
    }

    if (!note.isRest()) {
        SynthVoice* voice;
        float duration;

        for (size_t k = 0; k < note.getFreqs().size(); ++k) {
            if (note.ignorePlayback()) { return 0; }
            voice = getVoice(instrument);
            
            voice->setInternalParameterValue("freq", note.getFreqs().at(k));
            voice->setInternalParameterValue("amp", amplitude / 20.0f);

            if (note.isPedalOn()) {
                duration = voice->getInternalParameterValue("pedalDuration");
            }
            else {
                duration = (note.getBeatUnits() / beatUnitsPerSecond);
                duration = fmax(duration * 0.9f, duration - maxNoteSeparation);
            }

            sequencer->addVoice(
                voice,
                currentTime,
                duration
            );
        }
    }

    return note.getBeatUnits() / beatUnitsPerSecond;
}

void Score::playScore()
{
    loadScore();
    sequencer->playSequence();
}

void Score::checkStaffExists()
{
    if (staffNames.size() < 1) {
        std::cerr << "No staves exist" << std::endl;
        throw std::invalid_argument("No staves exist");
    }
}

void Score::checkMeasureExists()
{
    if (currentStaff->getMeasures().size() < 1) {
        std::cerr << "No measure in staff" << std::endl;
        throw std::invalid_argument("No measure in staff");
    }
}

SynthVoice* Score::getVoice(Instrument instrument)
{
    switch (instrument) {
    case Instrument::Celesta:
        return sequencer->synth().getVoice<Celesta>();
    case Instrument::Trumpet:
        return sequencer->synth().getVoice<Trumpet>();
    case Instrument::MutedTrumpet:
        return sequencer->synth().getVoice<MutedTrumpet>();
    case Instrument::FrenchHorn:
        return sequencer->synth().getVoice<FrenchHorn>();
    case Instrument::Trombone:
        return sequencer->synth().getVoice<Trombone>();
    case Instrument::Tuba:
        return sequencer->synth().getVoice<Tuba>();
    default:
        std::cerr << "Could not find voice for instrument" << std::endl;
        throw std::logic_error("Could not find voice for instrument");
    }
}
