#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXCHAR 1024

// ** STRUCTS **

// node
typedef struct Node
{
    char character;
    int frequency;
    struct Node *leftPtr, *rightPtr;
}
NODE;

// min heap
typedef struct minHeap
{
    int size;
    struct Node **array;
}
MINHEAP;






// ** MIN HEAP AND HUFFMAN TREE CODE **

// create a new node for minHeap
NODE* CreateNewNode(char character, int frequency)
{
    NODE *temp = malloc(sizeof(NODE));

    temp->leftPtr = temp->rightPtr = NULL;
    temp->character = character;
    temp->frequency = frequency;

    return temp;
}

void SwapNode(NODE **a, NODE **b)
{
    NODE *temp = *a;
    *a = *b;
    *b = temp;
}

// run heapify process
void Heapify(MINHEAP *minHeap, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    // larger number to left
    if(left < minHeap->size &&
    minHeap->array[left]->frequency < minHeap->array[smallest]->frequency)
    {
        smallest = left;
    }

    // larger number to right
    if(right < minHeap->size &&
    minHeap->array[right]->frequency < minHeap->array[smallest]->frequency)
    {
        smallest = right;
    }

    if(smallest != idx)
    {
        SwapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        // heapify until smallest is found
        Heapify(minHeap, smallest);
    }
}

// build min heap based on frequency numbers
MINHEAP* BuildMinHeap(int frequency[])
{
    int size = 0;

    // find size for the minHeap
    for(int i = 0; i < MAXCHAR; ++i)
    {
        if(frequency[i] > 0)
        {
            size++;
        }
    }

    //initialize heap
    MINHEAP *minHeap = malloc(sizeof(MINHEAP));
    minHeap->size = size;
    minHeap->array = malloc(minHeap->size * sizeof(NODE*));

    int index = 0;

    // create nodes for character with non-zero frequencies
    for(int i = 0; i < MAXCHAR; ++i)
    {
        if(frequency[i] > 0)
        {
            minHeap->array[index] = CreateNewNode((char)i, frequency[i]);
            index++;
        }
    }

    // heapify until min heap is completely made
    for(int i = (minHeap->size - 1) / 2; i >= 0; --i)
    {
        Heapify(minHeap, i);
    }

    return minHeap;
}

NODE* BuildHuffmanTree(MINHEAP *minHeap)
{
    while(minHeap->size != 1)
    {
        // extract smallest 2 frequency nodes from the min heap
        NODE *left = minHeap->array[0];
        minHeap->array[0] = minHeap->array[minHeap->size - 1];
        minHeap->size--;
        Heapify(minHeap, 0);

        NODE *right = minHeap->array[0];
        minHeap->array[0] = minHeap->array[minHeap->size - 1];
        minHeap->size--;
        Heapify(minHeap, 0);

        // make the new node for the huffman tree
        NODE *top = CreateNewNode('$', left->frequency + right->frequency);
        top->leftPtr = left;
        top->rightPtr = right;

        // insert new node into the tree
        minHeap->size++;
        minHeap->array[minHeap->size - 1]= top;

        // heapify to move smallest nodes up
        for(int i = (minHeap->size - 1) / 2; i >= 0; --i)
        {
            Heapify(minHeap, i);
        }
    }

    //returns root node
    return minHeap->array[0];
}

void FreeMinHeap(MINHEAP *minHeap)
{
    // frees array element (don't free nodes in array)
    free(minHeap->array);
    free(minHeap);
}

void FreeHuffmanTree(NODE *root)
{
    if(root == NULL)
    {
        return;
    }

    // frees nodes in minheap and huffman tree
    FreeHuffmanTree(root->leftPtr);
    FreeHuffmanTree(root->rightPtr);
    //printf("Freeing node for character: %c\n", root->character);
    free(root);
}

