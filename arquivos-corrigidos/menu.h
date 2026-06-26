/**
 * @file menu.h
 * @brief CLI interativa do RPG Manager — Dark Souls edition.
 *
 * BUGS CORRIGIDOS:
 *  1. pausar() chamava cin.ignore() seguido de cin.get(), consumindo o buffer
 *     duas vezes. Após um cin >> int bem-sucedido, o '\n' permanece no buffer;
 *     cin.ignore() o remove, mas cin.get() bloqueia esperando um segundo '\n'
 *     — o usuário precisava apertar Enter duas vezes para continuar.
 *     Corrigido: removido o cin.get() redundante; cin.ignore() já é suficiente
 *     para limpar o buffer e avançar.
 */
#ifndef MENU_H
#define MENU_H

#include "batalha.h"
#include "factories.h"
#include "persistencia.h"
#include <vector>
#include <string>
#include <limits>
using namespace std;

namespace RPG {

class Menu {
    private:
        vector<Personagem*> personagens;
        Personagem* personagemAtivo = nullptr;
        Persistencia        db;

        void limpar() {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        }

        // FIX: removido cin.get() redundante.
        // Antes: cin.ignore() limpava o '\n', depois cin.get() bloqueava
        // aguardando um segundo '\n' — exigia dois Enters para continuar.
        // Agora: cin.ignore() limpa o buffer e retorna imediatamente.
        void pausar() {
            cout << "\nPressione ENTER para continuar...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

        // ── criação de personagem ─────────────────────────────────────────────
        void criarPersonagem() {
            limpar(); cabecalho();
            cout << "=== CRIAR PERSONAGEM ===\n\n";

            cout << "Nome do personagem: ";
            string nome;
            cin >> ws;
            getline(cin, nome);

            cout << "\nEscolha a classe:\n"
                 << "  1. Cavaleiro  (força, vitalidade)\n"
                 << "  2. Piromante  (inteligência, magia de fogo)\n"
                 << "  3. Ladino     (destreza, furtividade)\n"
                 << "  4. Clérigo    (fé, cura)\n"
                 << "> ";
            int classe;
            if (!(cin >> classe)) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                classe = 1;
            }

            cout << "\nEscolha a raça:\n"
                 << "  1. Humano       (+1 em tudo)\n"
                 << "  2. Morto-Vivo   (+HP e força)\n"
                 << "  3. Pygmy        (+inteligência e fé)\n"
                 << "  4. Dragão Antigo(+HP e força brutais, -magia)\n"
                 << "> ";
            int raca;
            if (!(cin >> raca)) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                raca = 1;
            }

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

        // ── selecionar personagem ativo ───────────────────────────────────────
        void selecionarPersonagem() {
            if(personagens.empty()) {
                cout << "Nenhum personagem criado ainda.\n";
                pausar(); return;
            }
            limpar(); cabecalho();
            cout << "=== SELECIONAR PERSONAGEM ===\n";
            for(int i = 0; i < (int)personagens.size(); i++) {
                cout << "  " << (i+1) << ". " << *personagens[i] << "\n";
            }
            cout << "> ";
            int idx;
            if (!(cin >> idx)) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                idx = -1;
            }
            idx--;
            if(idx >= 0 && idx < (int)personagens.size()) {
                personagemAtivo = personagens[idx];
                cout << "Personagem ativo: " << personagemAtivo->getNome() << "\n";
            }
            pausar();
        }

        // ── ver status ────────────────────────────────────────────────────────
        void verStatus() {
            limpar(); cabecalho();
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            personagemAtivo->exibirStatus();
            pausar();
        }

        // ── inventário ────────────────────────────────────────────────────────
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
                     << "  5. Usar poção\n"
                     << "  0. Voltar\n> ";
                int op;
                if (!(cin >> op)) {
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    op = -1;
                }
                switch(op) {
                    case 1:
                        personagemAtivo->getInventario().listarItens();
                        pausar(); break;
                    case 2: {
                        cout << "Nome da arma: ";
                        string nm;
                        cin >> ws; getline(cin, nm);
                        personagemAtivo->getInventario().equiparArma(nm);
                        pausar(); break;
                    }
                    case 3: {
                        cout << "Nome da armadura: ";
                        string nm;
                        cin >> ws; getline(cin, nm);
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
                        } else cout << "Sem poções.\n";
                        pausar(); break;
                    }
                    case 0: voltar = true; break;
                }
            }
        }

        // ── batalha ───────────────────────────────────────────────────────────
        void menuBatalha() {
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            limpar(); cabecalho();
            cout << "=== BATALHA ===\n"
                 << "Escolha o inimigo:\n"
                 << "  1. Soldado Oco         (nível 1)\n"
                 << "  2. Cavaleiro Negro     (nível 5)\n"
                 << "  3. Catavento [Boss]    (nível 8)\n"
                 << "  4. Gárgula [Boss]      (nível 10)\n"
                 << "  5. Ornstein [Boss]     (nível 20)\n"
                 << "> ";
            int op;
            if (!(cin >> op)) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                op = 1;
            }

            string tipo;
            switch(op) {
                case 1: tipo = "hollow";      break;
                case 2: tipo = "blackKnight"; break;
                case 3: tipo = "pinwheel";    break;
                case 4: tipo = "gargoyle";    break;
                case 5: tipo = "ornstein";    break;
                default: tipo = "hollow";
            }

            Inimigo* inimigo = InimigoFactory::criarInimigo(tipo);
            Batalha batalha(*personagemAtivo, *inimigo);
            batalha.executar();
            batalha.exibirLog();
            delete inimigo;
            pausar();
        }

        // ── salvar / carregar ─────────────────────────────────────────────────
        void salvarJogo() {
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            try {
                db.salvar(*personagemAtivo);
            } catch(const exception &e) {
                cout << "Erro ao salvar: " << e.what() << endl;
            }
            pausar();
        }

        void verSaves() {
            limpar(); cabecalho();
            db.listarSaves();
            pausar();
        }

        // ── habilidades ───────────────────────────────────────────────────────
        void verHabilidades() {
            limpar(); cabecalho();
            if(!personagemAtivo) { cout << "Nenhum personagem ativo.\n"; pausar(); return; }
            cout << "=== HABILIDADES DE " << personagemAtivo->getNome() << " ===\n";
            for(const Habilidade* h : personagemAtivo->getHabilidades()) {
                cout << "  " << *h << "\n";
            }
            pausar();
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
                     << "  2. Selecionar personagem\n"
                     << "  3. Ver status\n"
                     << "  4. Inventário\n"
                     << "  5. Habilidades\n"
                     << "  6. Batalhar\n"
                     << "  7. Salvar jogo\n"
                     << "  8. Ver saves\n"
                     << "  0. Sair\n"
                     << "> ";
                int op;
                if (!(cin >> op)) {
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    op = -1;
                }
                switch(op) {
                    case 1: criarPersonagem();     break;
                    case 2: selecionarPersonagem(); break;
                    case 3: verStatus();            break;
                    case 4: menuInventario();       break;
                    case 5: verHabilidades();       break;
                    case 6: menuBatalha();          break;
                    case 7: salvarJogo();           break;
                    case 8: verSaves();             break;
                    case 0: rodando = false;        break;
                    default: cout << "Opção inválida.\n";
                }
            }
            cout << "\n\"Que a Chama não se apague.\"\n\n";
        }
};

} // namespace RPG
#endif
