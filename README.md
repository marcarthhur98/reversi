# Reversi in C

## Browser version

Play through a Streamlit interface backed by the same C engine. Run
`python -m pip install -r requirements.txt`, then
`python -m streamlit run streamlit_app.py`. GCC must be installed on the server
or local machine running Streamlit; visitors only need a browser.

See [DEPLOY.md](DEPLOY.md) for deploying to Streamlit Community Cloud and adding
the live app link to GitHub's About section. If a live game is linked there,
open that link to play without installing anything.

A playable 8x8 terminal game with a four-ply minimax opponent, alpha-beta
pruning, and stage-dependent positional evaluation. Developed from Marc Arthur
Kentsa's APS105 Reversi lab implementation; this standalone version was refactored
and extended with AI assistance. The original course interface/starter headers
were attributed to the APS105H1 Teaching Team. This package uses its own header
and does not require the course's opponent library.

## Difficulty modes

All three modes are available in the **same Streamlit app**:

| Mode | Strategy |
|---|---|
| **Easy** | Picks a random legal move, with no lookahead. |
| **Medium** | Uses the original greedy strategy from Lab 8 Part 1: choose the move that flips the most discs immediately. Ties are resolved in row-major order. |
| **Hard** | Uses four-ply minimax with alpha-beta pruning and stage-dependent board evaluation. |

Open the sidebar using the arrow at the upper left, choose **Difficulty**, then
press **New game**. The selected mode applies to the new game. You can also
choose to play Black (first) or White (second).

The board has no coordinate labels: click a highlighted dot to place your disc.
Your score and the computer's score appear above the board; the robot icon marks
the computer. Forced passes happen automatically, and the game ends when neither
player can move. The player with the most discs wins.

The terminal version continues to use the Hard opponent.

## Play on Windows

Requires GCC on PATH (tested with the installed MSYS2 UCRT64 GCC).
From this folder in PowerShell:

```powershell
./build.bat play
```

Or compile directly:

```powershell
gcc -std=c11 -O2 -Wall -Wextra -Werror reversi.c search.c main.c -o reversi.exe
./reversi.exe
```

Choose B to move first or W to let the computer start. Enter row then column
using letters a-h; for example `cd` is a legal initial move for Black. Enter `q`
to quit. Invalid input is rejected without forfeiting the game. A player without
a legal move passes automatically. The game ends when neither player can move,
even if empty squares remain. EOF exits cleanly.

## Build on Linux/macOS

With GCC (or a C11 compiler) and Make installed:

```sh
make
./reversi
make test
make benchmark
```

## How the Hard opponent works

- Minimax evaluates every leaf from the computer's perspective. The opponent
  minimizes that same score, rather than subtracting scores from different boards.
- Alpha-beta pruning skips branches that cannot improve the decision. Corners
  are searched first, not automatically selected. Equal scores are resolved
  deterministically using stable move ordering.
- The four-ply depth counts placed discs; a forced pass does not consume a ply.
- Terminal wins and losses dominate positional scores. Disc margin breaks ties
  among wins or among losses; a tied game scores zero.
- Evaluation preserves the original mobility, corner control, danger-square,
  edge occupancy and stage-specific piece-count features. Stable edge chains
  connected to owned corners are counted once per square. This is a conservative
  edge feature, not a complete test of all stable discs.
- Repeated piece counts are cached within each evaluation. The existing 26-column
  board stride is retained to keep the original rule code recognizable, but only
  8x8 games are supported. Move buffers hold 64 entries.

Search depth can be set through `searchBest` (1-6). Hard mode and the terminal game use depth 4.
The standalone `makeMove` adapter returns 1 when a move exists and 0 for a
pass/invalid arguments, with row/column set to -1. It is not a drop-in submission
for the original course grader.

## Validation

```powershell
./build.bat test
./build.bat benchmark
python -m unittest test_streamlit.py
```

Tests cover initial legal moves, invalid coordinates/colours, eight-direction
flipping, stable-edge deduplication, forced passes, terminal wins/losses/draws,
and board immutability during search. Pruned search is compared with exhaustive
minimax on varied reachable positions: scores and selected moves must match,
and pruning must not expand more nodes.

All three executables compiled with `-Wall -Wextra -Werror`. CLI smoke checks
also exercised EOF, invalid colour/move input, overlong input, and the computer
moving first. These are regression checks, not an exhaustive proof of correctness.

The five Python/Streamlit tests additionally cover browser moves and computer
replies, resets, session isolation, colour and difficulty selection, forced
passes, game end, and the three strategies. Medium is checked against actual
disc gains across varied positions; Hard is checked against the four-ply search.

## Benchmark results

The included `BENCHMARK.txt` records 20 complete games: 10 seeded six-ply random
openings, each replayed with the AI as Black and White. The baseline reproduces
Lab 8 Part 1's strategy: maximize immediate flips, breaking ties in row-major order.
These results compare **Hard against Medium**; Easy was not part of this benchmark.

| Metric | Recorded result |
|---|---:|
| Wins / losses / draws | 20 / 0 / 0 |
| Search depth | 4 plies |
| AI decisions | 580 |
| Average move time | 2.934 ms |
| Maximum move time | 43 ms |
| Positions searched | 310,867 |
| Average positions per move | 536.0 |
| Alpha-beta cutoffs | 84,177 |

Measured locally on Windows with GCC `-O2` using C `clock()`. Timer semantics
and resolution vary by platform; timings are illustrative and not portable speed
guarantees. This small, reproducible benchmark measures performance against a
simple greedy baseline, not tournament strength or improvement over the original
Part 2 opponent. Some games end with empty squares because both players must pass.

## Files

- `streamlit_app.py`: browser board, scores, settings, and per-session games.
- `engine.py`: Python bindings to the C engine and difficulty selection.
- `test_streamlit.py`: browser interaction and difficulty regression tests.
- `requirements.txt` / `packages.txt`: Python and cloud compiler dependencies.
- `DEPLOY.md`: local Streamlit setup and cloud deployment instructions.
- `reversi.c`: board rules and evaluation adapted from the original lab.
- `reversi.h`: consistent public declarations and search result type.
- `search.c`: minimax, pruning, ordering, and greedy baseline.
- `main.c`: validated terminal interface.
- `tests.c`: rules and search regression checks.
- `benchmark.c`: paired full-game benchmark with deterministic opening seeds.
- `build.bat` / `Makefile`: reproducible builds.

Original lab files were left unchanged. Generated binaries and objects are
excluded by `.gitignore`.

## Local launch limitation

Windows Application Control blocked the final launch of build/reversi.exe in
this environment. Earlier CLI smoke checks succeeded from the scratch build;
the final test and benchmark executables also ran successfully. No security
policy was changed. The packaged game executable launch remains unverified.
