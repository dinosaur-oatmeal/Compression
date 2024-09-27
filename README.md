# File Compression and Encryption Tool

## Description

This project is a versatile toolset designed for file compression and encryption. It enables users to efficiently compress ASCII-based files into a compact `.oats` format and decompress `.oats` files back into the `.txt` format. Additionally, the tool provides functionality to encode and decode files of any type using a user-defined key utilizing a XOR-based encryption mechanism.

## Features

- **Compression:** Losslessly compress files containing ASCII characters using Huffman coding to effectively reduce file size.
- **Decompression:** Restore compressed `.oats` files to a `.txt` format.
- **Encoding:** Secure files using an XOR-based encoding system that works across various file formats.
- **Decoding:** Decode encoded files back to their original format using the same key used during encoding.
- **Adaptive File Naming:** Automatically adjusts file names based on the performed operation, appending suffixes like `_compressed.oats` or `_encoded.oats` and deletes temporary files as needed.

## Algorithms and Data Structures

### Huffman Coding

Huffman coding is a lossless data compression algorithm that assigns variable-length codes to input characters based on their frequencies. Characters with higher frequencies are assigned shorter prefix codes, while those with lower frequencies receive longer prefix codes. This ensures that the overall size of the compressed data is minimized as common characters take up less storage.

- **Frequency Calculation:** The program reads the input file and calculates the frequency of each ASCII character.
- **Min Heap Construction:** A min heap is built using the character frequencies to efficiently retrieve the two least frequent nodes.
- **Huffman Tree Construction:** By repeatedly extracting the two nodes with the smallest frequencies from the heap and merging them into a new node, a binary Huffman tree is constructed.
- **Code Generation:** Traversing the Huffman tree generates unique prefix-free binary codes for each character, which are used for compression and decompression.

### XOR-Based Encryption

The tool employs a simple XOR-based encryption mechanism to secure files. This method involves using a user-defined key to perform a bitwise XOR operation on each byte of the file. The same key is used to reverse the process during decryption.

- **Encoding:** Each byte of the input file is XORed with a corresponding byte from the key, cycling through the key as necessary.
- **Decoding:** Applying the same XOR operation with the same key on the encoded file retrieves the original data.

### Data Structures

- **Min Heap:** Utilized during the construction of the Huffman tree to efficiently retrieve the nodes with the smallest frequencies.
- **Huffman Tree:** A binary tree where each leaf node represents an input character, and the path from the root to a leaf node defines the character's Huffman code.
- **Dynamic Arrays:** Used to store Huffman codes and manage various buffers during compression, decompression, encoding, and decoding processes.