/*
* This project is done by :
* 1-Hazem Mohamed ElKanawati , ID :221006745
* 2-Abdelrhman Yasser Ali , ID : 221004349
* 3-Youssif Hany Sayed , ID : 221005893
* 4-Mazen ElMoatazbella Ali , ID : 221005723
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TREE_HT 256
#define BUFFER_SIZE 10485 // 1MB buffer size

typedef struct code {
    char k;//Stores a character
    int l;//its Huffman code length
    int code_arr[16];//the Huffman code itself 
    struct code* p;//next code in the linked list
} code;

typedef struct Tree {
    char g;//Stores a character
    int len;//its Huffman code length
    int dec;//its Huffman code in decimal
} Tree;

struct MinHeapNode {
    char data;//Represents a node in the Huffman tree, containing the character
    unsigned freq;
    struct MinHeapNode* left, * right;
};

struct MinHeap {
    unsigned size;//Represents a min-heap, containing its current size
    unsigned capacity;
    struct MinHeapNode** array;//an array of pointers to MinHeapNode structures 
};
//Creates a new MinHeapNode with given data and frequency.
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}
//Initializes a new MinHeap with given capacity.
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}
//Swaps two MinHeapNode pointers.
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}
// Maintains the min-heap property by ensuring the smallest frequency node is at the root.
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}
//Checks if the size of the MinHeap is one.
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}
//Extracts the minimum frequency node from the MinHeap.
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}
// Inserts a new node into the MinHeap.
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}
//Builds the MinHeap.
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}
//Checks if a node is a leaf node (has no children).
int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}
//Creates and builds a MinHeap from given data and frequency arrays.
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);//sends it to the minheapify to maintain the property
    return minHeap;
}
//Builds the Huffman tree from given data and frequency arrays.
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode* left, * right, * top;
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);//summing up the two nodes to create a parent tree
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);//return the least frequency which will also maintain the property
}
//Recursively traverses the Huffman tree and writes the Huffman codes to a file and writes the encoded characters and their corresponding binary codes to a file
void printCodesIntoFile(FILE* fd2, struct MinHeapNode* root, int t[], int top) {
    static int k = 0;//counter to track the linkes list
    static code* front = NULL, * rear = NULL;//pointers to manage the linked list of 'codes' structure
    int i;
    //It traverses left and right subtrees, updating the binary code (t) accordingly (0 for left and 1 for right).
    if (root->left) {
        t[top] = 0;
        printCodesIntoFile(fd2, root->left, t, top + 1);
    }

    if (root->right) {
        t[top] = 1;
        printCodesIntoFile(fd2, root->right, t, top + 1);
    }

    if (isLeaf(root)) {
        code* data = (code*)malloc(sizeof(code));
        Tree* tree = (Tree*)malloc(sizeof(Tree));
        data->p = NULL;
        data->k = root->data;
        tree->g = root->data;
        fwrite(&tree->g, sizeof(char), 1, fd2);
        for (i = 0; i < top; i++) {
            data->code_arr[i] = t[i];
        }
        tree->len = top;
        fwrite(&tree->len, sizeof(int), 1, fd2);
        int dec = 0;
        //calculating the decimal number 
        for (i = 0; i < top; i++) {
            dec = dec * 2 + data->code_arr[i];
        }
        tree->dec = dec;
        fwrite(&tree->dec, sizeof(int), 1, fd2);
        data->l = top;
        data->p = NULL;//next linked list must be null terminated
        if (k == 0) {
            front = rear = data;
            k++;
        }
        else {
            rear->p = data;
            rear = rear->p;
        }
    }
}
//Generates Huffman codes for given data and frequencies, and writes them to a file.    
void HuffmanCodes(char data[], int freq[], int size, const char* codFilename) {
    struct MinHeapNode* root;
    int arr[MAX_TREE_HT], top = 0;

    // Special case handling for a single unique character
    if (size == 1) {
        root = newNode(data[0], freq[0]);
    }
    else {
        root = buildHuffmanTree(data, freq, size);
    }

    FILE* fd2 = fopen(codFilename, "wb");
    if (!fd2) {
        perror("Unable to open .cod file");
        exit(EXIT_FAILURE);
    }

    if (size == 1) {
        // Directly write the single character with a default bit (e.g., '0')
        code* data = (code*)malloc(sizeof(code));
        Tree* tree = (Tree*)malloc(sizeof(Tree));
        data->p = NULL;
        data->k = root->data;
        tree->g = root->data;
        fwrite(&tree->g, sizeof(char), 1, fd2);
        tree->len = 1;
        fwrite(&tree->len, sizeof(int), 1, fd2);
        tree->dec = 0;
        fwrite(&tree->dec, sizeof(int), 1, fd2);
        data->l = 1;
        data->code_arr[0] = 0;
        data->p = NULL;
    }
    else {
        printCodesIntoFile(fd2, root, arr, top);
    }

    fclose(fd2);
}
//Reads a file and calculates the frequency of each character.
void calculateFrequencies(char* filename, int* freq) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            freq[(unsigned char)buffer[i]]++;
        }
    }

    fclose(file);
}
//Compresses a file using Huffman coding, reading the codes from a .cod file, and writes the compressed data to a .com file.
void compressFile(const char* inputFilename, const char* codFilename, const char* comFilename) {
    FILE* inputFile = fopen(inputFilename, "rb");
    FILE* comFile = fopen(comFilename, "wb");
    if (!inputFile || !comFile) {
        perror("Unable to open file for compression");
        exit(EXIT_FAILURE);
    }

    FILE* codFile = fopen(codFilename, "rb");
    if (!codFile) {
        perror("Unable to open .cod file for reading");
        exit(EXIT_FAILURE);
    }

    // Read the Huffman codes from the .cod file and it's corresponding characters
    char codes[256][MAX_TREE_HT] = { 0 };
    char ch;
    while (fread(&ch, sizeof(char), 1, codFile)) {
        int len, dec;
        fread(&len, sizeof(int), 1, codFile);
        fread(&dec, sizeof(int), 1, codFile);
        for (int i = 0; i < len; i++) {
            codes[(unsigned char)ch][i] = (dec >> (len - i - 1)) & 1 ? '1' : '0';
        }
        codes[(unsigned char)ch][len] = '\0'; // Null-terminate the string
    }

    char inputBuffer[BUFFER_SIZE];
    unsigned char outputBuffer[BUFFER_SIZE];
    unsigned char byte = 0;
    int bitCount = 0;
    int outputIndex = 0;
    unsigned long long totalBits = 0;

    size_t bytesRead;
    while ((bytesRead = fread(inputBuffer, sizeof(char), BUFFER_SIZE, inputFile)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            ch = inputBuffer[i];
            char* code = codes[(unsigned char)ch];
            for (int j = 0; code[j] != '\0'; j++) {
                byte = (byte << 1) | (code[j] - '0');
                bitCount++;
                totalBits++;
                //when the bits= 8 then write it to the output file.
                if (bitCount == 8) {
                    outputBuffer[outputIndex++] = byte;
                    byte = 0;
                    bitCount = 0;
                    //Write the total number of bits used in the compressed file to ensure correct decompression.
                    if (outputIndex == BUFFER_SIZE) {
                        fwrite(outputBuffer, sizeof(unsigned char), BUFFER_SIZE, comFile);
                        outputIndex = 0;
                    }
                }
            }
        }
    }

    // Write any remaining bits
    if (bitCount > 0) {
        byte = byte << (8 - bitCount);
        outputBuffer[outputIndex++] = byte;
    }

    if (outputIndex > 0) {
        fwrite(outputBuffer, sizeof(unsigned char), outputIndex, comFile);
    }

    // Write the total number of bits at the end of the file
    fwrite(&totalBits, sizeof(unsigned long long), 1, comFile);

    fclose(inputFile);
    fclose(comFile);
    fclose(codFile);
}
//Prints the binary representation of the compressed file.
void printBinaryCompressedFile(const char* comFilename) {
    FILE* comFile = fopen(comFilename, "rb");
    if (!comFile) {
        perror("Unable to open .com file for reading");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer;
    while (fread(&buffer, sizeof(unsigned char), 1, comFile)) {
        for (int i = 7; i >= 0; i--) {
            printf("%d", (buffer >> i) & 1);
        }
    }
    printf("\n");
    fclose(comFile);
}
//Reads and prints the Huffman codes from a .cod file
struct MinHeapNode* rebuildHuffmanTreeFromCod(FILE* codFile) {
    struct MinHeapNode* root = newNode('$', 0);
    char ch_str[10];
    int len, dec;
    //function reads the characters and their corresponding length and decimal value
    while (fread(&ch_str[0], sizeof(char), 1, codFile)) {
        ch_str[1] = '\0';
        char ch = ch_str[0];
        fread(&len, sizeof(int), 1, codFile);
        fread(&dec, sizeof(int), 1, codFile);
        struct MinHeapNode* current = root;
        //navigate the huffman tree accordiing to bits in decimal value it creates new node when needed
        for (int i = 0; i < len; i++) {
            if ((dec >> (len - i - 1)) & 1) {
                if (!current->right)
                    current->right = newNode('$', 0);
                current = current->right;
            }
            else {
                if (!current->left)
                    current->left = newNode('$', 0);
                current = current->left;
            }
        }
        current->data = ch;
    }

    return root;
}
//Decompresses a file using Huffman coding, reading the codes from a .cod file, and writes the decompressed data to an output file.
void decompressFile(const char* comFilename, const char* codFilename, const char* outputFilename) {
    //opening the huffman codes from file
    FILE* codFile = fopen(codFilename, "rb");
    if (!codFile) {
        perror("Unable to open .cod file for reading");
        exit(EXIT_FAILURE);
    }
    //huffman tree is reconstructed from the cod file then it will be closed
    struct MinHeapNode* root = rebuildHuffmanTreeFromCod(codFile);
    fclose(codFile);
    //opening the compressed and the output file
    FILE* comFile = fopen(comFilename, "rb");
    FILE* outputFile = fopen(outputFilename, "w");
    if (!comFile || !outputFile) {
        perror("Unable to open file for decompression");
        exit(EXIT_FAILURE);
    }

    //This piece of code is for reading the last unsigned long long value from a binary file and then resetting the file pointer to the beginning of the file.
    //This line moves the file pointer to the position sizeof(unsigned long long) bytes before the end of the file.
    fseek(comFile, -sizeof(unsigned long long), SEEK_END);
    //Declare a variable to hold the read value
    unsigned long long totalBits;
    //This line reads the value at the current file pointer position into the totalBits variable.
    //The address of the variable to read into (&totalBits), The address of the variable to read into (&totalBits), The number of elements to read is 1, The file pointer (comFile).
    fread(&totalBits, sizeof(unsigned long long), 1, comFile);
    //The file pointer (comFile).
    fseek(comFile, 0, SEEK_SET);
    //The file is read in chunks using a buffer.
    unsigned char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];
    size_t bytesRead;
    unsigned long long bitCount = 0;
    //outputIndex: Tracks the position in the outputBuffer.
    int outputIndex = 0;
    //pointer to the start of the huffman tree
    struct MinHeapNode* current = root;
    //This loop reads data from comFile into inputBuffer in chunks of BUFFER_SIZE bytes.
    while ((bytesRead = fread(inputBuffer, 1, BUFFER_SIZE, comFile)) > 0) {
        //This loop iterates through each byte in inputBuffer.
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char byte = inputBuffer[i];
            //The inner loop processes each bit of the byte, starting from the most significant bit (7) to the least significant bit (0).
            for (int j = 7; j >= 0 && bitCount < totalBits; j--, bitCount++) {
                //This checks the value of the current bit: 1 go to the right child or 0 go to the left child
                if ((byte >> j) & 1)
                    current = current->right;
                else
                    current = current->left;
                //When a leaf node is reached, the corresponding data (decoded character) is added to outputBuffer.
                if (isLeaf(current)) {
                    outputBuffer[outputIndex++] = current->data;
                    //current is reset to root to start decoding the next sequence of bits.
                    current = root;
                    //If outputBuffer is full, it writes the buffer to outputFile and resets outputIndex.
                    if (outputIndex == BUFFER_SIZE) {
                        fwrite(outputBuffer, sizeof(char), BUFFER_SIZE, outputFile);
                        outputIndex = 0;
                    }
                }
            }
        }
    }

    // Write any remaining data in the output buffer
    if (outputIndex > 0) {
        fwrite(outputBuffer, sizeof(char), outputIndex, outputFile);
    }

    fclose(comFile);
    fclose(outputFile);
}
void printHuffmanCodes(const char* codFilename) {
    FILE* codFile = fopen(codFilename, "rb");
    if (!codFile) {
        perror("Unable to open .cod file for reading");
        exit(EXIT_FAILURE);
    }

    char ch;
    while (fread(&ch, sizeof(char), 1, codFile)) {
        int len, dec;
        fread(&len, sizeof(int), 1, codFile);
        fread(&dec, sizeof(int), 1, codFile);

        printf("%c : ", ch);
        for (int i = 0; i < len; i++) {
            printf("%d", (dec >> (len - i - 1)) & 1);
        }
        printf("\n");
    }

    fclose(codFile);
}


int main() {
    int choice;
    char inputFilename[100], codFilename[100], comFilename[100], outputFilename[100];

    while (true) {
        printf("\nMenu:\n");
        printf("1. Generate Huffman codes and print to terminal\n");
        printf("2. Compress a file\n");
        printf("3. Decompress a file\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: {
            printf("Enter the input filename: ");
            scanf("%s", inputFilename);
            snprintf(codFilename, sizeof(codFilename), "%s.cod", inputFilename);

            int freq[256] = { 0 };
            calculateFrequencies(inputFilename, freq);

            char data[256];
            int frequencies[256];
            int size = 0;

            for (int i = 0; i < 256; i++) {
                if (freq[i] > 0) {
                    data[size] = (char)i;
                    frequencies[size] = freq[i];
                    size++;
                }
            }

            HuffmanCodes(data, frequencies, size, codFilename);
            printf(".cod file generated successfully.\n");
            printf("Huffman Codes:\n");
            printHuffmanCodes(codFilename);
            break;
        }
        case 2: {
            printf("Enter the input filename: ");
            scanf("%s", inputFilename);
            snprintf(codFilename, sizeof(codFilename), "%s.cod", inputFilename);
            snprintf(comFilename, sizeof(comFilename), "%s.com", inputFilename);
            compressFile(inputFilename, codFilename, comFilename);
            printf("File compressed successfully.\n");
            break;
        }
        case 3: {
            printf("Enter the compressed (.com) filename: ");
            scanf("%s", comFilename);
            printf("Enter the .cod filename: ");
            scanf("%s", codFilename);
            printf("Enter the output filename: ");
            scanf("%s", outputFilename);
            decompressFile(comFilename, codFilename, outputFilename);
            printf("File decompressed successfully. Output written to %s\n", outputFilename);
            break;
        }
        case 4:
            exit(0);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

