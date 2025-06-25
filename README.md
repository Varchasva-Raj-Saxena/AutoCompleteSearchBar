# Autocomplete Search Bar

---

## 🌟 Project Overview

**Autocomplete Search Bar: Hybrid DSA + Deep Learning System**
This project presents an intelligent and high-performance search bar that merges **Data Structures and Algorithms (DSA)** with **Deep Learning**. Designed to offer lightning-fast suggestions, it incorporates both:

* ✨ **A C++ Trie-based backend** for **prefix**, **infix**, and **suffix** autocomplete.
* 🤖 **A Python-based LSTM (RNN) model** for **next-word prediction** using deep learning.

Trained on an **Indian cricket dataset**, this hybrid system delivers fast suggestions while intelligently predicting what you're likely to type next, all in real time.

> ☑️ **Live Web Demo:** Try it now at [http://34.131.245.79:8000/](http://34.131.245.79:8000/)

<h3>🎥 Demo</h3>
<p>
  <img src="assets/demo.gif" width="500"/>
</p>

---

## ✍️ Table of Contents

1. [Project Overview](#-project-overview)
2. [Web Demo](#-web-demo)
3. [Installation and Setup](#-installation-and-setup)
4. [Usage](#-usage)
5. [Features](#-features)
6. [Architecture](#-architecture)
7. [Configuration](#-configuration)
8. [Authors](#-authors)
9. [FAQ](#-faq)

---

## 🌐 Web Demo

> **Deployed at:** [http://34.131.245.79:8000/](http://34.131.245.79:8000/)
> Accessible across devices. Best viewed on desktop for full interaction.

---

## ⚙️ Installation and Setup

### Step 1: Clone Repository

```bash
git clone https://github.com/Abhinashroy/AutoCompleteSearchBar
cd AutoCompleteSearchBar
```

### Step 2: Dependencies

* **C++** with `g++`
* **Python 3.x** with:

  ```bash
  pip install flask flask-cors torch nltk
  ```
* A static file server (e.g., Live Server in VS Code) to serve the frontend

### Step 3: Backend Setup

* **Compile the C++ Trie server**:

  ```bash
  g++ Final.cpp -o server -lws2_32
  ```
* **Run the Trie server (C++)**:

  ```bash
  ./server
  ```
* **Start the Python API (LSTM)**:

  ```bash
  python app.py
  ```
* **Serve `index.html`** using any static file server or directly access via browser

---

## 🔄 Usage

### Local Setup

1. Run both backends:

   * C++ Trie Server: `http://localhost:8080`
   * Python LSTM API: `http://localhost:5000`
2. Open the `index.html` in your browser.
3. Start typing and experience:

   * Autocomplete suggestions via C++
   * Inline next-word predictions via LSTM (press `Tab` to accept)

### Web Version

Visit: [http://34.131.245.79:8000/](http://34.131.245.79:8000/)

---

## 🌟 Features

* ✔ **DSA-Powered Autocomplete**

  * Instant suggestions using Trie structure
  * Supports prefix, infix, suffix matches
  * Tracks most frequently searched terms
* ✨ **Deep Learning Next-Word Prediction**

  * Trained LSTM model on Indian cricket data
  * Context-aware multi-word continuation
* 🔍 **Ghost Text & Smart Suggestions**

  * Tab-accept inline prediction (ghost text)
  * Fully keyboard-navigable (Tab, Enter, Arrows)
* 📁 **Lightweight and Modular Codebase**

  * C++ Trie backend
  * PyTorch LSTM model API
  * Vanilla JS + HTML/CSS frontend

---

## 📄 Architecture

```
┌────────────┐      POST /predict      ┌─────────────────────┐
|  Frontend  | ─────────────────────▶ |  Python LSTM Model  |
| (JS/HTML)  |                        |   (Flask @ :5000)    |
└────────────┘                        └─────────────────────┘
     │   ▲
     ▼   │
POST /     
┌────────────────────┐
|  C++ Trie Backend   |
| (Winsock @ :8080)   |
└────────────────────┘
```

> Predictions and completions are seamlessly blended to feel natural.

---

## ⚙️ Configuration

* Change backend URLs in `script.js` under `API_BASE`:

  ```js
  const API_BASE = "http://34.131.245.79";
  ```
* For local use: change to `http://localhost`

---

## 💼 Authors

* [Varchasva Raj Saxena](https://github.com/Varchasva-Raj-Saxena)
* [Anmol Yadav](https://github.com/AnmolYadav1301)
* [Abhinash Roy](https://github.com/Abhinashroy)
* [Deepanshu](#)

---

## 🤔 FAQ

**Q1. How do I change the server port?**
Update ports in `script.js` and rerun the C++/Python servers.

**Q2. Why don’t suggestions show?**
Check if both backend servers are running and accessible.

**Q3. Can I change the dataset?**
Yes. Replace `cricket.txt`, retrain the model, and regenerate `nextword_model.pt`.

**Q4. Does it work on mobile?**
Yes. Ensure you access the public IP (`http://34.131.245.79`) and not `localhost` on mobile.

**Q5. Can I host this permanently?**
Yes. This is deployable via Docker and VM on Google Cloud using Artifact Registry.

---

## 🚀 Powered By

* **C++ STL and Trie DSA** for blazing fast autocomplete
* **PyTorch LSTM** model trained on cricket text corpus
* **Flask** and **Winsock** for backend APIs
* **Vanilla JS** frontend with inline UX features

> ✨ Ready to experience intelligent typing? Visit [http://34.131.245.79:8000/](http://34.131.245.79:8000/) and try it live!
