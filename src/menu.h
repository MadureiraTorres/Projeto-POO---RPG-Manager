/**
 * @file menu.h
 * @brief CLI interativa do RPG Manager — Dark Souls edition.
 *
 * Correções e melhorias:
 * - Adicionado menuCarregarJogo() (RF-7 corrigido)
 * - Adicionado listarPersonagens() com status detalhado (RF-2)
 * - Adicionado batalha PvP humano vs humano (RF-5)
 * - Melhorado tratamento de input cin para evitar loops infinitos
 * - Adicionado menu de gerenciamento de saves (deletar)
 */
#ifndef MENU_H
#define MENU_H

#include "batalha.h"
#include "factories.h"
#include "persistencia.h"
#include <vector>
#include <limits>
using namespace std;

namespace RPG {

class Menu {
    private:
        vector<Personagem*> personagens;
        Personagem*         personagemAtivo = nullptr;
        Persistencia        db;

        // ── utilitários de I/O ───────────────────────────────────────────────

        void limpar() {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        }

        void pausar() {
            cout << "\nPressione ENTER para continuar...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

        /// @brief Lê inteiro de cin com tratamento de erro robusto.
        int lerInt(const string &prompt = "> ") {
            int v;
            cout << prompt;
            while(!(cin >> v)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Entrada inválida. Tente novamente " << prompt;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }

        /// @brief Lê string (linha inteira) de cin.
        string lerString(const string &prompt = "> ") {
            cout << prompt;
            string s;
            // limpa buffer pendente
            if(cin.peek() == '\n') cin.ignore();
            getline(cin, s);
            return s;
        }

        void cabecalho() const {
            cout << "\n╔════════════════════════════════════════════╗\n"
                 << "║          RPG MANAGER — DARK SOULS          ║\n"
                 << "╚════════════════════════════════════════════╝\n";
            if(personagemAtivo)
                cout << "  Personagem ativo: " << personagemAtivo->getNome()
                     << " [" << personagemAtivo->getClasse() << "] "
                     << "Nível " << personagemAtivo->getNivel() << "\n\n";
        }

        // ── RF-1: criação de personagem ───────────────────────────────────────

        void criarPersonagem() {
            limpar(); cabecalho();
            cout << "=== CRIAR PERSONAGEM ===\n\n";

            string nome = lerString("Nome do personagem: ");
            if(nome.empty()) { cout << "Nome inválido.\n"; pausar(); return; }

            cout << "\nEscolha a classe:\n"
                 << "  1. Cavaleiro  (força, vitalidade)\n"
                 << "  2. Piromante  (inteligência, magia de fogo)\n"
                 << "  3. Ladino     (destreza, furtividade)\n"
                 << "  4. Clérigo    (fé, cura)\n";
            int classe = lerInt("> ");

            cout << "\nEscolha a raça:\n"
                 << "  1. Humano        (+1 em tudo)\n"
                 << "  2. Morto-Vivo    (+HP e força)\n"
                 << "  3. Pygmy         (+inteligência e fé)\n"
                 << "  4. Dragão Antigo (+HP e força brutais, -magia)\n";
            int raca = lerInt("> ");

            string nomeRaca;
            switch(raca) {
                case 1: nomeRaca = "Humano"; break;
                case 2: nomeRaca = "Morto-Vivo"; break;
                case 3: nomeRaca = "Pygmy"; break;
                case 4: nomeRaca = "Dragão Antigo"; break;
                default: nomeRaca = "Humano";
            }

            Personagem* p = nullptr;
            try {
                switch(classe) {
                    case 1: p = PersonagemFactory::criarCavaleiro(nome, nomeRaca); break;
                    case 2: p = PersonagemFactory::criarPiromante(nome, nomeRaca); break;
                    case 3: p = PersonagemFactory::criarLadino(nome, nomeRaca); break;
                    case 4: p = PersonagemFactory::criarClerigo(nome, nomeRaca); break;
                    default: p = PersonagemFactory::criarCavaleiro(nome, nomeRaca);
                }
                personagens.push_back(p);
                personagemAtivo = p;
                cout << "\n✔ Personagem criado!\n";
                p->exibirStatus();
            } catch(const exception &e) {
                cout << "Erro: " << e.what() << endl;
            }
            pausar();
        }

        // ── RF-2: listar personagens com status detalhado ─────────────────────

        void listarPersonagens() {
            limpar(); cabecalho();
            cout << "=== TODOS OS PERSONAGENS ===\n";
            if(personagens.empty()) {
                cout << "  Nenhum personagem criado ainda.\n";
                pausar(); return;
            }
            for(int i = 0; i < (int)personagens.size(); i++) {
                cout << "\n[" << (i+1) << "] ";
                personagens[i]->exibirStatus();
            }
            pausar();
        }

        // ── selecionar personagem ativo ───────────────────────────────────────

        void selecionarPersonagem() {
            if(personagens.empty()) {
                cout << "Nenhum personagem criado ainda.\n";
                pausar(); return;
            }
            limpar(); cabecalho();
            cout << "=== SELECIONAR PERSONAGEM ATIVO ===\n";
            for(int i = 0; i < (int)personagens.size(); i++) {
                cout << "  " << (i+1) << ". " << *personagens[i] << "\n";
            }
            int idx = lerInt("> ") - 1;
            if(idx >= 0 && idx < (int)personagens.size()) {
                personagemAtivo = personagens[idx];
                cout << "✔ Personagem ativo: " << personagemAtivo->getNome() << "\n";
            } else {
                cout << "Índice inválido.\n";
            }
            pausar();
        }

        // ── RF-3/4: ver status e inventário ──────────────────────────────────

        void verStatus() {
            limpar(); cabecalho();
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            personagemAtivo->exibirStatus();
            pausar();
        }

        void menuInventario() {
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            bool voltar = false;
            while(!voltar) {
                limpar(); cabecalho();
                cout << "=== INVENTÁRIO ===\n"
                     << "  1. Listar itens\n"
                     << "  2. Equipar arma\n"
                     << "  3. Equipar armadura\n"
                     << "  4. Desequipar arma\n"
                     << "  5. Usar poção do inventário\n"
                     << "  0. Voltar\n";
                int op = lerInt("> ");
                switch(op) {
                    case 1:
                        personagemAtivo->getInventario().listarItens();
                        pausar(); break;
                    case 2: {
                        string nm = lerString("Nome da arma: ");
                        personagemAtivo->getInventario().equiparArma(nm);
                        pausar(); break;
                    }
                    case 3: {
                        string nm = lerString("Nome da armadura: ");
                        personagemAtivo->getInventario().equiparArmadura(nm);
                        pausar(); break;
                    }
                    case 4:
                        personagemAtivo->getInventario().desequiparArma();
                        pausar(); break;
                    case 5: {
                        Pocao* poc = personagemAtivo->getInventario().getPocao();
                        if(poc && poc->temCargas()) {
                            poc->usar();
                            personagemAtivo->curar(poc->getCura());
                            cout << "+" << poc->getCura() << " HP!" << endl;
                        } else cout << "Sem poções disponíveis.\n";
                        pausar(); break;
                    }
                    case 0: voltar = true; break;
                    default: cout << "Opção inválida.\n";
                }
            }
        }

        // ── RF-5: batalha (PvE e PvP) ─────────────────────────────────────────

        void menuBatalha() {
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            limpar(); cabecalho();
            cout << "=== MODO DE BATALHA ===\n"
                 << "  1. Batalha contra Inimigo (PvE)\n"
                 << "  2. Batalha contra outro Personagem (PvP)\n"
                 << "  0. Voltar\n";
            int modo = lerInt("> ");

            if(modo == 1) {
                batalhaVsInimigo();
            } else if(modo == 2) {
                batalhaVsPersonagem();
            }
        }

        void batalhaVsInimigo() {
            limpar(); cabecalho();
            cout << "=== BATALHA (PvE) ===\n"
                 << "Escolha o inimigo:\n"
                 << "  1. Soldado Oco         (nível 1)\n"
                 << "  2. Cavaleiro Negro     (nível 5)\n"
                 << "  3. Catavento [Boss]    (nível 8)\n"
                 << "  4. Gárgula [Boss]      (nível 10)\n"
                 << "  5. Ornstein [Boss]     (nível 20)\n";
            int op = lerInt("> ");

            string tipo;
            switch(op) {
                case 1: tipo = "hollow";      break;
                case 2: tipo = "blackKnight"; break;
                case 3: tipo = "pinwheel";    break;
                case 4: tipo = "gargoyle";    break;
                case 5: tipo = "ornstein";    break;
                default: tipo = "hollow";
            }

            try {
                Inimigo* inimigo = InimigoFactory::criarInimigo(tipo);
                Batalha batalha(*personagemAtivo, *inimigo);
                batalha.executar();
                batalha.exibirLog();
                delete inimigo;
            } catch(const exception &e) {
                cout << "Erro na batalha: " << e.what() << endl;
            }
            pausar();
        }

        /**
         * @brief RF-5: Batalha humano vs humano.
         * Dois personagens da lista se enfrentam em turnos alternados.
         */
        void batalhaVsPersonagem() {
            if(personagens.size() < 2) {
                cout << "É necessário pelo menos 2 personagens para batalha PvP.\n"
                     << "Crie mais personagens primeiro.\n";
                pausar(); return;
            }
            limpar(); cabecalho();
            cout << "=== BATALHA PvP ===\n"
                 << "Personagem 1 (ativo): " << *personagemAtivo << "\n\n"
                 << "Escolha o oponente:\n";
            for(int i = 0; i < (int)personagens.size(); i++) {
                if(personagens[i] != personagemAtivo)
                    cout << "  " << (i+1) << ". " << *personagens[i] << "\n";
            }
            int idx = lerInt("> ") - 1;
            if(idx < 0 || idx >= (int)personagens.size() || personagens[idx] == personagemAtivo) {
                cout << "Oponente inválido.\n"; pausar(); return;
            }

            Personagem* oponente = personagens[idx];
            cout << "\n⚔  " << personagemAtivo->getNome()
                 << " vs " << oponente->getNome() << " ⚔\n";

            BatalhaPvP pvp(*personagemAtivo, *oponente);
            pvp.executar();
            pvp.exibirLog();
            pausar();
        }

        // ── habilidades ───────────────────────────────────────────────────────

        void verHabilidades() {
            limpar(); cabecalho();
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            cout << "=== HABILIDADES DE " << personagemAtivo->getNome() << " ===\n";
            const auto& habs = personagemAtivo->getHabilidades();
            if(habs.empty()) { cout << "  (sem habilidades)\n"; pausar(); return; }
            for(const Habilidade* h : habs) {
                cout << "  " << *h << "\n";
            }
            pausar();
        }

        // ── RF-7: salvar e carregar ───────────────────────────────────────────

        void salvarJogo() {
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            try {
                db.salvar(*personagemAtivo);
            } catch(const exception &e) {
                cout << "Erro ao salvar: " << e.what() << endl;
            }
            pausar();
        }

        /**
         * @brief Carrega um personagem do banco e o adiciona à lista de personagens.
         */
        void carregarJogo() {
            limpar(); cabecalho();
            cout << "=== CARREGAR JOGO ===\n";
            db.listarSaves();

            vector<string> nomes = db.listarNomesSaves();
            if(nomes.empty()) {
                cout << "\nNenhum save disponível.\n";
                pausar(); return;
            }

            cout << "\nDigite o nome do personagem a carregar (ou 0 para cancelar): ";
            string nome = lerString();
            if(nome == "0" || nome.empty()) { pausar(); return; }

            // verifica se já está carregado
            for(Personagem* p : personagens) {
                if(p->getNome() == nome) {
                    cout << "✔ Personagem '" << nome << "' já está carregado. Ativando...\n";
                    personagemAtivo = p;
                    pausar(); return;
                }
            }

            try {
                Personagem* p = db.carregarPersonagem(nome);
                personagens.push_back(p);
                personagemAtivo = p;
                p->exibirStatus();
            } catch(const exception &e) {
                cout << "Erro ao carregar: " << e.what() << endl;
            }
            pausar();
        }

        void verSaves() {
            limpar(); cabecalho();
            cout << "=== GERENCIAR SAVES ===\n";
            db.listarSaves();

            cout << "\n  1. Carregar personagem\n"
                 << "  2. Deletar save\n"
                 << "  0. Voltar\n";
            int op = lerInt("> ");
            if(op == 1) {
                carregarJogo();
                return; // carregarJogo já chama pausar()
            } else if(op == 2) {
                string nome = lerString("Nome do save a deletar: ");
                try {
                    db.deletarSave(nome);
                } catch(const exception &e) {
                    cout << "Erro: " << e.what() << endl;
                }
                pausar();
            }
        }

    public:
        Menu() : db("rpg_save.db") {}

        ~Menu() {
            for(Personagem* p : personagens) delete p;
        }

        void executar() {
            bool rodando = true;
            while(rodando) {
                limpar(); cabecalho();
                cout << "  1. Criar personagem\n"
                     << "  2. Listar todos os personagens\n"
                     << "  3. Selecionar personagem ativo\n"
                     << "  4. Ver status do personagem ativo\n"
                     << "  5. Inventário\n"
                     << "  6. Habilidades\n"
                     << "  7. Batalhar\n"
                     << "  8. Salvar jogo\n"
                     << "  9. Carregar jogo\n"
                     << " 10. Ver/Gerenciar saves\n"
                     << "  0. Sair\n";
                int op = lerInt("> ");
                switch(op) {
                    case 1:  criarPersonagem();      break;
                    case 2:  listarPersonagens();    break;
                    case 3:  selecionarPersonagem(); break;
                    case 4:  verStatus();            break;
                    case 5:  menuInventario();       break;
                    case 6:  verHabilidades();       break;
                    case 7:  menuBatalha();          break;
                    case 8:  salvarJogo();           break;
                    case 9:  carregarJogo();         break;
                    case 10: verSaves();             break;
                    case 0:  rodando = false;        break;
                    default: cout << "Opção inválida.\n"; pausar();
                }
            }
            cout << "\n\"Que a Chama não se apague.\"\n\n";
        }
};

} // namespace RPG
#endif
