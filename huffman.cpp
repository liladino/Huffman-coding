#include <bits/stdc++.h>
using namespace std;

class CodeWord{
private:
	vector<bool> word;
public:
	CodeWord(){};
	CodeWord(vector<bool>& word) : word(word){}
	CodeWord(const CodeWord& c){
		word = c.word;
	}
	CodeWord(bool b) {
		append(b);
	}
	
	CodeWord& operator=(const CodeWord& other){
		word = other.word;
		return *this;
	}
	
	void append(bool b){
		word.push_back(b);
	}
	
	size_t size(){
		return word.size();
	}
	
	const vector<bool>& getWord(){
		return word;
	}
	
	bool operator<(const CodeWord& other) const{
		if (word.size() != other.word.size()) return word.size() < other.word.size();
		for (size_t i = 0; i < min(word.size(), other.word.size()); i++){
			if ((int)word[i] < (int)other.word[i]){
				return false;
			}
			else if ((int)word[i] > (int)other.word[i]){
				return true;
			}
		}
		return false;
	}
	
	friend ostream& operator<<(ostream& os, const CodeWord& c);
};

ostream& operator<<(ostream& os, const CodeWord& c){
	for (size_t i = 0; i < c.word.size(); i++){
		os << (int)(c.word[i]);
	}
	return os;
}

struct Node{
	unique_ptr<Node> leftchild;
	unique_ptr<Node> rightchild;
	char character;
	int freq;
	Node(char c, int p): character(c), freq(p){
		rightchild = nullptr;
		leftchild = nullptr;
	}
	Node(unique_ptr<Node>& l, unique_ptr<Node>& r): character(0){
		freq = l->freq + r->freq;
		rightchild = move(r);
		leftchild = move(l);
	}
	bool isLeaf(){
		if (!rightchild) return true;
		return false;
	}
};

struct NodeUPtrCmp{
	bool operator()(const std::unique_ptr<Node>& lhs, const std::unique_ptr<Node>& rhs){
		return lhs->freq > rhs->freq;
	}
};

void setNumberOfChars(const string& s, vector<pair<char, int>>& v){
	int freqOfChars[256] = {0};
	for (size_t i = 0; i < s.size(); i++){
		unsigned char c = s[i];
		freqOfChars[(size_t)c]++;
	}
	for (int i = 0; i < 256; i++){
		if (freqOfChars[i]) {
			v.push_back(std::make_pair(i, freqOfChars[i]));
		}
	}
}

unique_ptr<Node> buildHuffmanTree(vector<pair<char, int>>& v){
	priority_queue<std::unique_ptr<Node>, vector<std::unique_ptr<Node>>, NodeUPtrCmp> p;
	for (const auto& x : v){
		p.push(make_unique<Node>(x.first, x.second));
	}
	
	while (p.size() > 1){
		unique_ptr<Node> temp1 = move((unique_ptr<Node>&)p.top());
        p.pop();
		unique_ptr<Node> temp2 = move((unique_ptr<Node>&)p.top());
        p.pop();
		
		p.push(make_unique<Node>(temp1, temp2));
	}
	
	return move((unique_ptr<Node>&)p.top());
}

void traverseHuffmanTree(map<char, CodeWord>& encodeTable, map<CodeWord, char>& decodeTable, unique_ptr<Node>& root, CodeWord c){
	if (root->isLeaf()){
		encodeTable[root->character] = c;
		decodeTable[c] = root->character;
		return;
	}
	CodeWord left(c);
	CodeWord right(c);
	left.append(0);
	right.append(1);
	
	traverseHuffmanTree(encodeTable, decodeTable, root->leftchild, left);
	traverseHuffmanTree(encodeTable, decodeTable, root->rightchild, right);
}

void makeCodeTables(map<char, CodeWord>& encodeTable, map<CodeWord, char>& decodeTable, unique_ptr<Node>& root){
	if (root->isLeaf()){
		encodeTable[root->character] = 0;
		decodeTable[0] = root->character;
		return;
	}
	CodeWord left(0);
	CodeWord right(1);
	
	traverseHuffmanTree(encodeTable, decodeTable, root->leftchild, left);
	traverseHuffmanTree(encodeTable, decodeTable, root->rightchild, right);
}

void encode(const string& s, map<char, CodeWord>& encodeTable, vector<bool>& result){
	for (char c : s){
		vector<bool> word = encodeTable[c].getWord();
		for (size_t i = 0; i < word.size(); i++){
			result.push_back(word[i]);
		}
	}
}

void decode(vector<bool>& encodedString, map<CodeWord, char>& decodeTable, string& s){
	if (encodedString.size() < 1) return;
	CodeWord temp;
	for (size_t i = 0; i < encodedString.size(); i++){
		temp.append(encodedString[i]);
		
		if (decodeTable.count(temp)){
			s += decodeTable[temp];
			temp = CodeWord();
		}
	}
}

int main() {
	string s;// = "Bus duledekeiden, Husztnak romvara megallek;\nCsend vala, felleg alol szallt fel az ejjeli hold.\nSzel kele most, mint sir szele kel; s a csarnok elontott\nOszlopi kozt lebego remalak inte felem.\nEs mond: Honfi, mit er epedo kebel e romok orman?\nRegi kor arnya fele visszamerengni mit er?\nMessze jovendovel komolyan vess ossze jelnkort;\nHass, alkoss, gyarapits; s a haza fenyre derul!";
	
	{
		char c;
		while (cin >> noskipws >> c){s += c;}
	}
	
	if (s.size() < 1) {
		cout << "Not text, nothing to compress" << endl;
		return 0;
	}

	cout << "Original text:\n\n" << s;
	
	vector<pair<char, int>> v;
	setNumberOfChars(s, v);

	unique_ptr<Node> root = buildHuffmanTree(v);
	
	map<char, CodeWord> encodeTable;
	map<CodeWord, char> decodeTable;
	makeCodeTables(encodeTable, decodeTable, root);
	
	cout << "\n\nchar    freq        codeword" << endl;
	
	double sum = 0;
	for_each(v.begin(), v.end(), [&sum](const pair<char, int>& p) {sum += p.second;});
	for (const auto& x : decodeTable){	
		cout << "'" << x.second << "'     ";
		cout << fixed << setprecision(6) << ((find_if(v.begin(), v.end(), [&x](const pair<char, int>& p) { return p.first == x.second;}))->second)/sum << "    " << x.first << endl;
	}
	
	vector<bool> enc;
	encode(s, encodeTable, enc);
	
	string s2;  
	decode(enc, decodeTable, s2);
	//~ cout << "Decoded:\n" << s2 << endl;
	
	if (s.compare(s2) != 0){
		cout << "Coding error";
		return 1;
	}
	
	cout << "Original size: " << s.size() * sizeof(char) * 8 << " bits, " << s.size() * sizeof(char) << " bytes." << endl;
	cout << "Encoded size:  " << enc.size() << " bits, " << (enc.size() + 7)/8 << " bytes." << endl;
	cout << ((enc.size() + 7)/8) / (s.size() * sizeof(char) * 0.01) << "% of the original size." << endl;
	
	
	return 0; 
}
