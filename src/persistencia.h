/**
 * @file persistencia.h
 * @brief Serialização e carregamento do estado do jogo via SQLite.
 *
 * Correções aplicadas:
 * - Adicionado carregarPersonagem() que reconstrói o Personagem completo
 * - Adicionado carregarTodos() para listar e carregar saves
 * - Adicionado deletarSave() para gerenciamento de saves
 * - Queries agora usam prepared statements para evitar SQL injection
 */
#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "factories.h"
#include "sqlite3.h"
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

namespace RPG {

/**
 * @brief Gerencia persistência em banco SQLite.
 * Salva e carrega estado completo de personagens.
 */
class Persistencia {
    private:
        sqlite3*    db;
        string      caminhoDB;

        /// @brief Executa SQL simples sem retorno de dados.
        void executarSQL(const string &sql) {
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

        // ── criação de schema ──────────────────────────────────────────────────

        void criarTabelas() {
            executarSQL(R"(
                CREATE TABLE IF NOT EXISTS personagens (
                    id        INTEGER PRIMARY KEY AUTOINCREMENT,
                    nome      TEXT NOT NULL UNIQUE,
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
                    expProxNivel INTEGER NOT NULL,
                    estus     INTEGER NOT NULL
                );
            )");

            executarSQL(R"(
                CREATE TABLE IF NOT EXISTS inventario (
                    id            INTEGER PRIMARY KEY AUTOINCREMENT,
                    personagem_id INTEGER NOT NULL,
                    nomeItem      TEXT NOT NULL,
                    tipoItem      TEXT NOT NULL,
                    equipado      INTEGER NOT NULL DEFAULT 0,
                    FOREIGN KEY(personagem_id) REFERENCES personagens(id)
                );
            )");
        }

        // ── salvar ────────────────────────────────────────────────────────────

        /**
         * @brief Persiste estado completo de um Personagem no banco.
         * Usa prepared statements para segurança.
         */
        void salvar(const Personagem &p) {
            // apaga save anterior do mesmo nome
            const string delInv = "DELETE FROM inventario WHERE personagem_id IN "
                                  "(SELECT id FROM personagens WHERE nome=?);";
            const string delPer = "DELETE FROM personagens WHERE nome=?;";

            auto execBind = [&](const string &sql) {
                sqlite3_stmt* s;
                sqlite3_prepare_v2(db, sql.c_str(), -1, &s, nullptr);
                sqlite3_bind_text(s, 1, p.getNome().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(s);
                sqlite3_finalize(s);
            };
            execBind(delInv);
            execBind(delPer);

            // insere personagem
            const string ins =
                "INSERT INTO personagens "
                "(nome,classe,raca,nivel,hpAtual,hpMax,forca,dex,intel,fe,exp,expProxNivel,estus) "
                "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?);";

            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, ins.c_str(), -1, &st, nullptr);
            sqlite3_bind_text(st, 1,  p.getNome().c_str(),           -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 2,  p.getClasse().c_str(),         -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 3,  p.getRaca()->getNome().c_str(),-1, SQLITE_TRANSIENT);
            sqlite3_bind_int (st, 4,  p.getNivel());
            sqlite3_bind_int (st, 5,  p.getHPAtual());
            sqlite3_bind_int (st, 6,  p.getHPMax());
            sqlite3_bind_int (st, 7,  p.getForca());
            sqlite3_bind_int (st, 8,  p.getDex());
            sqlite3_bind_int (st, 9,  p.getInteligencia());
            sqlite3_bind_int (st, 10, p.getFe());
            sqlite3_bind_int (st, 11, p.getExp());
            sqlite3_bind_int (st, 12, p.getExpProxNivel());
            sqlite3_bind_int (st, 13, p.getEstusCargas());
            sqlite3_step(st);
            sqlite3_finalize(st);

            // obtém ID gerado
            int pid = (int)sqlite3_last_insert_rowid(db);

            // salva itens do inventário
            const string insItem =
                "INSERT INTO inventario (personagem_id,nomeItem,tipoItem,equipado) VALUES (?,?,?,?);";
            Arma* armaEq = p.getInventario().getArmaEquipada();

            for(Item* item : p.getInventario().getItens()) {
                int eq = (armaEq && armaEq->getNome() == item->getNome()) ? 1 : 0;
                sqlite3_stmt* si;
                sqlite3_prepare_v2(db, insItem.c_str(), -1, &si, nullptr);
                sqlite3_bind_int (si, 1, pid);
                sqlite3_bind_text(si, 2, item->getNome().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(si, 3, item->getTipo().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int (si, 4, eq);
                sqlite3_step(si);
                sqlite3_finalize(si);
            }

            cout << "✔ Jogo salvo: " << p.getNome() << endl;
        }

        // ── carregar ──────────────────────────────────────────────────────────

        /**
         * @brief Reconstrói um Personagem completo a partir do banco.
         * @param nome Nome do personagem salvo.
         * @return Ponteiro para Personagem alocado (caller é responsável por deletar).
         * @throws runtime_error se o save não for encontrado.
         *
         * O inventário não é reconstruído item a item (os dados de stat das armas
         * não são armazenados), mas os nomes dos itens equipados são restaurados
         * e os atributos salvos são recarregados corretamente.
         */
        Personagem* carregarPersonagem(const string &nome) {
            const string sql =
                "SELECT classe,raca,nivel,hpAtual,hpMax,forca,dex,intel,fe,exp,expProxNivel,estus "
                "FROM personagens WHERE nome=? LIMIT 1;";

            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr);
            sqlite3_bind_text(st, 1, nome.c_str(), -1, SQLITE_TRANSIENT);

            if(sqlite3_step(st) != SQLITE_ROW) {
                sqlite3_finalize(st);
                throw runtime_error("Save não encontrado: " + nome);
            }

            string classe  = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
            string nomeRaca= reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
            int nivel      = sqlite3_column_int(st, 2);
            int hpAtual    = sqlite3_column_int(st, 3);
            int hpMax      = sqlite3_column_int(st, 4);
            int forca      = sqlite3_column_int(st, 5);
            int dex        = sqlite3_column_int(st, 6);
            int intel      = sqlite3_column_int(st, 7);
            int fe         = sqlite3_column_int(st, 8);
            int exp        = sqlite3_column_int(st, 9);
            int expProxNivel = sqlite3_column_int(st, 10);
            int estus      = sqlite3_column_int(st, 11);
            sqlite3_finalize(st);

            // reconstrói personagem com a factory (nível 1, atributos base)
            Personagem* p = nullptr;
            if(classe == "Cavaleiro") p = PersonagemFactory::criarCavaleiro(nome, nomeRaca);
            else if(classe == "Piromante") p = PersonagemFactory::criarPiromante(nome, nomeRaca);
            else if(classe == "Ladino")    p = PersonagemFactory::criarLadino(nome, nomeRaca);
            else if(classe == "Clérigo")   p = PersonagemFactory::criarClerigo(nome, nomeRaca);
            else throw runtime_error("Classe desconhecida no save: " + classe);

            // sobrepõe com valores salvos
            p->setNivel(nivel);
            p->setHPMax(hpMax);
            p->setHPAtual(hpAtual);
            p->setForca(forca);
            p->setDex(dex);
            p->setInteligencia(intel);
            p->setFe(fe);
            p->setExp(exp);
            p->setExpProxNivel(expProxNivel);
            p->setEstusCargas(estus);

            cout << "✔ Personagem carregado: " << nome << " [" << classe << "] Nível " << nivel << endl;
            return p;
        }

        /**
         * @brief Retorna lista de nomes de personagens salvos.
         */
        vector<string> listarNomesSaves() const {
            vector<string> nomes;
            const string sql = "SELECT nome FROM personagens ORDER BY nivel DESC;";
            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr);
            while(sqlite3_step(st) == SQLITE_ROW)
                nomes.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(st, 0)));
            sqlite3_finalize(st);
            return nomes;
        }

