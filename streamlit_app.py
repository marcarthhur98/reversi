"""Browser game. Each visitor's board lives only in their session state."""
import streamlit as st
import engine

st.set_page_config(page_title='Reversi | Play', page_icon='⚫', layout='centered', initial_sidebar_state='collapsed')
st.markdown('''<style>
.block-container {max-width:580px;padding-top:3.5rem;}
.st-key-board {gap:0!important;border:8px solid #164b39;border-radius:14px;overflow:hidden;
 box-shadow:0 10px 26px #12392a22;background:#164b39;}
.st-key-board [data-testid="stVerticalBlock"] {gap:0!important;}
.st-key-board [data-testid="stHorizontalBlock"] {gap:0!important;flex-wrap:nowrap!important;}
.st-key-board [data-testid="stColumn"] {min-width:0!important;width:12.5%!important;
 flex:1 1 0!important;}
.st-key-board [data-testid="stElementContainer"], .st-key-board .stButton {margin:0!important;}
.st-key-board button {width:100%!important;aspect-ratio:1;height:auto!important;
 min-height:0!important;padding:0!important;border-radius:0!important;
 border:1px solid #ffffff20!important;background:#247451!important;
 position:relative;opacity:1!important;box-shadow:none!important;}
.st-key-board button p {font-size:0!important;line-height:0!important;}
.st-key-board button::after {content:"";position:absolute;left:14%;top:14%;width:72%;height:72%;
 border-radius:50%;pointer-events:none;}
.st-key-board button[kind="primary"]::after {width:23%;height:23%;left:38.5%;top:38.5%;
 background:#d5f3a4;box-shadow:0 0 0 5px #d5f3a415;}
.st-key-board button[kind="primary"]:hover {background:#338761!important;}
.st-key-board button[kind="primary"]:hover::after {transform:scale(1.18);}
.st-key-board button:focus-visible {outline:3px solid #f9d978!important;outline-offset:-4px;z-index:1;}
.players {display:grid;grid-template-columns:1fr 1fr;gap:12px;margin:4px 0 12px;}
.player {display:flex;align-items:center;gap:12px;background:white;border:1px solid #dbe5dd;
 border-radius:14px;padding:14px 18px;color:#183d2c;}
.player strong {font-size:1.05rem;display:block;}.player small {color:#607467;}
.player .score {font-size:2rem;font-weight:700;margin-left:auto;}
.avatar {width:38px;height:38px;flex-shrink:0;}.avatar svg {width:100%;height:100%;}
.disc-mini {display:inline-block;width:10px;height:10px;border-radius:50%;border:1px solid #abb8ad;margin-right:4px;}
.disc-mini.black {background:#20252b;}.disc-mini.white {background:#fafaf5;}
.turn-message {padding:10px 0 3px;font-size:1.12rem;font-weight:700;color:#194831;}
.turn-help {color:#566b5d;margin-bottom:12px;font-size:.95rem;}
@media(max-width:480px){.block-container {padding-left:1rem;padding-right:1rem;}
 .player {padding:10px;gap:7px;}.avatar {width:28px;height:28px;}.player .score {font-size:1.6rem;}
 .st-key-board {border-width:5px;}}
</style>''', unsafe_allow_html=True)
st.title('Reversi')

ROBOT = '''<svg viewBox="0 0 40 40" fill="none" aria-hidden="true">
<path d="M20 7V3" stroke="#247451" stroke-width="2.5"/><circle cx="20" cy="3" r="2" fill="#247451"/>
<rect x="5" y="9" width="30" height="25" rx="8" fill="#e1eee4" stroke="#247451" stroke-width="2"/>
<rect x="10" y="15" width="20" height="10" rx="4" fill="#247451"/>
<circle cx="15" cy="20" r="2" fill="white"/><circle cx="25" cy="20" r="2" fill="white"/>
<path d="M16 29h8M2 18v7M38 18v7" stroke="#247451" stroke-width="2.5" stroke-linecap="round"/></svg>'''
PERSON = '''<svg viewBox="0 0 40 40" fill="none" aria-hidden="true">
<circle cx="20" cy="20" r="19" fill="#e1eee4"/>
<circle cx="20" cy="14" r="6" fill="#247451"/>
<path d="M9 32c0-14 22-14 22 0" fill="#247451"/></svg>'''


def player_card(name, colour, score, icon):
    colour_name = 'Black' if colour == 'B' else 'White'
    return (f'<div class="player"><span class="avatar">{icon}</span><div><strong>{name}</strong>'
            f'<small><span class="disc-mini {colour_name.lower()}"></span>{colour_name}</small></div>'
            f'<span class="score">{score}</span></div>')


