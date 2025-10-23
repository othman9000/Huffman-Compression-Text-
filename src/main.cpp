#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<chrono>
using namespace std;
//~5 us per one byte of the text file to be compressed + constant
//~1.1 us per one byte of the compressed file to be decompressed + constant
struct Node {
    unsigned int freq;
    unsigned int key;
    vector<bool> hCode;
    Node* left;
    Node* right;
    Node(unsigned int f = 0, unsigned int keyIN = 0xFF) {
        freq = f;
        key = keyIN;
        left = nullptr;
        right = nullptr;
    }
};

class SinglyLinkedList {//this class doesn't owe nodes
    struct listNode {
        Node* data;
        listNode* nxt;
        listNode(Node* n) : data(n), nxt(nullptr) {

        }
    };
    listNode* head;
    listNode* tail;
public:
    SinglyLinkedList() {
        head = nullptr;
        tail = nullptr;
    }
    Node* search(const unsigned int& key) {//O(n)
        listNode* result = head;
        while (result != nullptr && result->data->key != key) {
            result = result->nxt;
        }
        if (result == nullptr)
            return nullptr;
        return result->data;
    }
    void display(void) {
        listNode* d = head;
        while (d != nullptr) {
            cout << d->data->key << endl;
            d = d->nxt;
        }
    }
    void addNode(Node* n) {
        listNode* ptr = new listNode(n);
        if (tail != nullptr)
            tail->nxt = ptr;
        else
            head = ptr;
        tail = ptr;
    }
    ~SinglyLinkedList() {
        listNode* ptr = head;
        listNode* temp;
        while (ptr != nullptr) {
            temp = ptr->nxt;
            delete ptr;
            ptr = temp;
        }
    }
};
class heap {      // priority queue
    int size;     //logical size
    int capacity;
    Node** arr;
    inline int left(int i) {
        return 2 * i + 1;
    }
    inline int right(int i) {
        return 2 * i + 2;
    }
    inline int parent(int i) {//send the index
        if (i != 0)
            return (i - 1) / 2;
        return 0;
    }
    void Insert(Node* val) {//amortized O(1)
        if (size == capacity) {
            capacity *= 2;
            Node** temp = arr;
            arr = new Node * [capacity];
            for (int i = 0; i < size; ++i) {
                arr[i] = temp[i];
            }
            delete[]temp;
        }
        arr[size++] = val;
    }
public:
    heap(int cap = 2) : capacity(cap) {
        size = 0;
        arr = new Node * [capacity];
    }
    void minHeapify(const int& index) {// O(log(n)) , O(h)
        int l = left(index);
        int r = right(index);
        int smallest = index;
        if (l < size && arr[l]->freq < arr[index]->freq)
            smallest = l;
        if (r < size && arr[r]->freq < arr[smallest]->freq)
            smallest = r;
        if (smallest != index) {
            swap(arr[index], arr[smallest]);
            minHeapify(smallest);
        }
        return;
    }
    void BuildMinHeap(void) {//O(n)
        int n = size / 2;//floor
        n--; //0 origin indexing, n repersenting non-leaf elements
        for (int i = n; i >= 0; i--) {
            minHeapify(i);
        }
    }
    //sorting operation
    void HeapSort(void) {//O(nlog(n))
        BuildMinHeap();
        int temp = size;
        for (int i = size - 1; i >= 0; --i) {
            swap(arr[i], arr[0]);
            size--;
            minHeapify(0);
        }
        size = temp;
        return;
    }
    ~heap() {
        delete[] arr;
    }
    void displayArr() {
        for (int i = 0; i < size; ++i)
            cout << arr[i]->freq << endl;
        cout << "size = " << size << endl;
    }
    //priority queue operations
    Node* extractMin(void) {//log(n)
        if (size != 0) {
            Node* temp = arr[0];
            arr[0] = arr[--size];
            minHeapify(0);
            return temp;
        }
        cout << "ERROR : TRYING TO EXTRACT MIN WITH SIZE 0 OF THE HEAP" << endl;
        return nullptr;
    }
    Node* peekMin(void) {
        if (size != 0)
            return arr[0];
        cout << "ERROR : TRYING TO PEEK MIN WITH SIZE 0 OF THE HEAP" << endl;
        return nullptr;
    }
    void InsertElmPriorityQueue(Node* elm) {//O(log(n))
        Insert(elm);
        int index = size - 1;
        Node* temp1 = arr[parent(index)]; //parent's
        Node* temp2 = arr[index]; //child's
        while (temp2->freq < temp1->freq) {
            arr[parent(index)] = arr[index];
            arr[index] = temp1;
            index = parent(index);
            temp1 = arr[parent(index)];
        }
    }
    int getSize(void) {
        return size;
    }
};
class HashTable {
private:
    int m;//number of slots in the array
    SinglyLinkedList* arr;
    unsigned int hash(unsigned int key) {//O(1)
        key ^= (key >> 16);
        key *= 0x85ebca6b;
        key ^= (key >> 13);
        key *= 0xc2b2ae35;
        key ^= (key >> 16);
        return key % m;
    }
public:
    HashTable(int mIN) : m(mIN) {//send the size of the hash table 
        arr = new SinglyLinkedList[m];
    }
    Node* seacrh(unsigned int key) {
        //O(1) average case if the number of elemetns in the has ~= m
        //O(n) worst-case scenario ,very unlikely
        Node* temp = arr[hash(key)].search(key);
        return temp;
    }
    void addNode(Node* n) {
        unsigned int k = n->key;
        arr[hash(k)].addNode(n);
    }
    ~HashTable() {
        delete[] arr;
    }
};