// counts how many times every character shows up in the file
void CalculateFrequency(const char *fileName, int frequency[MAXCHAR])
{
    int inputFile = open(fileName, O_RDONLY);

    if(inputFile == -1)
    {
        printf("File failed to open.\n");
        exit(0);
    }

    char buffer[1024];
    ssize_t bytesRead;

    // read file in chunks and update frequencies of characters
    while((bytesRead = read(inputFile, buffer, sizeof(buffer))) > 0)
    {
        for(ssize_t i = 0; i < bytesRead; ++i)
        {
            frequency[(unsigned char)buffer[i]]++;
        }
    }

    close(inputFile);
}






// ** DEBUG PRINTING TREE CODE **

void PrintFrequencies(int frequency[])
{
    for(int i = 0; i < MAXCHAR; ++i)
    {
        if(frequency[i] > 0)
        {
            printf("Character '%c' has frequency %d\n", (char)i, frequency[i]);
        }
    }
}

void PrintCodes(NODE *root, int array[], int top)
{
    // traverse left
    if(root->leftPtr)
    {
        array[top] = 0;
        PrintCodes(root->leftPtr, array, top + 1);
    }

    // traverse right
    if(root->rightPtr)
    {
        array[top] = 1;
        PrintCodes(root->rightPtr, array, top + 1);
    }

    // printing
    if (!root->leftPtr && !root->rightPtr)
    {
        printf("%c: ", root->character);
        for (int i = 0; i < top; ++i)
        {
            printf("%d", array[i]);
        }
        printf("\n");
    }
}






// ** COMPRESSION CODE **

void GetCompressedFileName(char *inputFileName, char *compressedFileName)
{
    // Copy the input file name to the compressed file name buffer
    strcpy(compressedFileName, inputFileName);

    // Find the last occurrence of '.' in the input file name
    char *dot = strrchr(compressedFileName, '.');

    // If there is no '.', append '_compressed.oats' to the end of the file name
    if (dot == NULL)
    {
        strcat(compressedFileName, "_compressed.oats");
    }

    else
    {
        // Otherwise, insert '_compressed' before the '.'
        strcpy(dot, "_compressed.oats");
    }
}

// store huffman codes in a table to be used during compression
void StoreCodes(NODE *root, int array[], int top, char *codes[MAXCHAR])
{
    // add 0 to huffman code
    if(root->leftPtr)
    {
        array[top] = 0;
        StoreCodes(root->leftPtr, array, top + 1, codes);
    }

    // add 1 to huffman code
    if(root->rightPtr)
    {
        array[top] = 1;
        StoreCodes(root->rightPtr, array, top + 1, codes);
    }

    // see if current node is leaf (input character)
    if(!root->leftPtr && !root->rightPtr)
    {
        //printf("Storing code for character: %c\n", root->character);
        codes[(unsigned char)root->character] = malloc(top + 1);

        // add huffman code into lookup table (array)
        for(int i = 0; i < top; ++i)
        {
            codes[(unsigned char)root->character][i] = array[i] + '0';
        }

        // add NULL
        codes[(unsigned char)root->character][top] = '\0';
    }
}

void FreeCodes(char *codes[MAXCHAR])
{
    for(int i = 0; i < MAXCHAR; ++i)
    {
        if(codes[i])
        {
            //printf("Freeing code for character: %c\n", (char)i);
            free(codes[i]);
        }
    }
}

// write huffman tree data into file for later decompression
void WriteHuffmanTree(NODE *root, int outputFile)
{
    if (root == NULL) 
    {
        return;
    }

    // see if current node is internal
    if (root->leftPtr != NULL || root->rightPtr != NULL)
    {
        char internalNode = '\0';
        write(outputFile, &internalNode, sizeof(char));

        // recursively write the left and right pointers to the file
        WriteHuffmanTree(root->leftPtr, outputFile);
        WriteHuffmanTree(root->rightPtr, outputFile);
    }
    else
    {
        // write the character of the leaf node
        write(outputFile, &(root->character), sizeof(char));
    }
}

