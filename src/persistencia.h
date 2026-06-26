/**
 * @file persistencia.h
 * @brief Serialização e carregamento do estado do jogo via SQLite.
 */
#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "personagem.h"
#include "sqlite3.h"
#include <string>
#include <stdexcept>
using namespace std;

namespace RPG {

/**
 * @brief Gerencia persistência em banco SQLite.
 * Salva e carrega estado de personagens.
 */
class Persistencia {
    private:
        sqlite3*    db;
        string      caminhoDB;

        void executar(const string &sql) {
            char* erro = nullptr;
            if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &erro) != SQLITE_OK) {
                string msg = erro ? erro : "Erro desconhecido";
                sqlite3_free(erro);
                throw runtime_error("SQLite: " + msg);
            }
        }

    public:
        explicit Persistencia(const string &caminho = "rpg_save.db")
            : db(nullptr), caminhoDB(caminho) {
            if(sqlite3_open(caminho.c_str(), &db) != SQLITE_OK)
                throw runtime_error("Não foi possível abrir o banco: " + caminho);
            criarTabelas();
        }

        ~Persistencia() {
            if(db) sqlite3_close(db);
        }

        void criarTabelas() {
            executar(R"(
                CREATE TABLE IF NOT EXISTS personagens (
                    id        INTEGER PRIMARY KEY AUTOINCREMENT,
                    nome      TEXT NOT NULL,
                    classe    TEXT NOT NULL,
                    raca      TEXT NOT NULL,
                    nivel     INTEGER NOT NULL,
                    hpAtual   INTEGER NOT NULL,
                    hpMax     INTEGER NOT NULL,
                    forca     INTEGER NOT NULL,
                    dex       INTEGER NOT NULL,
                    intel     INTEGER NOT NULL,
                    fe        INTEGER NOT NULL,
                    exp       INTEGER NOT NULL,
                    estus     INTEGER NOT NULL
                );
            )");

            executar(R"(
                CREATE TABLE IF NOT EXISTS inventario (
                    id           INTEGER PRIMARY KEY AUTOINCREMENT,
                    personagem_id INTEGER NOT NULL,
                    nomeItem     TEXT NOT NULL,
                    tipoItem     TEXT NOT NULL,
                    equipado     INTEGER NOT NULL DEFAULT 0,
                    FOREIGN KEY(personagem_id) REFERENCES personagens(id)
                );
            )");
        }

        void salvar(const Personagem &p) {
            // remove save anterior do mesmo nome
            executar("DELETE FROM inventario WHERE personagem_id IN "
                     "(SELECT id FROM personagens WHERE nome='" + p.getNome() + "');");
            executar("DELETE FROM personagens WHERE nome='" + p.getNome() + "';");

            // insert personagem
            string sql = "INSERT INTO personagens "
                         "(nome, classe, raca, nivel, hpAtual, hpMax, forca, dex, intel, fe, exp, estus) VALUES ('"
                + p.getNome()          + "','"
                + p.getClasse()        + "','"
                + p.getRaca()->getNome() + "',"
                + to_string(p.getNivel())      + ","
                + to_string(p.getHPAtual())    + ","
                + to_string(p.getHPMax())      + ","
                + to_string(p.getForca())      + ","
                + to_string(p.getDex())        + ","
                + to_string(p.getInteligencia()) + ","
                + to_string(p.getFe())         + ","
                + to_string(p.getExp())        + ","
                + to_string(p.getEstusCargas()) + ");";
            executar(sql);

            // salva itens do inventário
            sqlite3_stmt* stmt;
            string qid = "SELECT id FROM personagens WHERE nome='" + p.getNome() + "';";
            sqlite3_prepare_v2(db, qid.c_str(), -1, &stmt, nullptr);
            int pid = -1;
            if(sqlite3_step(stmt) == SQLITE_ROW) pid = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);

            if(pid < 0) return;

            Arma* armaEq = p.getInventario().getArmaEquipada();
            for(Item* item : p.getInventario().getItens()) {
                int eq = (armaEq && armaEq->getNome() == item->getNome()) ? 1 : 0;
                string si = "INSERT INTO inventario (personagem_id, nomeItem, tipoItem, equipado) VALUES ("
                    + to_string(pid) + ",'"
                    + item->getNome() + "','"
                    + item->getTipo() + "',"
                    + to_string(eq) + ");";
                executar(si);
            }

            cout << "✔ Jogo salvo: " << p.getNome() << endl;
        }

        /**
         * @brief Carrega dados básicos de um personagem salvo.
         * @return true se encontrou o save.
         */
        bool carregarInfo(const string &nome,
                          string &outClasse, string &outRaca,
                          int &outNivel, int &outHP, int &outHPMax,
                          int &outForca, int &outDex, int &outExp) {
            string sql = "SELECT classe, raca, nivel, hpAtual, hpMax, forca, dex, exp "
                         "FROM personagens WHERE nome='" + nome + "' LIMIT 1;";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            bool found = false;
            if(sqlite3_step(stmt) == SQLITE_ROW) {
                outClasse  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                outRaca    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                outNivel   = sqlite3_column_int(stmt, 2);
                outHP      = sqlite3_column_int(stmt, 3);
                outHPMax   = sqlite3_column_int(stmt, 4);
                outForca   = sqlite3_column_int(stmt, 5);
                outDex     = sqlite3_column_int(stmt, 6);
                outExp     = sqlite3_column_int(stmt, 7);
                found = true;
            }
            sqlite3_finalize(stmt);
            return found;
        }

        void listarSaves() const {
            string sql = "SELECT nome, classe, raca, nivel FROM personagens;";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            cout << "\n=== SAVES DISPONÍVEIS ===\n";
            int i = 1;
            while(sqlite3_step(stmt) == SQLITE_ROW) {
                cout << i++ << ". "
                     << sqlite3_column_text(stmt, 0) << " | "
                     << sqlite3_column_text(stmt, 1) << " | "
                     << sqlite3_column_text(stmt, 2) << " | Nível "
                     << sqlite3_column_int(stmt, 3) << "\n";
            }
            sqlite3_finalize(stmt);
        }
};

} // namespace RPG
#endif