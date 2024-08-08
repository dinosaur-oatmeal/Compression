# File Compression and Encryption Tool

## Overview
This project offers a versatile toolset designed for file compression and optional encryption. It enables users to compress text files into `.oats` format and decompress `.oats` files back into .txt files. Additionally, it provides functionality to encode and decode `.oats` files using keys specified by the user.

## Features
- **Compression**: Losslessy Compress files using Huffman coding to reduce file size effectively.
- **Decompression**: Restore compressed files to their original state.
- **Encoding**: Secure files using an XOR-based encoding system.
- **Decoding**: Decode encoded files using the same key used for encoding.
- **Adaptive File Naming**: Automatically adjusts file names based on the operation, e.g., appending `_compressed.oats` or `_encoded.oats` and automatically delete temporary files during operation.
