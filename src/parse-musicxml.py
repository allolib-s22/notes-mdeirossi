import sys
import math
import xml.etree.ElementTree as ET


# Supports only one staff per instrument. For multiple staves, add another instrument
# Does not support pickup measures, use a full measure with rests instead

DEFAULT_ACCIDENTALS = {i: "natural" for i in ["A", "B", "C", "D", "E", "F", "G"]}
NOTES = [i + str(j) for j in range(9) for i in ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]]

NOTE_TYPES = [
    "1024th",
    "512th",
    "256th",
    "128th",
    "64th",
    "32nd",
    "16th",
    "eighth",
    "quarter",
    "half",
    "whole",
    "breve",
    "long",
    "maxima"
]

def parse_xml(argv):
    if len(argv) != 1:
        print('Usage: parse-musicxml.py <inputfile>')
        sys.exit()

    inputfile = argv[0]
    if len(inputfile) < 10:
        print('Input file name too short to be .musicxml')
        sys.exit()
    if inputfile[-9:] != '.musicxml':
        print('Input file must be .musicxml')
        sys.exit()

    print(f'Input file is {inputfile}')


    tree = ET.parse(inputfile)
    root = tree.getroot()
    id_table = {}
 
    result = """\
#ifndef PARSED_HPP
#define PARSED_HPP

void loadFromXML(Score& score) {

// ------------------------------
// Instrument List
// ------------------------------\n"""

    for part in root.find('part-list'):

        if part.tag == 'score-part':
            part_id = part.get('id')

            _instr = part.find('midi-instrument')
            if _instr != None:
                name = ""
                if _instr.find('midi-channel').text == "10":
                    name = get_percussion(int(_instr.find('midi-program').text))
                else:
                    name = get_instrument(int(_instr.find('midi-program').text))

                result += f"score.addStaff(Instrument::{name}, \"{name}-{part_id}\");\n"
                id_table[part_id] = name

    result += """\n\
// ------------------------------
// Parts
// ------------------------------\n"""

    for part in root.findall('part'):
        result += f"// {id_table[part.get('id')]}-{part.get('id')}\n"
        result += f"score.setStaff(\"{id_table[part.get('id')]}-{part.get('id')}\");\n"
        transpose = 0

        for measure in part.findall('measure'):
            result += f"// m{measure.get('number')}\n"

            _att = measure.find('attributes')

            # TIME SIGNATURE
            if _att != None and _att.find('time') != None:
                time_top = _att.find('time').find('beats').text
                time_bot = _att.find('time').find('beat-type').text
                result += f"score.addMeasure(TimeSignature({time_top}, {time_bot}));\n"
            else:
                result += f"score.addMeasure();\n"

            # TRANSPOSE
            if _att != None and _att.find('transpose') != None:
                if _att.find('transpose').find('octave-change') != None:
                    transpose = int(_att.find('transpose').find('octave-change').text) * 12
                transpose += int(_att.find('transpose').find('chromatic').text)
            
            chord_notes = 0
            accs = {}
            tied_notes = []
            dynamic = ""
            for t in measure:
                if t.tag == 'note':
                    if t.find('rest') != None:
                        if t.find('rest').get('measure') != "yes":
                            note_type = t.find('type').text
                    else:
                        note_type = t.find('type').text

                    # TUPLET VALUES
                    tuplet_num = 0
                    tuplet_value = ""
                    tuplet_dot = "false"
                    _mod = t.find('time-modification')
                    if _mod != None:
                        tuplet_num = int(_mod.find('actual-notes').text)
                        normal_num = int(_mod.find('normal-notes').text)
                        tuplet_type_index = 0
                        if _mod.find('normal-type') != None:
                            tuplet_type_index = NOTE_TYPES.index(_mod.find('normal-type').text)
                            if _mod.find('normal-dot') != None:
                                tuplet_dot = "true"
                        else:
                            tuplet_type_index = NOTE_TYPES.index(note_type)

                        tuplet_value = NOTE_TYPES[tuplet_type_index + int(math.log2(normal_num))]

                    # NOTATIONS
                    _notations = t.find('notations')
                    if _notations != None:

                        # TUPLETS
                        if _notations.find('tuplet') != None:
                            if _notations.find('tuplet').get('type') == "start":
                                result += f"score.addTuplet({tuplet_num}, NoteType::_{tuplet_value}, {tuplet_dot});\n"
                    
                    # REST OR PITCH
                    if t.find('rest') != None:
                        if t.find('rest').get('measure') == "yes":
                            result += "score.fillWithRests();\n"
                        else:
                            result += f"score.addRest(NoteType::_{t.find('type').text});\n"
                    else:
                        _pitch = t.find('pitch')
                        step = _pitch.find('step').text
                        accidental = "natural"
                        octave = _pitch.find('octave').text
                        tie_end = False

                        # ACCIDENTAL
                        if t.find('accidental') != None:
                            accs[step + octave] = t.find('accidental').text
                        accidental = accs.get(step + octave, "natural")

                        # TRANSPOSE
                        if transpose % 12 == 0:
                            octave = str(int(octave) + int(transpose / 12))
                        elif transpose != 0:
                            note_index = NOTES.index(step + octave)
                            if accidental == "sharp":
                                note_index += 1
                            elif accidental == "double-sharp":
                                note_index += 2
                            elif accidental == "flat":
                                note_index -= 1
                            elif accidental == "double-flat":
                                note_index -= 2

                            transposed_note = NOTES[note_index + transpose]
                            step = transposed_note[0]
                            octave = transposed_note[-1]
                            if len(transposed_note) > 2:
                                accidental = "sharp"
                            else:
                                accidental = "natural"

                        # TIES
                        ties = t.findall('tie')
                        if (len(ties) == 1 and ties[0].get('type') == "stop") or len(ties) == 2:
                            tie_end = True
                            for n in tied_notes:
                                if step == n["step"] and octave == n["octave"]:
                                    accidental = n["accidental"]
                        tied_notes = []

                        # NOTE AND CHORD
                        if t.find('chord') != None:
                            if tie_end:
                                result += f"score.addTiedChord(NoteName::{step}{octave}, Accidental::{accidental});\n"
                            else:
                                result += f"score.addChord(NoteName::{step}{octave}, Accidental::{accidental});\n"
                        else:
                            if tie_end:
                                result += f"score.addTiedNote(NoteName::{step}{octave}, Accidental::{accidental}, NoteType::_{note_type});\n"
                            else:
                                result += f"score.addNote(NoteName::{step}{octave}, Accidental::{accidental}, NoteType::_{note_type});\n"
                            
                            # DOTTED TIME VALUES    
                            dots = t.findall('dot')
                            if len(dots) == 2:
                                result += "score.addDoubleDot();\n"
                            elif len(dots) == 1:
                                result += "score.addDot();\n"

                        tied_notes.append({"step": step, "octave": octave, "accidental": accidental})

                    # ADD DYNAMIC AFTER NOTE
                    if dynamic != "":
                        result += f"score.setDynamic(Dynamic::{dynamic});\n"
                        dynamic = ""
                
                # TEXT DIRECTIONS
                elif t.tag == 'direction':
                    directions = t.findall('direction-type')
                    for direction in directions:

                        # TEMPO MARKING
                        if direction.find('metronome') != None:
                            beat_type = direction.find('metronome').find('beat-unit').text
                            has_dot = str(direction.find('metronome').find('beat-unit-dot') != None).lower()
                            bpm = direction.find('metronome').find('per-minute').text
                            result += f"score.setTempo(NoteType::_{beat_type}, {float(bpm)}f, {has_dot});\n"

                        # DYNAMICS
                        if direction.find('dynamics') != None:
                            dynamic = direction.find('dynamics')[0].tag

        result += "\n"
    result += "}\n\n#endif\n"

    file = open("parsed.hpp", "w")
    file.write(result)
    file.close()
    print("Output file is parsed.hpp")


