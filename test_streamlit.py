"""Run with: python -m unittest test_streamlit.py"""
import unittest
from pathlib import Path
from streamlit.testing.v1 import AppTest
import engine


class BrowserGameTests(unittest.TestCase):
    def app(self):
        app = AppTest.from_file(str(Path(__file__).with_name('streamlit_app.py'))).run(timeout=30)
        self.assertFalse(app.exception)
        self.assertFalse(app.error)
        return app

    def test_move_reply_reset_and_session_isolation(self):
        app = self.app()
        separate = self.app()
        self.assertEqual(len([b for b in app.button if (b.key or '').startswith('square_') and not b.disabled]), 4)
        app.button(key='square_2_3').click().run(timeout=30)
        self.assertFalse(app.exception)
        self.assertEqual(len(app.session_state['history']), 2)
        self.assertEqual(app.session_state['turn'], app.session_state['human'])
        self.assertEqual(separate.session_state['cells'], engine.initial())
        app.button(key='new_game').click().run(timeout=30)
        self.assertEqual(app.session_state['cells'], engine.initial())

    def test_white_and_difficulty(self):
        app = self.app()
        app.selectbox(key='colour').select('White · move second').run()
        app.selectbox(key='level').select('Easy').run()
        app.button(key='new_game').click().run(timeout=30)
        self.assertEqual(app.session_state['human'], 'W')
        self.assertEqual(app.session_state['depth'], 2)
        self.assertEqual(len(app.session_state['history']), 1)
        self.assertFalse(app.exception)

    def test_terminal_board_and_forced_pass(self):
        app = self.app()
        app.session_state['cells'] = 'U'+'W'+'B'*62
        app.session_state['human'] = 'W'
        app.session_state['turn'] = 'W'
        app.run(timeout=30)
        self.assertFalse(app.exception)
        self.assertIn('You: pass', app.session_state['history'])
        self.assertEqual(app.session_state['cells'], 'B'*64)
        self.assertTrue(app.success)
        self.assertTrue(all(b.disabled for b in app.button if (b.key or '').startswith('square_')))


if __name__ == '__main__':
    unittest.main()
