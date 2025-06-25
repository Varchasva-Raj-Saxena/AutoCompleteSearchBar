import torch
import torch.nn as nn

class LSTMmodel(nn.Module):
    def __init__(self, vocab_size):
        super().__init__()
        self.emmbedding = nn.Embedding(vocab_size, 100)
        self.lstm = nn.LSTM(100, 150, batch_first=True)
        self.fc = nn.Linear(150, vocab_size)

    def forward(self, x):
        embedded = self.emmbedding(x)
        _, (final_hidden_state, _) = self.lstm(embedded)
        output = self.fc(final_hidden_state.squeeze(0))
        return output