def get_instrument(midi_number):
    switcher = {
        1: "AcousticGrandPiano",
        2: "BrightAcousticPiano",
        3: "ElectricGrandPiano",
        4: "HonkyTonkPiano",
        5: "ElectricPiano1",
        6: "ElectricPiano2",
        7: "Harpsichord",
        8: "Clavinet",
        9: "Celesta",
        10: "Glockenspiel",
        11: "MusicBox",
        12: "Vibraphone",
        13: "Marimba",
        14: "Xylophone",
        15: "TubularBells",
        16: "Dulcimer",
        17: "DrawbarOrgan",
        18: "PercussiveOrgan",
        19: "RockOrgan",
        20: "ChurchOrgan",
        21: "ReedOrgan",
        22: "Accordion",
        23: "Harmonica",
        24: "TangoAccordion",
        25: "AcousticGuitarNylon",
        26: "AcousticGuitarSteel",
        27: "ElectricGuitarJazz",
        28: "ElectricGuitarClean",
        29: "ElectricGuitarMuted",
        30: "ElectricGuitarOverdriven",
        31: "ElectricGuitarDistortion",
        32: "ElectricGuitarHarmonics",
        33: "AcousticBass",
        34: "ElectricBassFinger",
        35: "ElectricBassPicked",
        36: "FretlessBass",
        37: "SlapBass1",
        38: "SlapBass2",
        39: "SynthBass1",
        40: "SynthBass2",
        41: "Violin",
        42: "Viola",
        43: "Cello",
        44: "Contrabass",
        45: "TremoloStrings",
        46: "PizzicatoStrings",
        47: "Harp",
        48: "Timpani",
        49: "StringEnsemble1",
        50: "StringEnsemble2",
        51: "SynthStrings1",
        52: "SynthStrings2",
        53: "ChoirAahs",
        54: "VoiceOohs",
        55: "SynthVoice",
        56: "OrchestraHit",
        57: "Trumpet",
        58: "Trombone",
        59: "Tuba",
        60: "MutedTrumpet",
        61: "FrenchHorn",
        62: "BrassSection",
        63: "SynthBrass1",
        64: "SynthBrass2",
        65: "SopranoSax",
        66: "AltoSax",
        67: "TenorSax",
        68: "BaritoneSax",
        69: "Oboe",
        70: "EnglishHorn",
        71: "Bassoon",
        72: "Clarinet",
        73: "Piccolo",
        74: "Flute",
        75: "Recorder",
        76: "PanFlute",
        77: "BlownBottle",
        78: "Shakuhachi",
        79: "Whistle",
        80: "Ocarina"
    }
    return switcher.get(midi_number, "Invalid Midi Number")