// compress input file with huffman codes and write compressed data
void CompressFile(const char *fileName, const char *outputFileName, NODE *root, char *codes[MAXCHAR])
{
    // input for read
    int inputFile = open(fileName, O_RDONLY);
    if(inputFile == -1)
    {
        printf("File failed to open.\n");
        exit(0);
    }

    // output for write
    int outputFile = open(outputFileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (outputFile == -1)
    {
        printf("Output file failed to open.\n");
        close(inputFile);
        exit(0);
    }

    // write huffman tree to beginning of file
    WriteHuffmanTree(root, outputFile);

    char buffer[1024];
    ssize_t bytesRead;

    // store bits before writing to output
    char bitBuffer = 0;

    // track number of bits in the buffer
    int bitCount = 0;

    // buffer for output to minimize write calls
    char outputBuffer[1024];
    int outputBufferIndex = 0;

    // read files in chunks
    while ((bytesRead = read(inputFile, buffer, sizeof(buffer))) > 0)
    {
        // process each byte
        for (ssize_t i = 0; i < bytesRead; ++i)
        {
            // find huffman code for the character
            char *code = codes[(unsigned char)buffer[i]];

            // process each bit in the huffman code
            for (int j = 0; code[j] != '\0'; ++j)
            {
                // add bit to the bit buffer
                bitBuffer = (bitBuffer << 1) | (code[j] - '0');
                bitCount++;

                // fill up outputBuffer
                if (bitCount == 8)
                {
                    outputBuffer[outputBufferIndex++] = bitBuffer;
                    bitBuffer = 0;
                    bitCount = 0;

                    // only write to output when outputBuffer is full
                    if(outputBufferIndex == sizeof(outputBuffer))
                    {
                        write(outputFile, outputBuffer, sizeof(outputBuffer));
                        outputBufferIndex = 0;
                    }
                }
            }
        }
    }

    // write rest of output buffer bytes
    if (outputBufferIndex > 0)
    {
        write(outputFile, outputBuffer, outputBufferIndex);
    }

    // write leftover bits and pad them
    if (bitCount > 0)
    {
        // shift bits left to fill byte
        bitBuffer <<= (8 - bitCount);
        write(outputFile, &bitBuffer, 1);
    }

    close(inputFile);
    close(outputFile);
}

// see if file is valid for compression
bool ASCII(char *inputFileName)
{
    FILE *inputFile = fopen(inputFileName, "r");

    if(inputFile == NULL)
    {
        printf("Error: input file failed to open.\n");
        exit(0);
    }

    int character;
    while((character = fgetc(inputFile)) != EOF)
    {
        if(character < 0 || character > 127)
        {
            fclose(inputFile);
            return false;
        }
    }

    fclose(inputFile);
    return true;
}




// ** DECOMPRESSION CODE **

void GetDecompressedFileName(char *inputFileName, char *decompressedFileName)
{
    // copy input name to buffer
    strcpy(decompressedFileName, inputFileName);

    // find last '.' in name
    char *dot = strrchr(decompressedFileName, '.');

    // alter filename depending on input file
    if (strstr(decompressedFileName, "_encoded.oats") != NULL)
    {
        strcpy(dot - 8, "_decompressed.txt");
    }
    
    else if (strstr(decompressedFileName, "_compressed.oats") != NULL)
    {
        strcpy(dot - 11, "_decompressed.txt");
    }
}

// read huffman tree from file and reconstruct it
NODE* ReadHuffmanTree(int inputFile)
{
    char character;
    if(read(inputFile, &character, sizeof(char)) != sizeof(char))
    {
        return NULL;
    }

    NODE *node = CreateNewNode(character, 0);

    // internal node
    if(character == '\0')
    {
        node->leftPtr = ReadHuffmanTree(inputFile);
        node->rightPtr = ReadHuffmanTree(inputFile);
    }

    return node;
}

// decompress file with huffman tree
void DecompressFile(const char *inputFileName, const char *outputFileName)
{
    // input for read
    int inputFile = open(inputFileName, O_RDONLY);
    if(inputFile == -1)
    {
        printf("File failed to open.\n");
        exit(0);
    }

    // output for write
    int outputFile = open(outputFileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (outputFile == -1)
    {
        printf("Output file failed to open.\n");
        close(inputFile);
        exit(0);
    }

    NODE *root = ReadHuffmanTree(inputFile);

    // avoid seg faults if the wrong key is provided
    if(root == NULL || (root->leftPtr == NULL && root->rightPtr == NULL))
    {
        close(inputFile);
        close(outputFile);
        remove(outputFileName);
        printf("Failed to read Huffman Tree. Incorrect key provided.\n");
        exit(0);
    }

    NODE *current = root;

    char buffer[1024];
    ssize_t bytesRead;

    // buffer for output to minimize write calls
    char outputBuffer[1024];
    int outputBufferIndex = 0;

    // read file in chunks
    while ((bytesRead = read(inputFile, buffer, sizeof(buffer))) > 0)
    {
        // process each byte
        for (ssize_t i = 0; i < bytesRead; ++i)
        {
            // buffer to hold bits up to a byte
            unsigned char bitBuffer = (unsigned char)buffer[i];

            // process each bit
            for (int bitCount = 0; bitCount < 8; ++bitCount)
            {
                // traverse huffman tree on current bit
                if ((bitBuffer & (1 << (7 - bitCount))) == 0)
                {
                    current = current->leftPtr;
                }
                else
                {
                    current = current->rightPtr;
                }

                // if leaf node, write character to output buffer
                if (!current->leftPtr && !current->rightPtr)
                {
                    outputBuffer[outputBufferIndex++] = current->character;

                    // only write when outputBuffer is full
                    if (outputBufferIndex == sizeof(outputBuffer))
                    {
                        write(outputFile, outputBuffer, sizeof(outputBuffer));
                        outputBufferIndex = 0;
                    }

                    // reset to root
                    current = root;
                }
            }
        }
    }

    // write any leftover data to output
    if (outputBufferIndex > 0)
    {
        write(outputFile, outputBuffer, outputBufferIndex);
    }

    close(inputFile);
    close(outputFile);

    // free dynamic memory
    FreeHuffmanTree(root);

    // delete temporary file 
    if (remove(inputFileName) != 0) 
    {
        printf("Error deleting original file.\n");
    }
}






// ** ENCODE / DECODE

void GetEncodedFileName(char *inputFileName, char *encodedFileName)
{
    // copy file name into buffer
    strcpy(encodedFileName, inputFileName);

    // find last '.' in name
    char *dot = strrchr(encodedFileName, '.');

    if(strstr(encodedFileName, "_compressed.oats") != NULL)
    {
        strcpy(dot - 11, "_encoded.oats");
        //printf("\n%s\n", encodedFileName);
    }

    else if(strstr(encodedFileName, "_encoded.oats") != NULL)
    {
        strcpy(dot - 8, "_compressed.oats");
        //printf("\n%s\n", encodedFileName);
    }

    else
    {
        // get original extension
        const char *extension = strchr(inputFileName, '.');
        size_t extensionLength = strlen(dot);
        char extensionBuffer[16];
        strcpy(extensionBuffer, dot);

        char *encodedPosition = strstr(encodedFileName, "_encoded");

        // decoding filename
        if(encodedPosition != NULL && (encodedPosition + 8 == dot))
        {
            strcpy(encodedPosition, extensionBuffer);
        }

        // encoding filename
        else
        {
            // cut off end of original filename
            *dot = '\0';

            // add data to old filename
            strcat(encodedFileName, "_encoded");
            strcat(encodedFileName, extensionBuffer);
        }
    }
}

void Encode(const char *inputFileName, const char *outputFileName, const char *key)
{
    FILE *inputFile = fopen(inputFileName, "rb");
    if(inputFile == NULL)
    {
        printf("Input file failed to open.\n");
        exit(0);
    }

    FILE *outputFile = fopen(outputFileName, "wb");
    if(outputFile == NULL)
    {
        printf("Output file failed to open.\n");
        fclose(inputFile);
        exit(0);
    }

    // NULL key
    size_t keyLength = strlen(key);
    if(keyLength == 0)
    {
        printf("Key can't be empty.\n");
        fclose(inputFile);
        fclose(outputFile);
        exit(0);
    }

    // fixed buffer size
    const size_t bufferSize = 1024;
    unsigned char buffer[bufferSize];
    size_t bytesRead;

    // read and write file in chunks
    while ((bytesRead = fread(buffer, 1, bufferSize, inputFile)) > 0)
    {
        for (size_t i = 0; i < bytesRead; ++i)
        {
            // bit mask to encode file based on key input
            unsigned char bitMask = 1 << (key[i % keyLength] % 8);
            buffer[i] ^= bitMask;
        }

        fwrite(buffer, 1, bytesRead, outputFile);
        fflush(outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    // delete temporary files
    if (strstr(inputFileName, "_encoded.oats") == NULL)
    {
        if (remove(inputFileName) != 0) 
        {
            printf("Error deleting original file.\n");
        }
    }
}






// ** MAIN FUNCTION **

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("File must be provided on command line...exiting\n");
        exit(0);
    }

    int choice;
    char *fileName = argv[1];

    // find file extension
    const char *extension = strchr(fileName, '.');
    int isHuff = extension && strcmp(extension, ".oats") == 0;

    printf("1. compress and encrypt file\n2. decrypt and decompress file\n");
    printf("\n3. compress a file\n4. decompress a file\n5. encrypt / decrypt a file\n\nYour Choice: ");
    scanf("%d", &choice);

    // Compression
    if(choice == 1 || choice == 3)
    {
        if(isHuff)
        {
            printf("Error: can't compress a .oats file.\n");
            exit(0);
        }

        if(!ASCII(fileName))
        {
            printf("Error: can't compress this file.\n");
        }

        char compressedFileName[500];
        GetCompressedFileName(fileName, compressedFileName);

        // step 1: Calculate frequency of each character
        int frequency[MAXCHAR] = {0};
        CalculateFrequency(fileName, frequency);

        #ifdef PRINT
            PrintFrequencies(frequency);
        #endif

        // step 2: Build min heap from frequencies
        MINHEAP *minHeap = BuildMinHeap(frequency);

        // step 3: Build Huffman tree
        NODE *root = BuildHuffmanTree(minHeap);

        char *codes[MAXCHAR] = {0};
        int array[MAXCHAR], top = 0;

        #ifdef PRINT
            PrintCodes(root, array, top);
        #endif

        // step 4: store codes for tree
        StoreCodes(root, array, top, codes);

        // step 5: write compressed data to file
        CompressFile(fileName, compressedFileName, root, codes);

        // free dynamic memory
        FreeCodes(codes);
        FreeHuffmanTree(root);
        FreeMinHeap(minHeap);

        // necessary if file is being encoded immediately after compression
        strcpy(fileName, compressedFileName);
    }

    // Encrypt / Decrypt
    if(choice == 1 || choice == 2 || choice == 5)
    {
        // never encode source file
        if(strcmp(fileName, "Compression.c") == 0)
        {
            printf("Error: This file can't be encoded.\n");
            exit(0);
        }

        // key used for encryption
        char key[101];
        printf("Enter your key for the file: ");
        if(scanf("%100s", key) != 1)
        {
            printf("Error: key overflow.\n");
            exit(0);
        }

        // get encoded file name from compressed file name
        char encodedFileName[500];
        GetEncodedFileName(fileName, encodedFileName);

        // encode to new file with key
        Encode(fileName, encodedFileName, key);

        strcpy(fileName, encodedFileName);

        //printf("Encode Complete\n");
    }

    // Decompression
    if(choice == 2 || choice == 4)
    {
        // only decompress valid file format
        if(!isHuff)
        {
            printf("Error: can only decompress .oats files\n");
            exit(0);
        }

        // get decompressed file name from encoded filename
        char decompressedFileName[500];
        GetDecompressedFileName(fileName, decompressedFileName);

        // decompress file to .txt
        DecompressFile(fileName, decompressedFileName);
    }

    return 0;
}