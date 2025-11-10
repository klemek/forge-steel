#!/usr/bin/env python3

import os

TARGET_FILE = os.path.join(os.path.dirname(__file__), "..", "default", "inc_sentences.glsl")

SENTENCES = [
    "forge",
    "FORGE",
    "F.O.R.G.E.",
    "Fusion",
    "Of",
    "Real-time",
    "Generative",
    "Effects",
    "Everything you see",
    "Pixel by pixel",
    "Live",
    "Generated",
    "Controlled",
    "π",
    "Kleπek",
    "Code as art",
    "Art as code",
]

MAPPING = {
    "π": "0xE3"
}

def code(char: str) -> str:
    if char in MAPPING:
        return MAPPING[char]
    return "0x" + f"{ord(char):02x}".upper()

def convert(txt: str) -> str:
    out = []
    for i in range(20):
        out += [code(txt[i]) if i < len(txt) else "0x00"]
    return f"{{{', '.join(out)}}}"

def main() -> None:
    with open(TARGET_FILE, mode = 'w') as file:
        file.write(f"#ifndef INC_SENTENCES\n")
        file.write(f"#define INC_SENTENCES\n\n")
        file.write(f"#define SENTENCE_COUNT {len(SENTENCES)}\n\n")
        file.write("const int sentences[SENTENCE_COUNT][20] = {\n")
        for sentence in SENTENCES:
            file.write("    " + convert(sentence) + "," + "\n")
        file.write("};\n\n")
        file.write("const int lengths[SENTENCE_COUNT] = {\n")
        file.write("    " + ", ".join(str(len(sentence)) for sentence in SENTENCES) + "\n")
        file.write("};\n")
        file.write("#endif")

if __name__ == '__main__':
    main()