#!/usr/bin/env python3
"""
Generate placeholder tile images for terrain types
Creates simple colored BMP files for each terrain type
"""

import struct
import os

# Terrain types and their colors (RGB)
TERRAIN_COLORS = {
    'dirt': (139, 90, 43),      # Brown
    'sand': (238, 214, 175),    # Tan/beige
    'grass': (34, 139, 34),     # Forest green
    'snow': (240, 248, 255),    # Alice blue
    'swamp': (107, 142, 35),    # Olive drab
    'rough': (112, 128, 144),   # Slate gray
    'lava': (255, 69, 0),       # Orange red
    'water': (0, 105, 148),     # Deep sky blue
}

TILE_SIZE = 128  # 128x128 base resolution for zoom feature

def create_bmp(filename, width, height, r, g, b):
    """Create a simple solid-color BMP file"""
    # BMP header
    file_size = 54 + (width * height * 3)
    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)

    # DIB header (BITMAPINFOHEADER)
    dib_header = struct.pack('<IiiHHIIiiII',
        40,  # DIB header size
        width, height,
        1,   # color planes
        24,  # bits per pixel
        0,   # no compression
        width * height * 3,  # image size
        0, 0,  # resolution
        0, 0   # colors
    )

    # Pixel data (BGR format, bottom-up)
    row_size = ((width * 3 + 3) // 4) * 4  # Row must be multiple of 4 bytes
    padding = row_size - (width * 3)

    with open(filename, 'wb') as f:
        f.write(header)
        f.write(dib_header)

        for y in range(height):
            for x in range(width):
                # Add some variation to make it less boring
                variation = ((x + y) % 16) - 8
                f.write(struct.pack('BBB',
                    max(0, min(255, b + variation)),
                    max(0, min(255, g + variation)),
                    max(0, min(255, r + variation))
                ))
            # Padding
            f.write(b'\x00' * padding)

def main():
    output_dir = os.path.join(os.path.dirname(__file__), '../assets/tiles')
    os.makedirs(output_dir, exist_ok=True)

    print(f"Generating {TILE_SIZE}x{TILE_SIZE} placeholder tiles...")

    for terrain, (r, g, b) in TERRAIN_COLORS.items():
        filename = os.path.join(output_dir, f'{terrain}.bmp')
        create_bmp(filename, TILE_SIZE, TILE_SIZE, r, g, b)
        print(f"  Created: {filename}")

    print(f"\nGenerated {len(TERRAIN_COLORS)} terrain tiles in {output_dir}")

if __name__ == '__main__':
    main()
