import sys
import os
import gzip
import shutil

def minify_html(html: str) -> str:
    # Simple minification: remove newlines and extra spaces
    import re
    html = re.sub(r'\n+', '', html)
    html = re.sub(r'>\s+<', '><', html)
    html = re.sub(r'\s{2,}', ' ', html)
    return html.strip()

def html_to_c_array(input_path: str, output_path: str, array_name: str = "update_page_gz"):
    # # Read and minify HTML
    # with open(input_path, 'r', encoding='utf-8') as f:
    #     html = f.read()
    # minified = minify_html(html)

    # # Write minified HTML to a temp file
    # temp_html = input_path + ".min"
    # with open(temp_html, 'w', encoding='utf-8') as f:
    #     f.write(minified)

    # Gzip the minified HTML
    gz_path = input_path + ".gz"
    with open(input_path, 'rb') as f_in, gzip.open(gz_path, 'wb', compresslevel=9) as f_out:
        shutil.copyfileobj(f_in, f_out)

    # Read gzipped file and convert to C array
    with open(gz_path, 'rb') as f:
        gz_bytes = f.read()

    c_array = f'/*\nServe with Content-Encoding: gzip\nadd this to header :\n\tserver.sendHeader("Content-Encoding", "gzip");\n*/\n#pragma once\n#include "Arduino.h"\nconst uint8_t {array_name}[] PROGMEM = {{\n'
    for i, b in enumerate(gz_bytes):
        if i % 16 == 0:
            c_array += '    '
        c_array += f'0x{b:02x}, '
        if (i + 1) % 16 == 0:
            c_array += '\n'
    c_array = c_array.rstrip(', \n') + '\n};\n'
    c_array += f'const uint32_t {array_name}_len = {len(gz_bytes)};\n'

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(c_array)

    # Clean up temp files
    # os.remove(temp_html)
    os.remove(gz_path)

    print(f"C array written to {output_path} (length: {len(gz_bytes)})")

def generate_pages_header(html_dir, output_path):
    page_structs = []
    page_ptrs = []
    includes = []
    for filename in os.listdir(html_dir):
        if filename.endswith('.html'):
            base = os.path.splitext(filename)[0]
            h_name = f'{base.lower()}_p.h'
            arr_name = f'{base.upper()}_PAGE'
            temp_name = f'__{base.lower()}_temp'
            includes.append(f'#include "{h_name}"')
            page_structs.append(f'_content_t {temp_name} = {{\n    .name = "{base.lower()}",\n    .address = {arr_name},\n    .size = sizeof({arr_name})\n,    .type = "text/html"\n}};')
            page_ptrs.append(f'&{temp_name}')
        if filename.endswith('.css'):
            base = os.path.splitext(filename)[0]
            h_name = f'{base.lower()}_s.h'
            arr_name = f'{base.upper()}_STYLE'
            temp_name = f'__{base.lower()}_temp'
            includes.append(f'#include "{h_name}"')
            page_structs.append(f'_content_t {temp_name} = {{\n    .name = "{base.lower()}",\n    .address = {arr_name},\n    .size = sizeof({arr_name})\n,    .type = "text/css"\n}};')
            page_ptrs.append(f'&{temp_name}')

            

    header = [
        '#pragma once',
        '#ifndef __content__',
        '#define __content__',
        '#include "Arduino.h"',
    ]
    header.extend(includes)
    header.append('\ntypedef struct{\n    const char * name ;\n    const uint8_t * address;\n    size_t size;\n    const char * type;\n}_content_t;\n')
    header.extend(page_structs)
    header.append(f'\n_content_t * __web_contents[]={{ { ", ".join(page_ptrs) } }};\n')
    header.append('#endif\n')
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(header))
    print(f"Generated {output_path} with {len(page_ptrs)} pages.")

if __name__ == "__main__":
    

    html_dir = "lib/Servers/html"
    output_dir = "lib/Servers/pages"

    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)

    os.makedirs(output_dir, exist_ok=True)

    for filename in os.listdir(html_dir):
        if filename.endswith(".html"):
            input_html = os.path.join(html_dir, filename)
            base_name = os.path.splitext(filename)[0]
            array_name = f"{base_name.upper()}_PAGE"
            output_h = os.path.join(output_dir, f"{base_name.lower()}_p.h")
            html_to_c_array(input_html, output_h, array_name)
        if filename.endswith(".css"):
            input_html = os.path.join(html_dir, filename)
            base_name = os.path.splitext(filename)[0]
            array_name = f"{base_name.upper()}_STYLE"
            output_h = os.path.join(output_dir, f"{base_name.lower()}_s.h")
            html_to_c_array(input_html, output_h, array_name)

    
    html_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../lib/Servers/html'))
    output_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../lib/Servers/pages/_content.h'))
    generate_pages_header(html_dir, output_path)
    