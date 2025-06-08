# fontconverter.py
"""
Font Converter Script for Embedded Displays (Adafruit GFX style)

This script converts TTF/OTF font files to a C header file with bitmap arrays and glyph tables,
similar to Adafruit's fontconvert.c, but in Python and cross-platform.

Usage:
    python fontconverter.py <input_font_file> <output_header_file> --size <font_size>

Dependencies:
    pip install pillow

Example:
    python fontconverter.py RobotoSlab-Regular.ttf RobotoSlab_16.h --size 16
"""
import sys
import os
import argparse
from PIL import Image, ImageFont, ImageDraw

ASCII_FIRST = 32
ASCII_LAST = 126

class Glyph:
    def __init__(self, bitmap_offset, width, height, xAdvance, xOffset, yOffset):
        self.bitmap_offset = bitmap_offset
        self.width = width
        self.height = height
        self.xAdvance = xAdvance
        self.xOffset = xOffset
        self.yOffset = yOffset

def render_glyph(font, char):
    # Render a single character to a monochrome bitmap
    (width, height), (offset_x, offset_y) = font.font.getsize(char)
    print(char , (width, height), (offset_x, offset_y))
    image = Image.new('L', (width, height), 0)
    draw = ImageDraw.Draw(image)
    draw.text((-offset_x, -offset_y), char, 255, font=font)
    # For debugging: show the rendered glyph image
    # image.show()  # Uncomment to display the glyph window
    return image , (offset_x, offset_y)

def font_to_c_array(font_path, output_path, size=16, first=ASCII_FIRST, last=ASCII_LAST):
    font = ImageFont.truetype(font_path, size)
    bitmaps = []
    glyphs = []
    bitmap_offset = 0
    for code in range(first, last+1):
        char = chr(code)
        img , (offset_x, offset_y)= render_glyph(font, char)
        # if char == 'M':
        #     try:
        #         img.show(char)
        #     except:
        #         pass
        # img = img.convert('1')  # 1-bit pixels
        
        # if char == 'M':
        #     try:
        #         img.show(char)
        #     except:
        #         pass
        width, height = img.size
        # Pack bits row by row
        char_bitmap = []
        # row = 0
        # bits = 0
        # for y in range(height):
        #     for x in range(width):
        #         if img.getpixel((x, y)):
        #             row |= (1 << (7 - (x % 8)))
        #         bits += 1
        #         if bits == 8 :
        #             char_bitmap.append(row)
        #             row = 0
        #             bits = 0
        
        # if bits :
        #     char_bitmap.append(row)
        bits = []
        for y in range(height):
            for x in range(width):
                if img.getpixel((x, y))>50:
                    bits.append(1)
                else:
                    bits.append(0)
        row = 0
        for i in range(len(bits)):
            if i and (i % 8)==0:
                char_bitmap.append(row)
                row=0
            row |= (bits[i] << (7 - (i % 8)))
        
        if len(bits):
            char_bitmap.append(row)

        bitmaps.extend(char_bitmap)
        xAdvance = width + 1 - offset_x
        xOffset = 1
        yOffset = offset_y - int(size*1.3)  #-font.getmetrics()[0] + img.getbbox()[1] if img.getbbox() else 0
        glyphs.append(Glyph(bitmap_offset, width, height, xAdvance, xOffset, yOffset))
        bitmap_offset += len(char_bitmap)
    # Write C header
    with open(output_path, 'w') as f:
        f.write('// Generated font header\n')
        f.write(f'// Font: {os.path.basename(font_path)}, Size: {size}\n')
        f.write('#pragma once\n#include <Adafruit_GFX.h>\n')
        f.write('const uint8_t fontBitmaps[] PROGMEM = {\n    ')
        for i, b in enumerate(bitmaps):
            f.write(f'0x{b:02X}, ')
            if (i+1) % 16 == 0:
                f.write('\n    ')
        f.write('\n};\n\n')
        f.write('const GFXglyph fontGlyphs[] PROGMEM = {\n')
        for i, g in enumerate(glyphs):
            f.write(f'    {{{g.bitmap_offset}, {g.width}, {g.height}, {g.xAdvance}, {g.xOffset}, {g.yOffset}}}, // 0x{first+i:02X} \'{chr(first+i)}\'\n')
        f.write('};\n')
        f.write(f'const GFXfont LCD_Font PROGMEM = {{\n    (uint8_t *)fontBitmaps,\n    (GFXglyph *)fontGlyphs,\n    0x{first:02X}, 0x{last:02X}, {int(size*1.3)}\n}};\n')



def main():
    parser = argparse.ArgumentParser(description='Font Converter for Embedded Displays (Adafruit GFX style)')
    parser.add_argument('input_font', help='Input font file (TTF/OTF)')
    parser.add_argument('output_header', help='Output C header file')
    parser.add_argument('--size', type=int, default=16, help='Font size (default: 16)')
    parser.add_argument('--first', type=int, default=ASCII_FIRST, help='First char code (default: 32)')
    parser.add_argument('--last', type=int, default=ASCII_LAST, help='Last char code (default: 126)')
    args = parser.parse_args()
    font_to_c_array(args.input_font, args.output_header, args.size, args.first, args.last)

if __name__ == '__main__':
    main()