unsigned int convertIntoUTF8UINT(ifstream& f) {
    //0xxxxxxx for one byte
    //110xxxxx 10xxxxxx for two bytes
    //1110xxxx 10xxxxxx 10xxxxxx for three bytes
    //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx for four bytes
    unsigned char byte;
    unsigned int result;
    f.get(reinterpret_cast<char&>(byte));
    result = static_cast<unsigned int>(byte);
    if ((byte & 0x80) == 0) {
        return result;
    }
    else {
        int num_of_bytes = 0;
        if ((byte & 0xE0) == 0xC0) {
            num_of_bytes = 1;
        }
        else if ((byte & 0xF0) == 0xE0) {
            num_of_bytes = 2;
        }
        else if ((byte & 0xF8) == 0xF0) {
            num_of_bytes = 3;
        }
        else {
            cout << "fatal error: there is a problem in the file's data " << endl;
            return 0;
        }
        for (int i = 1; i <= num_of_bytes; ++i) {
            f.get(reinterpret_cast<char&>(byte));
            if ((byte & 0xC0) == 0x80) {
                result = result << 8;
                result += byte;
            }
            else {
                cout << "fatal error: there is a problem in the file data " << endl;
                break;
            }
        }
        return result;
    }
}

void readFileIntoIntVectorOfUTF8(vector<unsigned int>& theFileInBytes, const string& dir, vector<Node*>& nodes, HashTable& table) {//O(n)
    ifstream file(dir);
    if (!file.is_open()) {
        cout << "ERROR : the file is not opened" << endl;
        return;
    }
    unsigned int temp = 0;
    //uintmax_t sizeOfFile = filesystem::file_size(dir);
    //v.resize(sizeOfFile); don't really need them, going to complicate things 
    while (file.peek() != EOF) {//O(n) where n is the size of the file
        temp = convertIntoUTF8UINT(file);
        theFileInBytes.push_back(temp);
        Node* tempNode = table.seacrh(temp);//O(1)
        if (tempNode != nullptr) {
            tempNode->freq++;
        }
        else {
            Node* n = new Node(1, temp);
            nodes.push_back(n);
            table.addNode(n);
        }
    }
    file.close();
}
Node* createHuffTree(vector<Node*>& Nodes) {//O(nlog(n))
    //returns the root of the huff tree 
    heap prioQ;
    for (Node* temp : Nodes) {//O(nlog(n)) where n = number of nodes
        prioQ.InsertElmPriorityQueue(temp);
    }
    int Qsize = prioQ.getSize();
    Qsize--;
    for (int i = 0; i < Qsize; ++i) {//O(nlog(n))
        Node* n1 = prioQ.extractMin();
        Node* n2 = prioQ.extractMin();
        Node* n3 = new Node();
        n3->left = n1;
        n3->right = n2;
        n3->freq = n1->freq + n2->freq;
        Nodes.push_back(n3);
        prioQ.InsertElmPriorityQueue(n3);
    }
    return Nodes.back();
}
void displayTree(Node* tree) {//dfs to display the tree O(n)
    if (tree->right != nullptr) {
        displayTree(tree->right);
    }
    if (tree->left != nullptr) {
        displayTree(tree->left);
    }
    cout << "key : " << tree->key << ", freq : " << tree->freq << endl;
    if (tree->left == nullptr) {//O(1) for simplicity
        vector<bool> tempCode;
        cout << "huff code = ";
        tempCode = tree->hCode;
        for (bool b : tempCode) {
            cout << (b ? '1' : '0');
        }
        cout << endl;
    }
    cout << endl;
    return;
}
void displayNodes(vector<Node*>& Nodes) {
    for (Node* temp : Nodes) {
        cout << "key : " << temp->key << ", freq : " << temp->freq << endl;
        if (temp->left == nullptr) {//O(1) for simplicity
            vector<bool> tempCode;
            cout << "huff code = ";
            tempCode = temp->hCode;
            for (bool b : tempCode) {
                cout << (b ? '1' : '0');
            }
            cout << endl;
        }
        cout << endl;
    }
    return;
}
void createHuffCodes(Node* tree, vector<bool>& path) {//adds huff codes into the nodes ,dfs O(n)
    if (tree->right != nullptr) {
        path.push_back(true);
        createHuffCodes(tree->right, path);
    }
    if (tree->left != nullptr) {
        path.push_back(false);
        createHuffCodes(tree->left, path);
    }
    if (tree->left == nullptr) // since this is a full binary tree, we only check left 
        tree->hCode = path;
    if (!path.empty())
        path.pop_back();
    return;
}
void writeUTF8Int(ofstream& outFile, unsigned int key) {
    unsigned char put = 0;
    for (int i = 0; i < 3; ++i) {
        if ((key & 0x80000000) != 0) {
            put = static_cast<unsigned char>(key >> 24);
            outFile.put(put);
        }
        key = key << 8;
    }
    put = static_cast<unsigned char>(key >> 24);
    outFile.put(put);
}
void createHuffCodedFile(vector<unsigned int>& fileInIntsUTF8, HashTable& table, vector<Node*>& Nodes, const string& name) {//O(n), where n = characters * avg size of huff codes
    /*
        encoding as follow:
        first byte = the number of bits that are not part of the huff code at the last byte, as we have to add
        bits to fill up the last byte if the encoded huff file doesn't fill up exact number of bytes
        then we ecnode the UTF8-to-frequency table as follow:
        UTF-8       Frequency
        1-4bytes    4 bytes
        which will be used to reconstruct the huffman tree
        reaching a UTF-8 character of 0xFF means we reached the end of the table
        (as there is no UTF-8 character of 0xff value)
        then we encode the text file into huff codes
    */
    string outFileName = name + ".huffcoded";
    ofstream outFile(outFileName, ios::binary);
    vector<bool> hCode;
    if (outFile.is_open()) {
        unsigned char byte = 0;
        unsigned int bitCount = 0;
        outFile.put(0); //placeholder for the number of bits that are not part of the huff code in the last byte
        for (Node* temp : Nodes) {//writing the table :- 
            unsigned int freq;
            if (temp->left != nullptr)
                break;//reaching none leaf elements
            writeUTF8Int(outFile, temp->key);
            hCode = temp->hCode;
            freq = temp->freq;
            for (int i = 0; i < 4; ++i) {
                byte = static_cast<unsigned char>(freq >> ((3 - i) * 8));
                outFile.put(byte);
            }
        }
        outFile.put(0xff);
        byte = 0;
        for (unsigned int i : fileInIntsUTF8) {  //O(n) for each bit of the huffman codes
            Node* temp = table.seacrh(i);        //O(1)
            hCode = temp->hCode;// O(1)
            for (bool b : hCode) {
                if (b)
                    byte |= (0x80 >> bitCount);
                bitCount++;
                if (bitCount == 8) {
                    bitCount = 0;
                    outFile.put(byte);
                    byte = 0;
                }
            }
        }
        if (bitCount) {
            outFile.put(byte);
            outFile.seekp(0, ios::beg);
            outFile.put(8 - bitCount);
        }
        outFile.close();
    }
    else {
        cout << "fatal error : couldn't have written a file :( " << endl;
    }
}
void decodeHuffCodedFileIntoTXT(string& dir, string& name) {
    unsigned char ignoreBits = 0;
    unsigned char bitsToRead = 8;
    unsigned char byte;
    ifstream inFile(dir, ios::binary);
    unsigned int UTF8;
    unsigned int freq;
    vector<Node*> Nodes;
    if (inFile.is_open()) {
        ofstream outFile(name + ".txt");
        inFile.get(reinterpret_cast<char&>(byte));
        ignoreBits = byte;
        byte = 0;
        while (inFile.peek() != 0xff) {//O(1) for each iteration,O(n) where n ~= bytes of the tree table
            UTF8 = convertIntoUTF8UINT(inFile);
            freq = 0;
            for (int i = 0; i < 4; ++i) {
                inFile.get(reinterpret_cast<char&>(byte));
                freq += byte << (3 - i) * 8;
            }
            Node* temp = new Node(freq, UTF8);
            Nodes.push_back(temp);
        }
        Node* tree = createHuffTree(Nodes);//O(nlog(n)),where n = number of nodes
        Node* huff = tree;
        //cout << "the tree after reconstruction : " << endl;
        //displayNodes(Nodes);
        inFile.get(reinterpret_cast<char&>(byte));
        while (inFile.get(reinterpret_cast<char&>(byte))) {//O(1) for each iteration,O(n) where n ~= bytes of the actual encoded text
            if (inFile.peek() == EOF) //reading the last byte
                bitsToRead = 8 - ignoreBits;
            unsigned char bitMask = 0x80;
            for (int i = 0; i < bitsToRead; ++i) {
                if ((byte & bitMask) != 0) {
                    huff = huff->right;
                }
                else {
                    huff = huff->left;
                }
                bitMask = bitMask >> 1;
                if (huff->left == nullptr) {
                    UTF8 = huff->key;
                    huff = tree;
                    writeUTF8Int(outFile, UTF8);
                }
            }
        }
        inFile.close();
        outFile.close();
        for (Node* temp : Nodes) {
            delete temp;
        }
    }
    else {
        cout << "ERROR : cannot open the file that you sent its dierctory!" << endl;
    }
}
int main() {
    //great ! I've done the priority queue :D 2/13/2025
    cout << "please enter 1 for compressing,2 decompressing : " << endl;
    short choice;
    cin >> choice;
    switch (choice) {
    case 1: {
        cin.ignore();
        vector<Node*> Nodes;
        cout << "enter the directory " << endl;
        string dir;
        getline(cin, dir);
        cout << "enter the name of the file you want to create :" << endl;
        string name;
        getline(cin, name);
        auto start = chrono::high_resolution_clock::now();
        vector<unsigned int> fileInIntsUTF8;
        HashTable NodesTable(503);
        readFileIntoIntVectorOfUTF8(fileInIntsUTF8, dir, Nodes, NodesTable);//O(n)
        /*for (unsigned int i : fileInIntsUTF8) {
            cout << i << ' ';
        }*/
        cout << endl;
        //great I've done the reading of the text file and conversion into UTF-8 2/27/2025
        Node* result;
        result = createHuffTree(Nodes);//O(n) where n = number of nodes
        vector<bool> path;
        createHuffCodes(result, path);
        //displayNodes(Nodes);
        createHuffCodedFile(fileInIntsUTF8, NodesTable, Nodes, name);
        for (Node* temp : Nodes) {
            delete temp;
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "time taken for compression = " << duration.count() << "seconds" << endl;
        break;
    }
    case 2: {
        cin.ignore();
        cout << "please enter the directory of the file you want to decompress" << endl;
        string dir;
        getline(cin, dir);
        cout << "please enter the name of the text file you want to create" << endl;
        string name;
        getline(cin, name);
        auto start = chrono::high_resolution_clock::now();
        decodeHuffCodedFileIntoTXT(dir, name);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "time taken for decompression = " << duration.count() << "seconds" << endl;
        break;
    }
    default:
        cout << "wrong choice " << endl;
    }
    return 0;
}
