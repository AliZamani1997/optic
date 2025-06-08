import sys
from PIL import Image, ImageFont, ImageDraw

# SETTINGS
FONT_PATH = "scripts/RobotoSlab-Regular.ttf"       # Path to TTF font file
import sys
from PIL import Image, ImageFont, ImageDraw

# SETTINGS
FONT_PATH = "scripts/RobotoSlab-Regular.ttf"       # Path to TTF font file
FONT_SIZE = 9                    # Font size in points
CHARS = ''.join([chr(i) for i in range(32, 127)])  # ASCII 32-126
FONT_NAME = "MyMono9pt7b"        # C variable name

def render_glyph(char, font):
    """Render a single glyph as a monochrome bitmap and return bitmap + metrics."""
    # Get bounding box: (left, top, right, bottom)
    bbox = font.getbbox(char)
    if bbox is None or bbox[2] <= bbox[0] or bbox[3] <= bbox[1]:
        # Space or empty glyph
        width = font.getlength(char)
        return None, (0, 0, int(width), 0, 0)

    left, top, right, bottom = bbox
    w, h = right - left, bottom - top

    # Create image large enough for the glyph
    img = Image.new('L', (w, h), color=0)
    d = ImageDraw.Draw(img)
    # Draw text at (0,0) so the whole glyph is in the image
    d.text((-left, -top), char, font=font, fill=255)

    # Metrics
    xAdvance = int(font.getlength(char))  # Distance to advance cursor
    xOffset = left  # X dist from cursor pos to UL corner
    yOffset = -top  # Y dist from cursor pos to UL corner

    return img.convert('1'), (w, h, xAdvance, xOffset, yOffset)

def bitmap_to_bytes(img):
    """Convert PIL monochrome image to a bytes object, MSB first per row."""
    w, h = img.size
    data = []
    for y in range(h):
        byte = 0
        bits = 0
        for x in range(w):
            if img.getpixel((x, y)):
                byte |= (1 << (7 - (bits % 8)))
            bits += 1
            if bits % 8 == 0:
                data.append(byte)
                byte = 0
        if bits % 8 != 0:
            data.append(byte)
    return data

def main():
    font = ImageFont.truetype(FONT_PATH, FONT_SIZE)
    bitmap = []
    glyphs = []
    bitmap_offset = 0

    for c in CHARS:
        glyph_img, metrics = render_glyph(c, font)
        if glyph_img is None:
            # Empty glyph
            glyphs.append((bitmap_offset, 0, 0, metrics[2], 0, 0))
            continue

        # Convert to bytes
        bits = bitmap_to_bytes(glyph_img)
        bitmap.extend(bits)
        w, h, xAdvance, xOffset, yOffset = metrics
        glyphs.append((bitmap_offset, w, h, xAdvance, xOffset, yOffset))
        bitmap_offset += len(bits)



    # Print C arrays
    print("#pragma once")
    print(f"#include <Adafruit_GFX.h>")
    print(f"const uint8_t {FONT_NAME}Bitmaps[] PROGMEM = {{")
    for i, b in enumerate(bitmap):
        if i % 12 == 0: print("    ", end="")
        print(f"0x{b:02X}, ", end="")
        if (i+1) % 12 == 0: print()
    print("\n};\n")

    print(f"const GFXglyph {FONT_NAME}Glyphs[] PROGMEM = {{")
    for i, g in enumerate(glyphs):
        print(f"    {{{g[0]}, {g[1]}, {g[2]}, {g[3]}, {g[4]}, {g[5]}}},  // 0x{ord(CHARS[i]):02X} '{CHARS[i]}'")
    print("};\n")

    ascent, descent = font.getmetrics()
    yAdvance = ascent + descent

    print(f"const GFXfont {FONT_NAME} PROGMEM = {{")
    print(f"  (uint8_t *){FONT_NAME}Bitmaps,")
    print(f"  (GFXglyph *){FONT_NAME}Glyphs, 0x20, 0x7E, {yAdvance}")
    print("};")

if __name__ == '__main__':
    main()