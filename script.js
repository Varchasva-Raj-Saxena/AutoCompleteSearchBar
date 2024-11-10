const btnEle = document.querySelector(".search-btn");
const containerEle = document.querySelector(".search-container");
const suggestionsContainer = document.querySelector(".suggestions-container");
const searchInput = document.querySelector(".search-input");

let currentFocus = -1;

btnEle.addEventListener("click", () => {
    containerEle.classList.toggle("active");
    searchInput.focus();
    suggestionsContainer.innerHTML = "";
});

searchInput.addEventListener("input", async () => {
    const query = searchInput.value;
    const cursorPosition = searchInput.selectionStart;

    // Split text into words and identify the word being edited
    const beforeCursor = query.slice(0, cursorPosition);
    const afterCursor = query.slice(cursorPosition);
    
    // Find word boundaries around cursor
    const wordsBeforeCursor = beforeCursor.split(" ");
    const wordsAfterCursor = afterCursor.split(" ");
    
    // Get the partial word at cursor
    const currentWordBefore = wordsBeforeCursor[wordsBeforeCursor.length - 1];
    const currentWordAfter = wordsAfterCursor[0];
    
    // Combine the parts of the current word
    const currentWord = (currentWordBefore + currentWordAfter).trim();
    
    // Get the part of the word before the cursor for matching
    const searchPrefix = currentWordBefore.trim();

    if (searchPrefix.length > 0) {
        // Convert the search prefix to lowercase before sending to the server
        const lowercasePrefix = searchPrefix.toLowerCase();
        
        const response = await fetch("http://localhost:8080", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ query: lowercasePrefix })
        });

        const suggestions = await response.json();
        
        console.log("Suggestions from server:", suggestions);
        // Calculate prefix and suffix for suggestion display
        const prefixWords = wordsBeforeCursor.slice(0, -1);
        const suffixWords = wordsAfterCursor.slice(1);
        const prefixText = prefixWords.length > 0 ? prefixWords.join(" ") + " " : "";
        const suffixText = suffixWords.length > 0 ? " " + suffixWords.join(" ") : "";

        displaySuggestions(suggestions, prefixText, suffixText, searchPrefix);
    } else {
        suggestionsContainer.innerHTML = "";
    }
});

function displaySuggestions(suggestions, prefixText, suffixText, searchPrefix) {
    suggestionsContainer.innerHTML = "";
    currentFocus = -1;

    suggestions.forEach(suggestion => {
        const item = document.createElement("div");
        item.classList.add("suggestion-item");
        
        // Construct full suggestion text with surrounding words
        const fullText = `${prefixText}${suggestion}${suffixText}`.trim();
        item.textContent = fullText;

        item.addEventListener("click", () => {
            searchInput.value = fullText;
            searchInput.focus();
            suggestionsContainer.innerHTML = "";
        });

        suggestionsContainer.appendChild(item);
    });
}

searchInput.addEventListener("keydown", (e) => {
    const items = suggestionsContainer.querySelectorAll(".suggestion-item");
    if (e.key === "ArrowDown") {
        currentFocus++;
        addActive(items);
    } else if (e.key === "ArrowUp") {
        currentFocus--;
        addActive(items);
    } else if (e.key === "Enter") {
        e.preventDefault();
        if (currentFocus > -1 && items[currentFocus]) {
            searchInput.value = items[currentFocus].textContent;
            suggestionsContainer.innerHTML = "";
        }
    }
});

function addActive(items) {
    if (!items) return;
    removeActive(items);
    if (currentFocus >= items.length) currentFocus = 0;
    if (currentFocus < 0) currentFocus = items.length - 1;
    items[currentFocus].classList.add("active-suggestion");
}

function removeActive(items) {
    items.forEach(item => item.classList.remove("active-suggestion"));
}

document.addEventListener("click", (e) => {
    if (!containerEle.contains(e.target)) {
        suggestionsContainer.innerHTML = "";
    }
});
