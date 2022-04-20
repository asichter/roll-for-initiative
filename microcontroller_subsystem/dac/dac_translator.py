#****************************************************************************************************
# File:     dac_translatory.py
# Author:   Alexandra E. Sichterman
# Project:  Roll For Initiative
# Description:
#   Inputs a txt file with the format "note_value note_length"
#     note_value:   str, in format "A4" <note name + octave>
#                   sharps are indicated like "AS4" <note name + S + octave>
#     note_length:  float, length of note on 4/4 quarter note separation
#                 Ex.
#                   4.0   = whole note
#                   2.0   = half note
#                   1.0   = quarter note
#                   0.5   = eighth note
#                   0.25  = sixteenth note
#   Outputs a txt file in format matching C struct:
#     struct Tone {
#         float freq;
#         float len;
#       } song[] = {
#           {note_freq, next psc},
#       };
#
#   The first value is the associated note value in hertz units. The second is the prescaler value 
#   that when coupled with an ARR of 6000-1 will set the timer to interrupt after the note duration 
#   has elapsed to load the next note.
#
#****************************************************************************************************


import sys
from notes import *

if __name__ == "__main__":
  tim_arr = 6000
  argv = sys.argv
  if len(argv) == 3:
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    with open(input_file) as song_file:
      lines = song_file.readlines()
      song_list = []
      for line in lines:
        song_list.append(tuple(line.split()))
    
    if song_list is not []:
      bpm = int(song_list.pop(0)[0])

      freq_psc = []
      for note, length in song_list:
        freq    = get_freq(note)
        hz      = bpm / (60 * float(length))
        psc     = int(48000000 / (hz * tim_arr)) - 1
        freq_psc.append((freq, psc))
      print(freq_psc)

    with open(output_file, "w") as f:
      f.write("SONG[] = {\n")
      if freq_psc is not []:
        for freq, psc in freq_psc:
          f.write(f"\t{{{freq}, {psc}}}\n")
      f.write("}\n")

    
  else:
    print("command format: python3 dac_translator.py input.txt output.txt\n")
    