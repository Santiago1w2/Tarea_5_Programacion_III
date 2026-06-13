#include <iostream>
#include <vector>
using namespace std;

struct Voto{

    string voter_id;
    string option;


    Voto(string id, string opt) : voter_id(id), option(opt) {}
};

struct Block {
    int index;
    string previous_hash;
    vector<Voto> votos;
    int nonce;
    string current_hash;

    Block(int idx, string prev_hash, vector<Voto> lista_votos)
        : index(idx), previous_hash(prev_hash), votos(lista_votos), nonce(0) {
        current_hash = calculateHash();
    }

    string calculateHash() const {
        string data_to_hash = to_string(index) + previous_hash + to_string(nonce);

        for (const auto& voto : votos) {
            data_to_hash += voto.voter_id + voto.option;
        }

        hash<string> hasher;
        size_t hashed_value = hasher(data_to_hash);

        return to_string(hashed_value);
    }
};

class Blockchain {
    vector<Block> blocks;
public:
    void agregarBlock(const Block& block) {
        blocks.push_back(block);
    }

    void mineBlock(int dificultad) {
        if (blocks.empty()) {
            return;
        }
        Block& block = blocks.back();
        string objetivo;
        for (int i = 0; i < dificultad; i++) {
            objetivo += "0";
        }
        string hash_actual=block.calculateHash();
        while (hash_actual.substr(0,dificultad)!= objetivo) {
            block.nonce++;
            hash_actual=block.calculateHash();
        }
        block.current_hash=hash_actual;
    }

    bool isChainValid(){
        for (int i=1; i<blocks.size(); i++) {
            const Block& actual = blocks[i];
            const Block& anterior = blocks[i-1];
            if (actual.previous_hash!=anterior.current_hash) {
                return false;
            }
            if (actual.current_hash!=actual.calculateHash()) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    cout << "Hello World!" << endl;
    return 0;
}