MusiTuna is an appliaction written for the LaFortuna AVR 8-bit microcontroller.

It provides an interface and the functionality for playing short songs (currently up to 16 groups of 3 notes).


Frontend

There two mode: staves and buttons. Switching between modes is done by a long center button press.

Stave mode:
The current note is displayed in purple and its abc notation appears below.
Left/Right -> change the horizontal position on the staves. There is a small position indicator at the top of the screen. 
Up/Down -> change the pitch of the current note (there are 24 available pitches).
Center -> add the current note to the song, or erase the current group of notes
Wheel -> change the duration of the current group of notes.

Button mode:
RESET: erase everything on the staves
PLAY: play the current song
ADD/ERASE: switch between adding and erasing notes (indicator is green for add, red for erase)


Backend

Before showing the main screen, the application computes and stores in RAM the waveform of each of the 24 pitches in 
the form of a periodic wave of up to 250 bytes. When playing multiple notes, the waves are added up, and when playing 
long notes, the wave is repeated. The sound quality is low because the waves were made short enough to fit in RAM, 
a possible future fix is using flash or SD memory.
