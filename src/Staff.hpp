#ifndef STAFF_HPP
#define STAFF_HPP

#include "Measure.hpp"


class Staff
{
public:
	Staff();
	Staff(Instrument instrument);

	void addMeasure(TimeSignature timeSignature);
	void addMeasure();
	void addNote(NoteName name, Accidental accidental, NoteType type);
	void addChord(NoteName name, Accidental accidental);
	void addRest(NoteType type);
	void addDot();
	void addDoubleDot();
	void fillWithRests();
	void addTuplet(int numNotes, NoteType type, bool dotted);
	void addTiedNote(NoteName name, Accidental accidental, NoteType type);
	void addTiedChord(NoteName name, Accidental accidental);

	void setDynamic(Dynamic dynamic);
	void setBeatUnitsPerSecond(float bps);

	Instrument getInstrument() { return instrument; }
	std::vector<Measure>& getMeasures() { return measures; }
	bool isEmpty();

protected:
	Instrument instrument;
	std::vector<Measure> measures;
};


#endif
