# Publish the browser game

The Streamlit app is ready for deployment; a public deployment has not yet been
created. GitHub hosts the source code, and Streamlit Community Cloud runs it.
GitHub Pages cannot run this Python/C application.

## Run locally

With Python 3.12 and GCC installed, from this folder:

```powershell
python -m pip install -r requirements.txt
python -m streamlit run streamlit_app.py
```

The Python interface compiles `reversi.c` and `search.c` into a temporary shared
library on first use. It runs the existing C engine directly using ctypes; the
game logic is not duplicated in Python. The temporary library is shared by the
server process; board data is separate for each browser session. Restarting the
app or losing a session can reset a game. No accounts, API keys or database are needed.

## GitHub and Streamlit Community Cloud

1. Create a GitHub repository named `reversi` and upload the source files from
   this folder, including `.streamlit/config.toml`, `requirements.txt`, and
   `packages.txt`. Exclude `build/`, executables, archives, and `__pycache__/`.
2. In https://share.streamlit.io, sign in and connect your GitHub account.
3. Create an app, select the repository and its branch, and set the entrypoint
   to `streamlit_app.py`. Choose Python 3.12 in the advanced settings.
4. Deploy. `requirements.txt` installs Streamlit and `packages.txt` installs GCC
   on the Linux host. Choose an available app subdomain if desired.
5. Test the live app, copy its actual `https://...streamlit.app` URL, and add that
   URL to the repository's About > Website field. Add a Play link to README too.

Do not use localhost or an invented URL in the About field. The public link only
exists after deployment succeeds. Visitors can then click it and play without
installing Python or a C compiler.

Official deployment instructions:
https://docs.streamlit.io/deploy/streamlit-community-cloud/deploy-your-app/deploy

External dependencies:
https://docs.streamlit.io/deploy/streamlit-community-cloud/deploy-your-app/app-dependencies

## Browser regression checks

```powershell
python -m unittest test_streamlit.py
```

These use Streamlit AppTest to exercise legal board clicks, automatic computer
replies, resets, colour/difficulty changes, session separation, passes and game end.
