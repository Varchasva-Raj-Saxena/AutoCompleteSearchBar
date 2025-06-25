const btnEle               = document.querySelector(".search-btn");
const containerEle         = document.querySelector(".search-container");
const suggestionsContainer = document.querySelector(".suggestions-container");
const searchInput          = document.querySelector(".search-input");
const ghostTextContainer   = document.querySelector(".ghost-text");

const API_BASE = "http://34.131.245.79";

let currentFocus    = -1;
let pendingNextWords= "";

/** measure text width in px in same font */
const measureText = (() => {
  const canvas = document.createElement("canvas");
  const ctx    = canvas.getContext("2d");
  return (text, font) => {
    ctx.font = font;
    return ctx.measureText(text).width;
  };
})();

/** toggle open */
btnEle.addEventListener("click", () => {
  containerEle.classList.toggle("active");
  searchInput.focus();
  suggestionsContainer.innerHTML = "";
});

/** input & fetching */
searchInput.addEventListener("input", async () => {
  const val      = searchInput.value;
  const lastChar = val.slice(-1);

  // always show dropdown if typing
  containerEle.classList.add("active");

  /**––– LSTM next-word on space —––*/
  if (lastChar === " " && val.trim().length > 0) {
    const resp = await fetch(`${API_BASE}:5000/predict`, {
      method: "POST",
      headers:{"Content-Type":"application/json"},
      body: JSON.stringify({ text: val.trim() })
    });
    if (resp.ok) {
      const { next_word } = await resp.json();
      const baseCount = val.trim().split(" ").length;
      const tokens    = next_word.split(" ").slice(baseCount);
      pendingNextWords = tokens.join(" ");

      // measure & position ghost
      const style       = getComputedStyle(searchInput);
      const font        = `${style.fontWeight} ${style.fontSize} ${style.fontFamily}`;
      const prefixWidth = measureText(val, font);
      const wrapperPad  = parseInt(getComputedStyle(document.querySelector(".input-wrapper")).paddingLeft);
      ghostTextContainer.style.left = `${wrapperPad + prefixWidth}px`;
      ghostTextContainer.textContent = pendingNextWords;
    }
  } else {
    pendingNextWords = "";
    ghostTextContainer.textContent = "";
  }

  /**––– Trie prefix/infix/suffix –––*/
  // find current word before cursor
  const cursorPos         = searchInput.selectionStart;
  const beforeCursor      = val.slice(0, cursorPos);
  const wordsBefore       = beforeCursor.split(" ");
  const searchPrefix      = wordsBefore[wordsBefore.length - 1].trim().toLowerCase();

  if (searchPrefix) {
    const res = await fetch(`${API_BASE}:8080/`, {
      method:"POST",
      headers:{"Content-Type":"application/json"},
      body: JSON.stringify({ query: searchPrefix })
    });
    if (res.ok) {
      const suggestions = await res.json();
      const prefixWords = wordsBefore.slice(0,-1);
      const suffixWords = val.slice(cursorPos).split(" ").slice(1);
      const prefixText  = prefixWords.length? prefixWords.join(" ") + " " : "";
      const suffixText  = suffixWords.length? " " + suffixWords.join(" ") : "";
      displaySuggestions(suggestions, prefixText, suffixText);
    }
  } else {
    suggestionsContainer.innerHTML = "";
  }
});

/** render dropdown */
function displaySuggestions(list, prefixText, suffixText) {
  suggestionsContainer.innerHTML = "";
  currentFocus = -1;
  list.forEach(sg => {
    const item = document.createElement("div");
    item.className = "suggestion-item";
    item.textContent = `${prefixText}${sg.trim()}${suffixText}`.trim();
    item.addEventListener("click", () => {
      // Place the chosen suggestion exactly, without trailing space
      searchInput.value = item.textContent;
      containerEle.classList.remove("active");
      suggestionsContainer.innerHTML = "";
    });
    suggestionsContainer.appendChild(item);
  });
}

/** keyboard nav & tab accept */
searchInput.addEventListener("keydown", e => {
  const items = suggestionsContainer.querySelectorAll(".suggestion-item");
  if (e.key === "Tab" && pendingNextWords) {
    e.preventDefault();
    searchInput.value += pendingNextWords + " ";
    pendingNextWords = "";
    ghostTextContainer.textContent = "";
    suggestionsContainer.innerHTML = "";
    return;
  }
  if (e.key === "ArrowDown") {
    currentFocus++;
    setActive(items);
  } else if (e.key === "ArrowUp") {
    currentFocus--;
    setActive(items);
  } else if (e.key === "Enter") {
    e.preventDefault();
    if (currentFocus >= 0 && items[currentFocus]) {
      // On Enter, also insert without extra space
      searchInput.value = items[currentFocus].textContent;
      suggestionsContainer.innerHTML = "";
      currentFocus = -1;
    }
  }
});

/** highlight */
function setActive(items) {
  items.forEach(i=>i.classList.remove("active-suggestion"));
  if (!items.length) return;
  if (currentFocus >= items.length) currentFocus = 0;
  if (currentFocus < 0) currentFocus = items.length -1;
  items[currentFocus].classList.add("active-suggestion");
}

/** click outside closes */
document.addEventListener("click", e => {
  if (!containerEle.contains(e.target)) {
    containerEle.classList.remove("active");
    suggestionsContainer.innerHTML = "";
    pendingNextWords = "";
    ghostTextContainer.textContent = "";
  }
});
