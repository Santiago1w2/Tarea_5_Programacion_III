#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
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
        string data_to_hash = to_string(index)
                            + previous_hash
                            + to_string(nonce);

        for (const auto& voto : votos) {
            data_to_hash += voto.voter_id + voto.option;
        }

        hash<string> hasher;
        size_t hashed_value = hasher(data_to_hash);

        stringstream ss;
        ss << hex << setw(16) << setfill('0') << hashed_value;

        return ss.str();
    }
    void mineBlock(int dificultad) {
        string objetivo;
        for (int i = 0; i < dificultad; i++) {
            objetivo+= "0";
        }
        while (current_hash.substr(0,dificultad)!= objetivo) {
            ++nonce;
            current_hash=calculateHash();
        }
        cout << "Block minado " << endl;
    }
};

class Blockchain {
    vector<Block> blocks;
public:
    Blockchain() {
        vector<Voto> v;
        Block p_bock(0,"0", v);
        blocks.push_back(p_bock);
    }


    Block crearBlock(vector<Voto> votos) {
        Block new_block(blocks.size(),blocks.back().current_hash,votos);
        new_block.mineBlock(4);
        blocks.push_back(new_block);
        return new_block;
    }

    bool agregarBlock(Block block_) {
        if (block_.previous_hash == blocks.back().current_hash && block_.current_hash == block_.calculateHash()) {
            blocks.push_back(block_);
            return true;
        }
        return false;
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

    void mostrar() const {
        for (auto block : blocks) {
            cout << "----------------Block " << block.index + 1 <<"-----------" << endl;
            cout << "#Current_Hash: " << block.current_hash << endl;
            cout << "#Votos: " << block.current_hash << endl;
            for (auto v:block.votos) {
                cout << "- " << v.voter_id << " : "<<v.option <<"." << endl;
            }
            cout << "#Previus_hash: " << block.previous_hash << endl;
            cout << "------------------------------------------------------" << endl;
        }
    }
};

class MesaElectoralObserver {
public:
    virtual void update(const Block& block_) = 0;
    virtual void mostrar_mesa()= 0;
};

class MesaElectoral: public MesaElectoralObserver {
private:
    Blockchain blockchain_;
    string nombre;
public:
        MesaElectoral(string nom): nombre(nom) {
        }
    void update(const Block& block_) override {
            if (blockchain_.agregarBlock(block_)) {
                cout << nombre <<": se agrego correcatmente el bolock " << block_.index << ", todo correcto :)"<< endl;
            } else {
                cout << nombre <<": no se agrego correcatmente el bolock " << block_.index << ", algo fue modificado!" <<endl;
            }
    }
    void mostrar_mesa() override {
            cout << "============================= Mesa: " << nombre << "================" << endl;
            blockchain_.mostrar();
        }
    Block registrarVotos(vector<Voto> votos){
            cout << nombre<< " crea el Block y agrega el Block, ";
            return blockchain_.crearBlock(votos);
        }
};

class CentroElectoralSubject {
private:
    vector<MesaElectoralObserver* > observadores;
public:

    void attach(MesaElectoralObserver* mesa) {
        observadores.push_back(mesa);
    }
    void notificarNuevoBloque(const Block& block_, MesaElectoralObserver* origen) {
        cout << "Notificados------------------------------------------------ " << endl;

        for (auto& obs : observadores) {
            if (obs != origen) {
                obs->update(block_);
            }
        }
        cout << "--------------------------------------------------------" << endl;
    }

    void mostrar_central() {
        cout << "------------------------------Centro Electoral------------------------------" << endl;
        for (auto op: observadores) {
            op->mostrar_mesa();
        }
    }
};


int main() {
    vector<Voto> votos1;
    vector<Voto> votos2;
    Voto voto1("123", "Keiko");
    Voto voto2("234", "JP");
    Voto voto3("456", "Keiko");
    Voto voto4("678", "JP");
    votos1.push_back(voto1);
    votos1.push_back(voto2);
    votos2.push_back(voto3);
    votos2.push_back(voto4);
    auto* Centro1 = new CentroElectoralSubject();
    auto* Mesa1 = new MesaElectoral("Mesa1");
    auto* Mesa2 = new MesaElectoral("Mesa2");
    auto* Mesa3 = new MesaElectoral("Mesa3");
    Centro1->attach(Mesa1);
    Centro1->attach(Mesa2);
    Centro1->attach(Mesa3);
    Block b1 = Mesa1->registrarVotos(votos1);

    Centro1->notificarNuevoBloque(b1, Mesa1);

    Block b2 = Mesa1->registrarVotos(votos2);

    //b2.votos[0].option="MORALES";

    Centro1->notificarNuevoBloque(b2, Mesa1);

    Centro1->mostrar_central();
    delete Mesa1;
    delete Mesa2;
    delete Mesa3;
    delete Centro1;
    return 0;
}