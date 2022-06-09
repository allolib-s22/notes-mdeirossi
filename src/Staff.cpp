#include "Staff.hpp"


Staff::Staff()
{
	this->instrument = Instrument::AcousticGrandPiano;
}

Staff::Staff(Instrument instrument)
{
	this->instrument = instrument;
}

void Staff::addMeasure(TimeSignature timeSignature)
{
	this->measures.push_back(Measure(timeSignature));
	std::cout << "--- Measure " << measures.size() << std::endl;
}

void Staff::addMeasure()
{
	this->measures.push_back(Measure(this->measures.back().getTimeSignature()));
	std::cout << "--- Measure " << measures.size() << std::endl;
}

void Staff::addNote(NoteName name, Accidental accidental, NoteType type)
{
	this->measures.back().addNote(name, accidental, type);
}

void Staff::addChord(NoteName name, Accidental accidental)
{
	this->measures.back().addChord(name, accidental);
}

void Staff::addRest(NoteType type)
{
	this->measures.back().addRest(type);
}

void Staff::addDot()
{
	this->measures.back().addDot();
}

void Staff::addDoubleDot()
{
	this->measures.back().addDoubleDot();
}

void Staff::fillWithRests()
{
	this->measures.back().fillWithRests();
}

void Staff::addTuplet(int numNotes, NoteType type, bool dotted)
{
	this->measures.back().addTuplet(numNotes, type, dotted);
}

void Staff::addTiedNote(NoteName name, Accidental accidental, NoteType type)
{
	for (int i = measures.size() - 1; i >= 0; --i) {
		Measure& m = measures[i];
		for (int j = m.getNotes().size() - 1; j >= 0; --j) {
			Note& n = m.getNotes()[j];
			if (n.isRest()) {
				std::cerr << "No note to tie from" << std::endl;
				throw std::logic_error("No note to tie from");
			}

			auto tie = n.getEndTieVect();
			auto isTieEnd = std::find(tie.begin(), tie.end(), std::make_pair(name, accidental));
			if (isTieEnd == tie.end()) {
				measures.back().addTiedNote(n.getNoteName(), n.getAccidental(), type);
				int timeToAdd = as_int(type);
				Note& n = m.getNotes()[j];
				if (measures.back().getTupletBeatsRemaining() > 0) {
					timeToAdd *= measures.back().getTupletDurationModifier();
				}

				n.setBeatUnits(n.getBeatUnits() + timeToAdd);
				std::cout << n.getBeatUnits() << std::endl;

				return;
			}
		}
	}
	std::cerr << "No note to tie from" << std::endl;
	throw std::logic_error("No note to tie from");
}

void Staff::addTiedChord(NoteName name, Accidental accidental)
{

}

void Staff::setDynamic(Dynamic dynamic)
{
	this->measures.back().setDynamic(dynamic);
}

void Staff::setBeatUnitsPerSecond(float bps)
{
	this->measures.back().setBeatUnitsPerSecond(bps);
}

bool Staff::isEmpty()
{
	return (this->measures.empty() || this->measures.back().isEmpty());
}