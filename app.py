from flask import Flask, request, jsonify
from flask_cors import CORS
import torch
import torch.nn as nn
import nltk
from nltk.tokenize import TreebankWordTokenizer
import pickle
from model import LSTMmodel  # Ensure model.py has LSTMmodel defined

# Use Treebank tokenizer (no punkt dependency)
tokenizer = TreebankWordTokenizer()

# --------------------------
# Load Vocabulary
# --------------------------
with open("vocab.pkl", "rb") as f:
    vocab = pickle.load(f)

# --------------------------
# Utilities
# --------------------------
def text_indices(tokens, vocab):
    return [vocab.get(token, vocab.get('<UNK>', 0)) for token in tokens]

def prediction(model, vocab, text, max_words=3):
    model.eval()
    try:
        tokens = tokenizer.tokenize(text.lower())  # No punkt needed
        idx_to_word = {i: w for w, i in vocab.items()}

        for _ in range(max_words):
            indices = text_indices(tokens, vocab)

            # Pad/truncate to 23 tokens
            if len(indices) > 23:
                indices = indices[-23:]
            else:
                indices = [0] * (23 - len(indices)) + indices

            input_tensor = torch.tensor(indices, dtype=torch.long).unsqueeze(0).to(device)

            with torch.no_grad():
                output = model(input_tensor)
                _, predicted_index = torch.max(output, dim=1)

            predicted_word = idx_to_word.get(predicted_index.item(), "<UNK>")

            if predicted_word in ["<UNK>", ".", ","]:
                break  # optional: stop if the word is not meaningful

            tokens.append(predicted_word)

        return " ".join(tokens)

    except Exception as e:
        print("Error in prediction():", str(e))
        raise e

# --------------------------
# Load Model
# --------------------------
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = LSTMmodel(vocab_size=len(vocab))
model.load_state_dict(torch.load("nextword_model.pt", map_location=device))
model.to(device)

# --------------------------
# Flask App
# --------------------------
app = Flask(__name__)
CORS(app)

@app.route('/predict', methods=['POST'])
def predict_route():
    try:
        data = request.get_json(force=True)

        if not data or "text" not in data:
            return jsonify({"error": "Missing 'text' key in request body"}), 400

        input_text = data["text"].strip()
        if not input_text:
            return jsonify({"error": "Input text is empty"}), 400

        print(f"[Flask] Received input: {input_text}")
        result = prediction(model, vocab, input_text)
        print(f"[Flask] Prediction result: {result}")

        return jsonify({"next_word": result}), 200

    except Exception as e:
        import traceback
        traceback.print_exc()
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    print("Testing direct prediction:")
    test_text = "virat kohli"
    print(prediction(model, vocab, test_text))
    app.run(host="0.0.0.0", port=5000)
