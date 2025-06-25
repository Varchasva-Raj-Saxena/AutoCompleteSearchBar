# Autocomplete Search Bar
---
## Project Title and Description
**Autocomplete Search Bar**  
This project is an interactive search bar with autocomplete functionality. It dynamically provides search suggestions as the user types, making it ideal for applications that require fast and efficient text searches. Unique features include prefix, suffix, and infix search suggestions, with a priority for frequently searched terms. Additionally, we now **introduce next-word prediction using a deep learning LSTM model**, creating a powerful combination of a DSA-based trie in C++ and an RNN/LSTM model in Python.

## Table of Contents
1. [Project Title and Description](#project-title-and-description)
2. [Table of Contents](#table-of-contents)
3. [Installation and Setup](#installation-and-setup)
4. [Usage](#usage)
5. [Configuration](#configuration)
6. [Features](#features)
7. [Architecture or Code Structure](#architecture-or-code-structure)
8. [Authors](#authors)
9. [FAQ](#faq)

## Installation and Setup
1. **Clone the repository**:  
   ```bash
   git clone https://github.com/Abhinashroy/AutoCompleteSearchBar
   ```
2. **Dependencies**:
   - C++ compiler (e.g., g++).
   - Python 3.x with packages: `flask`, `torch`, `nltk`, `flask-cors`.
   - A simple static server to serve `index.html` (e.g., Live Server in VS Code).

3. **Setup Instructions**:
   - **Compile the C++ backend** by running:
     ```bash
     g++ Final.cpp -o server -lws2_32
     ```
   - **Start the C++ backend**:
     ```bash
     ./server
     ```
   - **Run the Python LSTM API** (`app.py`):
     ```bash
     python app.py
     ```
   - **Serve the frontend** (e.g., using Live Server extension) and open `index.html` in your browser.

## Usage
1. **Run the Autocomplete Feature**:
   - Ensure both `server` (C++) and `app.py` (Python) are running.
   - Open `index.html` in a web browser via your static server.
   - Click the search icon and start typing to see:
     - **Trie-based suggestions** (prefix, suffix, infix) from the C++ backend.
     - **Next-word predictions** (up to 5 words) from the Python LSTM model, shown inline and accepted with Tab.

2. **C++ Console Usage**:
   - `./server` runs on `http://localhost:8080` by default.
   - It listens for POST requests containing `{ "query": "<prefix>" }` and returns JSON suggestions.

3. **Python API Usage**:
   - `app.py` runs on `http://localhost:5000`.
   - It listens for POST requests containing `{ "text": "<current sentence>" }` and returns a JSON `{ "next_word": "<sentence + predicted words>" }`.

## Configuration
- **Backend URLs**: Modify the endpoints in `script.js` if ports or hosts change.
- **Environment Variables**: None required beyond ensuring Flask-CORS and Torch are installed.

## Features
- **Autocomplete Suggestions**: Fast prefix, suffix, and infix suggestions powered by a C++ trie.
- **Next-Word Prediction**: Deep learning–based LSTM model predicts up to 5 next words inline.
- **Hybrid Architecture**: Combines efficient DSA (trie) with powerful neural language modeling (RNN/LSTM).
- **Keyboard Navigation**: Arrow keys for dropdown, Tab to accept inline predictions, Enter to select.
- **Custom Dataset**: Currently trained on an Indian cricket dataset; replace `cricket.txt` and retrain to adapt.

## Architecture or Code Structure
- **Frontend** (`index.html`, `style.css`, `script.js`):
  - HTML for UI structure.
  - CSS for styling and layering (ghost text + dropdown).
  - JS for event handling, fetch calls to both backends, and DOM updates.

- **Backend 1: C++ Trie** (`Final.cpp`):
  - Implements prefix, infix, suffix search.
  - Tracks and ranks frequent searches.
  - Exposes an HTTP POST API on port 8080 via Winsock.

- **Backend 2: Python LSTM API** (`app.py`):
  - Loads pretrained PyTorch LSTM model (`nextword_model.pt`).
  - Exposes an HTTP POST API on port 5000 using Flask + Flask-CORS.
  - Tokenizes input, pads/truncates, predicts, and returns combined sentence.

## Authors
1. [Varchasva Raj Saxena](https://github.com/Varchasva-Raj-Saxena)  
2. [Anmol Yadav](https://github.com/AnmolYadav1301)  
3. [Abhinash Roy](https://github.com/Abhinashroy)  
4. [Deepanshu](#)

---
## FAQ
1. **How do I change the server port?**  
   Update the fetch URL in `script.js` and recompile or restart the corresponding backend.
2. **Why are suggestions not appearing?**  
   Ensure both C++ server (`localhost:8080`) and Python API (`localhost:5000`) are running and accessible.
3. **How can I change the word list?**  
   Update the file loaded in `Final.cpp` (e.g., `cricket_words.txt`), recompile, and restart.
4. **Can I train on a new dataset?**  
   Yes—replace `cricket.txt`, retrain the LSTM in your Jupyter notebook, save `nextword_model.pt`, and rerun `app.py`.
