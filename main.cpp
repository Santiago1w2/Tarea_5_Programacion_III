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

int main() {
//hola santi saodosajdoisjaoij come keke
    //no me digas vago p me pongo tite
    cout << "Hello World!" << endl;
    return 0;
}