/*
Christina Shafer
CIS 29
Fall 2017
Lab 3 - Huffman Tree Encryption/Decryption */

#include <map> //map
#include <queue> //priority_queue
#include <iostream>
#include <string>
#include <fstream>//file input/output
#include <vector>
#include <algorithm> //sort

using namespace std;

struct CharCount{
public:
    char key;
    int value;
public:
    CharCount() {key=-1; value=0;}
    char getKey() {return key;}
    int getValue() {return value;}
    void setKey(char c) {key=c;}
    void setValue(int v) {value=v;}

};
//function to sort by value in key/value pair
bool sortbysec(const CharCount &a,
              const CharCount &b)
{
    return (a.value < b.value);
}

class FrequencyTable {
public:
    map<char, int> FMap;
    vector<CharCount> FVector;
public:
    FrequencyTable(){
        string filename="Speech.txt";
        ReadFile(filename);
        MapToVector();
        SortVector();
       // Output();
    };
    FrequencyTable(string filename) {
        ReadFile(filename);
        MapToVector();
        SortVector();
       // Output();
    };
    void ReadFile(string FilenameString){
        char tempChar;
        ifstream inputFile(FilenameString);
        if (!inputFile){
            cerr << "File Failure. \""<<FilenameString<<"\" file not found";
            return;
        }
        else
        inputFile.open(FilenameString);
        do {
            inputFile.get(tempChar);
            FMap[tempChar]=0;
        }
        while (inputFile.peek() != EOF);
            inputFile.close();

        inputFile.open(FilenameString);
        do
        {
            inputFile.get(tempChar);
            int& number = FMap[tempChar];
            FMap[tempChar]++;
        } while (!inputFile.eof());
        inputFile.close();
    };
    void MapToVector(){
        for (auto p : FMap)
        {
            if (p.second){
                CharCount combo;
                combo.setKey(p.first);
                combo.setValue(p.second);
                FVector.push_back(combo);
            }
        }
    };
    void SortVector(){
        sort(FVector.begin(), FVector.end(), sortbysec);
    };

    void Output(){
        ofstream outfile("Frequency.txt");
        for (auto p : FVector)
        {
            if (p.value)
            {
                outfile << p.getKey() << "\t" << p.getValue() << endl;
                cout <<"\""<< p.getKey() <<"\""<< "\t" << p.getValue() << endl;
            }
        }
        outfile.close();
    };
};
template <typename T>
class TreeNode {
public:
	T item;
	TreeNode<T> *left;
	TreeNode<T> *right;
	//constructor
	TreeNode(T t) { item = t;left=NULL;right=NULL;}
};

class CompareNodeCharCount
{
    public:
    bool operator () (TreeNode<CharCount>* const &p1, TreeNode<CharCount>* const &p2)
    {
        return p1->item.value > p2->item.value;
    }
};

class HuffmanTree {
public:
    TreeNode<CharCount> *root;
    FrequencyTable Frequencies;
    priority_queue<TreeNode<CharCount>*, vector<TreeNode<CharCount>*>, CompareNodeCharCount> forest;
    CompareNodeCharCount cmp;

public:
	//constructor
	HuffmanTree() {
	    root=NULL;
	    buildForest();
	    buildTree();
	}
    void buildForest(){
        for (int i=0; i<Frequencies.FVector.size(); i++){
            TreeNode<CharCount> *newNode;
            newNode = new TreeNode<CharCount>(Frequencies.FVector[i]);
            forest.push(newNode);
        }
    }
    void buildTree(){
        while (forest.size()>1)
        {
            TreeNode<CharCount>* ptr1 = forest.top();
            forest.pop();
            TreeNode<CharCount>* ptr2 = forest.top();
            forest.pop();
            int tempInt = ptr1->item.getValue() + ptr2->item.getValue();
            CharCount NewCount;
            NewCount.setValue(tempInt);
            TreeNode<CharCount> *newNode;
            newNode = new TreeNode<CharCount>(NewCount);
            newNode->left=ptr1;
            newNode->right=ptr2;
            forest.push(newNode);
        }
        root=forest.top();
    }
};
class HuffmanEncryptor {
public:
    ifstream inputFile;
    ofstream outputFile;
    HuffmanTree Huff;
    map<char, string> EncryptPairs;
public:
    HuffmanEncryptor() {
       BuildEncryptedNodesMap(Huff.root,"");
    }

    void Encrypt(string INfilename, string OUTfilename) {
        inputFile.open(INfilename);
        outputFile.open(OUTfilename);
        string tempString;
        char tempEntry;

        if (!inputFile){
            cerr << "File Failure. \""<<INfilename<<"\" file not found.";
        }
        else {
            while (inputFile  ) {
                inputFile.get(tempEntry);
                tempString=EncryptPairs[tempEntry];
                outputFile<<tempString;
                }
            }
        inputFile.close();
        outputFile.close();
    }

    void BuildEncryptedNodesMap(TreeNode<CharCount>* node, string s){
        if (node->left) {
                BuildEncryptedNodesMap(node->left,s+"0");
                BuildEncryptedNodesMap(node->right,s + "1");
        }
        if (!node->left) {
            //cout<<"\""<<node->item.getKey()<<"\" : "<<s<<endl;
            EncryptPairs[node->item.getKey()]=s;
        }
    }
    void outputPairs(){
    for (auto p : EncryptPairs)
        cout<<p.first<<": "<<p.second<<endl;
    }

};

class HuffmanDecryptor {
public:
    ifstream inputFile;
    ofstream outputFile;
    HuffmanTree Huff;
    TreeNode<CharCount>* currentNode;
HuffmanDecryptor(){currentNode=Huff.root;}

void Decrypt(string INfilename, string OUTfilename){
    inputFile.open(INfilename);
    outputFile.open(OUTfilename);
    char tempChar;

    if (!inputFile){
		cerr << "File Failure. \""<<INfilename<<"\" file not found";
		return;
	}
	else
    do {
        inputFile.get(tempChar);
        //if the currentNode is not the leaf
        if (currentNode->left) {

            if (tempChar =='0'){
                currentNode=currentNode->left;
            }
            else
                currentNode=currentNode->right;
            }
        // if the currentNode is the leaf
        if (!currentNode->left)
            {
            outputFile<<currentNode->item.getKey();
            currentNode=Huff.root;
            }
        }
	while (inputFile.peek() != EOF);
		inputFile.close();
		outputFile.close();
}
};

int main() {
HuffmanEncryptor ENC;
HuffmanDecryptor DEC;
cout<<"Encrypting \"Speech.txt\" using Huffman Coding. \nOutput file is \"SpeechEncrypted.bin\".\n\n";
ENC.Encrypt("Speech.txt", "SpeechEncrypted.bin");
cout<<"Decrypting \"SpeechEncrypted.bin\" using Huffman Coding. \nOutput file is \"SpeechDecrypted.txt\".\n";
DEC.Decrypt("SpeechEncrypted.bin", "SpeechDecrypted.txt");
}
