"""Browser game. Each visitor's board lives only in their session state."""
import streamlit as st
import engine

st.set_page_config(page_title='Reversi | Play', page_icon='⚫', layout='centered')
st.markdown('''<style>
.block-container {max-width:760px;padding-top:2rem;}
div[data-testid="stHorizontalBlock"] {gap:0.35rem;}
div[data-testid="stHorizontalBlock"] button {min-height:2.7rem;padding:0;border-radius:8px;}
div[data-testid="stHorizontalBlock"] button p {font-size:1.15rem;}
</style>''', unsafe_allow_html=True)
st.title('Reversi')
st.caption('A classic strategy game. Your next move can change the board.')


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
        st.write('Click a highlighted square to place a disc. Trap opposing discs between yours to flip them. '
                 'Black moves first. If you cannot move, your turn passes automatically. '
                 'When neither side can move, the player with the most discs wins.')
        st.caption('Coordinates use row then column: cd means row c, column d.')

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
    left, right = st.columns(2)
    left.metric('⚫ Black' + (' · You' if s.human == 'B' else ' · Computer'), black)
    right.metric('⚪ White' + (' · You' if s.human == 'W' else ' · Computer'), white)
    if over:
        mine = black if s.human == 'B' else white
        theirs = white if s.human == 'B' else black
        st.success('Draw!' if mine == theirs else 'You win!' if mine > theirs else 'Computer wins. Try another game!')
    else:
        st.write(f'**Your turn** · {len(legal)} legal moves · {"Black" if s.human == "B" else "White"}')
    if s.notice:
        st.info(s.notice)
        s.notice = ''
    for row in range(8):
        columns = st.columns(8)
        for col, column in enumerate(columns):
            cell = s.cells[row*8+col]
            label = '⚫' if cell == 'B' else '⚪' if cell == 'W' else f'{chr(97+row)}{chr(97+col)}'
            enabled = (row, col) in legal and not over
            column.button(label, key=f'square_{row}_{col}', disabled=not enabled,
                          type='primary' if enabled else 'secondary', use_container_width=True,
                          on_click=human_move, args=(row, col),
                          help=f'Row {chr(97+row)}, column {chr(97+col)}')
    st.caption('Highlighted coordinates are your legal moves. Start a new game from the sidebar.')
    with st.expander('Move history'):
        st.write(' · '.join(s.history) if s.history else 'Make the first move.')
except (RuntimeError, OSError) as error:
    st.error('The game engine could not start. Please contact the app owner.')
    with st.expander('Setup details'):
        st.code(str(error))
    st.stop()
