"""Python interface to the existing C rules and alpha-beta opponent."""
import ctypes as ct
from functools import lru_cache
import hashlib
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent
Board = (ct.c_char * 26) * 26
Moves = (ct.c_char * 2) * 64


class Result(ct.Structure):
    _fields_ = [('row', ct.c_int), ('col', ct.c_int), ('score', ct.c_double),
                ('nodes', ct.c_uint64), ('cutoffs', ct.c_uint64)]


@lru_cache(maxsize=1)
def library():
    compiler = shutil.which('gcc') or shutil.which('cc')
    if not compiler:
        raise RuntimeError('A C compiler is required. Install GCC locally; Community Cloud uses packages.txt.')
    sources = [ROOT / name for name in ('reversi.c', 'search.c', 'reversi.h')]
    digest = hashlib.sha256(b''.join(p.read_bytes() for p in sources)).hexdigest()[:16]
    directory = Path(tempfile.mkdtemp(prefix=f'reversi-{digest}-'))
    windows = platform.system() == 'Windows'
    target = directory / ('engine.dll' if windows else 'engine.so')
    command = [compiler, '-std=c11', '-O2', '-shared']
    command += ['-static-libgcc'] if windows else ['-fPIC']
    command += [str(sources[0]), str(sources[1]), '-o', str(target)]
    completed = subprocess.run(command, capture_output=True, text=True, timeout=60)
    if completed.returncode:
        raise RuntimeError('C engine compilation failed: ' + completed.stderr)
    lib = ct.CDLL(str(target))
    lib.initialiseBoard.argtypes = [Board, ct.c_int]
    lib.initialiseBoard.restype = None
    lib.getValidMoves.argtypes = [Board, ct.c_int, ct.c_char, Moves]
    lib.getValidMoves.restype = ct.c_int
    lib.playMove.argtypes = [Board, ct.c_int, ct.c_int, ct.c_int, ct.c_char]
    lib.playMove.restype = None
    lib.searchBest.argtypes = [Board, ct.c_char, ct.c_int, ct.c_bool]
    lib.searchBest.restype = Result
    return lib


def native(cells):
    if len(cells) != 64 or any(v not in 'BWU' for v in cells):
        raise ValueError('Expected 64 B/W/U cells')
    board = Board()
    for i, value in enumerate(cells):
        board[i // 8][i % 8] = value.encode()
    return board


def flatten(board):
    return ''.join(board[r][c].decode() for r in range(8) for c in range(8))


def initial():
    board = Board()
    library().initialiseBoard(board, 8)
    return flatten(board)


def legal(cells, turn):
    if turn not in ('B', 'W'):
        raise ValueError('Invalid colour')
    moves = Moves()
    count = library().getValidMoves(native(cells), 8, turn.encode(), moves)
    return [(moves[i][0][0], moves[i][1][0]) for i in range(count)]


def play(cells, turn, row, col):
    if (row, col) not in legal(cells, turn):
        raise ValueError('Illegal move')
    board = native(cells)
    library().playMove(board, 8, row, col, turn.encode())
    return flatten(board)


def best(cells, turn, depth=4):
    if turn not in ('B', 'W') or depth not in (2, 3, 4):
        raise ValueError('Invalid search settings')
    return library().searchBest(native(cells), turn.encode(), depth, True)


def other(turn):
    return 'W' if turn == 'B' else 'B'
