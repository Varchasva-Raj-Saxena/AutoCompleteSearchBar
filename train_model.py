import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
from collections import Counter
from torch.utils.data import DataLoader, Dataset
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import sent_tokenize, TreebankWordTokenizer
import re
import pickle
from model import LSTMmodel  # <- Make sure this imports your model

# Download NLTK data
nltk.download('punkt')
nltk.download('stopwords')

# Use Treebank tokenizer
tokenizer = TreebankWordTokenizer()

# Step 1: Load and clean the data
with open("cricket.txt", "r", encoding="utf-8") as f:
    document = f.read()

sentences = sent_tokenize(document)
cleaned_sentences = []
for sentence in sentences:
    sentence = re.sub(r'[^a-zA-Z\s]', '', sentence)
    sentence = re.sub(r'\s+', ' ', sentence)
    sentence = sentence.strip().lower()
    if sentence:
        cleaned_sentences.append(sentence)

document = '\n'.join(cleaned_sentences)

# Tokenize entire document
tokens = []
for line in cleaned_sentences:
    tokens.extend(tokenizer.tokenize(line))

# Step 2: Create vocabulary
vocab = {'<UNK>': 0}
for token in Counter(tokens).keys():
    if token not in vocab:
        vocab[token] = len(vocab)

# Step 3: Convert text to numerical sequences
def text_indices(sentence, vocab):
    return [vocab.get(token, vocab['<UNK>']) for token in sentence]

input_numerical_sentences = [
    text_indices(tokenizer.tokenize(sentence), vocab)
    for sentence in cleaned_sentences
]

training_sequences = []
for sentence in input_numerical_sentences:
    for i in range(1, len(sentence)):
        training_sequences.append(sentence[:i+1])

max_len = max(len(seq) for seq in training_sequences)
padded_sequences = [
    [0] * (max_len - len(seq)) + seq for seq in training_sequences
]
padded_sequences = torch.tensor(padded_sequences, dtype=torch.long)

x = padded_sequences[:, :-1]
y = padded_sequences[:, -1]

# Step 4: Dataset & DataLoader
class CustomDataset(Dataset):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __len__(self):
        return self.x.shape[0]

    def __getitem__(self, idx):
        return self.x[idx], self.y[idx]

dataset = CustomDataset(x, y)
dataloader = DataLoader(dataset=dataset, batch_size=32, shuffle=True)

# Step 5: Train the model
model = LSTMmodel(vocab_size=len(vocab))
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
model.to(device)

criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)

epochs = 50
for epoch in range(epochs):
    total_loss = 0
    for batch_x, batch_y in dataloader:
        batch_x, batch_y = batch_x.to(device), batch_y.to(device)
        optimizer.zero_grad()
        output = model(batch_x)
        loss = criterion(output, batch_y)
        loss.backward()
        optimizer.step()
        total_loss += loss.item()
    print(f"Epoch: {epoch+1}, Loss: {total_loss:.4f}")

# Step 6: Save model and vocab
with open("vocab.pkl", "wb") as f:
    pickle.dump(vocab, f)

torch.save(model.state_dict(), "nextword_model.pt")