def get_percussion(midi_number):
    switcher = {
        35: "AcousticBassDrum",
        36: "BassDrum1",
        37: "SideStick",
        38: "AcousticSnare",
        39: "HandClap",
        40: "ElectricSnare",
        41: "LowFloorTom",
        42: "ClosedHiHat",
        43: "HighFloorTom",
        44: "PedalHiHat",
        45: "LowTom",
        46: "OpenHiHat",
        47: "LowMidTom",
        48: "HighMidTom",
        49: "CrashCymbal1",
        50: "HighTom",
        51: "RideCymbal1",
        52: "ChineseCymbal",
        53: "RideBell",
        54: "Tambourine",
        55: "SplashCymbal",
        56: "Cowbell",
        57: "CrashCymbal2",
        58: "Vibraslap",
        59: "RideCymbal2",
        60: "HighBongo",
        61: "LowBongo",
        62: "MuteHighConga",
        63: "OpenHighConga",
        64: "LowConga",
        65: "HighTimbale",
        66: "LowTimbale",
        67: "HighAgogo",
        68: "LowAgogo",
        69: "Cabasa",
        70: "Maracas",
        71: "ShortWhistle",
        72: "LongWhistle",
        73: "ShortGuiro",
        74: "LongGuiro",
        75: "Claves",
        76: "HighWoodBlock",
        77: "LowWoodBlock",
        78: "MuteCuica",
        79: "OpenCuica",
        80: "MuteTriangle",
        81: "OpenTriangle"
    }
    return switcher.get(midi_number, "Invalid Midi Number")

if __name__ == "__main__":
	parse_xml(sys.argv[1:]);
