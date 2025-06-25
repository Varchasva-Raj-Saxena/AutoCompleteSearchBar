# Use a lightweight Ubuntu base image
FROM ubuntu:22.04

# Set environment variables
ENV PYTHONUNBUFFERED=1 \
    DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

# Install system dependencies and Python
RUN apt-get update && \
    apt-get install -y python3 python3-pip g++ curl && \
    apt-get clean

# Set working directory
WORKDIR /AutoCompleteSearchBar

# Copy project files
COPY . .

# Install Python dependencies
RUN pip3 install --no-cache-dir -r requirements.txt

# Compile the C++ autocomplete server
RUN g++ Final.cpp -o server -std=c++11

# Expose necessary ports
EXPOSE 5000
EXPOSE 8000

# Run all services
CMD ["bash", "-c", "./server & python3 app.py & python3 -m http.server 8000"]