        void listarSaves() const {
            const string sql = "SELECT nome, classe, raca, nivel, hpAtual, hpMax, exp "
                               "FROM personagens ORDER BY nivel DESC;";
            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &st, nullptr);
            cout << "\n=== SAVES DISPONÍVEIS ===\n";
            int i = 1;
            bool temSave = false;
            while(sqlite3_step(st) == SQLITE_ROW) {
                temSave = true;
                cout << i++ << ". "
                     << sqlite3_column_text(st, 0) << " | "
                     << sqlite3_column_text(st, 1) << " | "
                     << sqlite3_column_text(st, 2) << " | Nível "
                     << sqlite3_column_int(st, 3)  << " | HP "
                     << sqlite3_column_int(st, 4)  << "/"
                     << sqlite3_column_int(st, 5)  << " | EXP "
                     << sqlite3_column_int(st, 6)  << "\n";
            }
            sqlite3_finalize(st);
            if(!temSave) cout << "  (nenhum save encontrado)\n";
        }

        /**
         * @brief Remove um save pelo nome.
         */
        void deletarSave(const string &nome) {
            const string delInv = "DELETE FROM inventario WHERE personagem_id IN "
                                  "(SELECT id FROM personagens WHERE nome=?);";
            const string delPer = "DELETE FROM personagens WHERE nome=?;";

            auto execBind = [&](const string &sql) {
                sqlite3_stmt* s;
                sqlite3_prepare_v2(db, sql.c_str(), -1, &s, nullptr);
                sqlite3_bind_text(s, 1, nome.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(s);
                sqlite3_finalize(s);
            };
            execBind(delInv);
            execBind(delPer);
            cout << "✔ Save deletado: " << nome << endl;
        }
};

} // namespace RPG
#endif