def reset():
    st.session_state.cells = engine.initial()
    st.session_state.turn = 'B'
    st.session_state.human = 'B' if st.session_state.colour == 'Black · move first' else 'W'
    st.session_state.depth = {'Easy': 2, 'Medium': 3, 'Hard': 4}[st.session_state.level]
    st.session_state.history = []
    st.session_state.notice = ''


def human_move(row, col):
    s = st.session_state
    if s.turn != s.human or (row, col) not in engine.legal(s.cells, s.turn):
        return
    s.cells = engine.play(s.cells, s.turn, row, col)
    s.history.append(f'You: {chr(97+row)}{chr(97+col)}')
    s.turn = engine.other(s.turn)


with st.sidebar:
    st.header('Your game')
    st.selectbox('Play as', ['Black · move first', 'White · move second'], key='colour')
    st.selectbox('Difficulty', ['Easy', 'Medium', 'Hard'], index=2, key='level')
    st.button('New game', key='new_game', on_click=reset, use_container_width=True)
    st.caption('Colour and difficulty changes apply when you start a new game.')
    with st.expander('How to play', expanded=True):
        st.write('Click a dot to place your disc. Trap opposing discs between yours to flip them. '
                 'Black moves first. If you cannot move, your turn passes automatically. '
                 'When neither side can move, the player with the most discs wins.')

try:
    if 'cells' not in st.session_state:
        reset()
    s = st.session_state
    # Settle passes and computer turns before accepting another human move.
    with st.spinner('Preparing your next turn…'):
        while True:
            moves = engine.legal(s.cells, s.turn)
            if not moves:
                if not engine.legal(s.cells, engine.other(s.turn)):
                    break
                who = 'You have' if s.turn == s.human else 'The computer has'
                s.notice = f'{who} no legal move. Turn passed.'
                s.history.append(('You' if s.turn == s.human else 'Computer') + ': pass')
                s.turn = engine.other(s.turn)
                continue
            if s.turn == s.human:
                break
            result = engine.best(s.cells, s.turn, s.depth)
            s.cells = engine.play(s.cells, s.turn, result.row, result.col)
            s.history.append(f'Computer: {chr(97+result.row)}{chr(97+result.col)}')
            s.turn = engine.other(s.turn)

    legal = set(engine.legal(s.cells, s.human))
    over = not legal and not engine.legal(s.cells, engine.other(s.human))
    black, white = s.cells.count('B'), s.cells.count('W')
    mine = black if s.human == 'B' else white
    theirs = white if s.human == 'B' else black
    st.markdown('<div class="players">' + player_card('You', s.human, mine, PERSON)
                + player_card('Computer', engine.other(s.human), theirs, ROBOT) + '</div>',
                unsafe_allow_html=True)
    if over:
        mine = black if s.human == 'B' else white
        theirs = white if s.human == 'B' else black
        st.success('Draw!' if mine == theirs else 'You win!' if mine > theirs else 'Computer wins. Try another game!')
    else:
        st.markdown('<div class="turn-message">Your turn · Click a dot to play</div>', unsafe_allow_html=True)
    if s.notice:
        st.info(s.notice)
        s.notice = ''
    piece_styles = []
    for index, cell in enumerate(s.cells):
        if cell != 'U':
            paint = ('radial-gradient(circle at 35% 28%,#50565c,#14191e 72%)' if cell == 'B'
                     else 'radial-gradient(circle at 35% 28%,#fff,#e4e6df 75%)')
            piece_styles.append(f'.st-key-square_{index//8}_{index%8} button::after '
                                f'{{background:{paint};box-shadow:0 3px 4px #082a3655;}}')
    st.markdown('<style>' + ''.join(piece_styles) + '</style>', unsafe_allow_html=True)
    with st.container(key='board'):
        for row in range(8):
            columns = st.columns(8)
            for col, column in enumerate(columns):
                cell = s.cells[row*8+col]
                enabled = (row, col) in legal and not over
                # Text remains available to screen readers, hidden only visually.
                description = 'Place your disc' if enabled else {'B':'Black disc','W':'White disc','U':'Empty square'}[cell]
                label = f'{description}, row {row+1}, column {col+1}'
                column.button(label, key=f'square_{row}_{col}', disabled=not enabled,
                              type='primary' if enabled else 'secondary', use_container_width=True,
                              on_click=human_move, args=(row, col))
    st.button('New game', key='restart_below_board', on_click=reset, use_container_width=True)
except (RuntimeError, OSError) as error:
    st.error('The game engine could not start. Please contact the app owner.')
    with st.expander('Setup details'):
        st.code(str(error))
    st.stop()
