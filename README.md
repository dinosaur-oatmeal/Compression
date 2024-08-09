# File Compression and Encryption Tool

## Overview
This project offers a versatile toolset designed for file compression and encryption. It enables users to compress files containing ASCII characters into the `.oats` file format and decompress `.oats` files int `.txt` files.
This project also allows users to encode and decode any type of file.

## Features
- **Compression**: Losslessy Compress files with ASCII characters using Huffman coding to reduce file size effectively.
- **Decompression**: Restore compressed files to their original state as a `.txt` file.
- **Encoding**: Secure files using an XOR-based encoding system and works regardless of file format.
- **Decoding**: Decode encoded files using the same key used for encoding back into their original file format.
- **Adaptive File Naming**: Automatically adjusts file names based on the operation, e.g., appending `_compressed.oats` or `_encoded.oats` and automatically delete temporary files during operation.
