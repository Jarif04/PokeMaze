POKE-MAZE ADVENTURE
====================

WHAT CHANGED
------------
1. UpdateGame() was split into 4 focused functions, called in sequence from
   UpdateGame():
     - UpdateMenuScreens()            -> START and HIGHSCORES screens
     - UpdateGameEndStates()          -> LEVELUP / GAMEOVER / VICTORY states
     - UpdatePlayerMovementAndBombs() -> player input/movement + bomb spawn/tick/explode
     - UpdateBallsGhostsCollisions()  -> ball pickup, exit unlock, ghost AI + collisions

2. All runtime audio synthesis (genSound, genGhostSound, genGameOverSound,
   genVictorySound, genBGM, genBombSound) was removed from the code. Each of
   those generators was "baked" into an actual .wav file using the exact same
   waveform math, so the audio sounds identical to before. The game now just
   calls LoadSound("audio/xxx.wav") for each sound at startup.

FOLDER CONTENTS
---------------
main.cpp      - updated source code
PokeMaze.exe  - compiled Windows executable (statically linked, no DLLs needed)
PokeMaze      - compiled Linux executable
audio/        - the 6 generated soundtrack/SFX files:
                collect.wav, ghost.wav, gameover.wav, victory.wav, bgm.wav, bomb.wav

IMPORTANT: keep the audio/ folder in the same directory as PokeMaze.exe (or
PokeMaze) — the code loads sounds via the relative path "audio/....wav".

ABOUT THE EXECUTABLES
----------------------
PokeMaze.exe was cross-compiled for 64-bit Windows using mingw-w64 and raylib
4.5.0, statically linked (libgcc/libstdc++ and raylib itself are baked in),
so it should run on a Windows machine without installing anything extra —
just double-click it with the audio/ folder sitting next to it.

PokeMaze (no extension) is the Linux build from the same source, tested and
confirmed working in this sandbox (renders, loads all 6 audio files).

If you ever need to rebuild from source yourself:
  Windows (mingw-w64): x86_64-w64-mingw32-g++ -std=c++17 -O2 main.cpp -o PokeMaze.exe -lraylib -lopengl32 -lgdi32 -lwinmm
  Linux:                g++ -std=c++17 -O2 main.cpp -o PokeMaze -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

