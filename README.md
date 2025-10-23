# Huffman-Compression-Text-
A simple implementation of Huffman coding for compressing and decompressing text files.
Supports UTF-8 encoded text files and performs efficient file compression by encoding characters into variable-length bit sequences based on frequency.


Features:
-Compresses and decompresses text files using Huffman coding.
-Full support for UTF-8 encoded text.
-Built entirely in C++, implementing:
--Custom heap, hash table, and linked list structures.
--Binary tree–based encoding and decoding.
--Bit-level file I/O for compact storage.
--Displays timing information and handles large files efficiently.

How it works:
-The program reads a text file and counts the frequency of each character.
-Builds a Huffman tree where less frequent characters get longer bit codes.
-Writes the encoded output to a .huffcoded binary file.
-Can then decode that file back into the original text.
