#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

struct Voto{
    string voter_id;
    string option;
    Voto(const string& id, const string& opt) : voter_id(id), option(opt) {}
};

struct Block {
    int index;
    string previous_hash;
    vector<Voto> votos;
    int nonce;
    string current_hash;

    Block(int idx, const string& prev_hash, const vector<Voto>& lista_votos)
        : index(idx), previous_hash(prev_hash), votos(lista_votos), nonce(0) {
        current_hash = calculateHash();
    }

    string calculateHash() const {
        // Uso de stringstream para simular una huella segura y evitar colisiones
        stringstream ss_data;
        ss_data << index << previous_hash << nonce;

        for (const auto& voto : votos) {
            ss_data << "|" << voto.voter_id << ":" << voto.option;
        }

        string base_data = ss_data.str();
        stringstream hash_result;
        hash<string> hasher;

        // Simulamos un hash más largo (64 caracteres) concatenando pasadas de std::hash
        for (int i = 0; i < 4; ++i) {
            string data_chunk = base_data + to_string(i);
            size_t hashed_chunk = hasher(data_chunk);
            hash_result << hex << setw(16) << setfill('0') << hashed_chunk;
        }

        return hash_result.str();
    }
};

class Blockchain {
    vector<Block> blocks;
public:
    Blockchain() {
        Voto vs("GENESIS", "GENESIS");
        vector<Voto> v = {vs};
        Block p_bock(0,"0", v);
        blocks.push_back(p_bock);
    }

    void mineBlock(int dificultad, Block& blockToMine) {
        string objetivo(dificultad, '0');
        while (blockToMine.current_hash.compare(0, dificultad, objetivo) != 0) {
            blockToMine.nonce++;
            blockToMine.current_hash = blockToMine.calculateHash();
        }
        cout << "Block minado: " << blockToMine.index << endl;
    }

    Block crearBlock(const vector<Voto>& votos) {
        Block new_block(blocks.size(), blocks.back().current_hash, votos);

        mineBlock(4, new_block);

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
    bool isChainValid() const {
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
        for (const auto& block : blocks){
            cout << "----------------Block " << block.index + 1 <<"-----------" << endl;
            cout << "#Current_Hash: " << block.current_hash << endl;
            cout << "#Votos: " << endl;
            for (const auto& v : block.votos) {
                cout << "- " << v.voter_id << " : "<<v.option <<"." << endl;
            }
            cout << "#Previus_hash: " << block.previous_hash << endl;
            cout << "------------------------------------------------------" << endl;
        }
    }
};

class MesaElectoralObserver {
public:
    virtual ~MesaElectoralObserver() = default;
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
                cout << nombre <<": se agrego correcatmente el Bolock " << block_.index << ", todo correcto :)"<< endl;
            } else {
                cout << nombre <<": no se agrego correcatmente el Block " << block_.index << ", algo fue modificado en el BLock!" << endl;
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
    void auditarMesa() const {
            cout << "\n--- AUDITORIA DE " << nombre << " ---" << endl;
            if (blockchain_.isChainValid()) {
                cout << "[ESTADO: SEGURO] Los hashes coinciden. Ningun voto historico ha sido alterado." << endl;
            } else {
                cout << "[ESTADO: PELIGRO] ¡Se detecto corrupcion en los datos! La cadena esta ROTA." << endl;
            }
            cout << "---------------------------------\n" << endl;
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
    vector<Voto> votos1 = {
        {"123", "Keiko"}, {"234", "JP"}, {"345", "Keiko"}, {"456", "JP"},
        {"567", "Keiko"}, {"678", "JP"}, {"789", "Keiko"}, {"901", "Keiko"}, {"102", "JP"}
    };

    vector<Voto> votos2 = {
        {"113", "Keiko"}, {"124", "JP"}, {"135", "Keiko"}, {"146", "JP"},
        {"157", "Keiko"}, {"168", "JP"}, {"179", "Keiko"}, {"180", "JP"},
        {"191", "Keiko"}, {"202", "JP"}
    };

    vector<Voto> votos3 = {
        {"213", "Keiko"}, {"224", "JP"}, {"235", "Keiko"}, {"246", "JP"},
        {"257", "Keiko"}, {"268", "JP"}, {"279", "Keiko"}, {"280", "JP"},
        {"291", "Keiko"}, {"302", "JP"}
    };

    auto* Centro1 = new CentroElectoralSubject();
    auto* Mesa1 = new MesaElectoral("Mesa1");
    auto* Mesa2 = new MesaElectoral("Mesa2");
    auto* Mesa3 = new MesaElectoral("Mesa3");

    Centro1->attach(Mesa1);
    Centro1->attach(Mesa2);
    Centro1->attach(Mesa3);

    Block b1 = Mesa1->registrarVotos(votos1);
    Centro1->notificarNuevoBloque(b1, Mesa1);
    Mesa1->auditarMesa();

    Block b2 = Mesa2->registrarVotos(votos2);
    Centro1->notificarNuevoBloque(b2, Mesa2);
    Mesa2->auditarMesa();

    Block b3 = Mesa3->registrarVotos(votos3);
    b3.votos[0].option = "JP"; // Intento de fraude
    Centro1->notificarNuevoBloque(b3, Mesa3);
    Mesa3->auditarMesa();


    Centro1->mostrar_central();

    delete Mesa1;
    delete Mesa2;
    delete Mesa3;
    delete Centro1;

    return 0;
}