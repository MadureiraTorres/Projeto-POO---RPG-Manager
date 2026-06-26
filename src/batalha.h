/**
 * @file batalha.h
 * @brief Sistema de batalha por turnos com log de ações e drop de itens.
 *
 * Melhorias:
 * - Log usa std::list<LogBatalha> (critério STL)
 * - Sistema de drop de itens inspirado em Dark Souls após vitória
 * - BatalhaPvP mantido
 */
#ifndef BATALHA_H
#define BATALHA_H

#include "personagem.h"
#include "inimigo.h"
#include "factories.h"
#include <vector>
#include <list>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
using namespace std;

namespace RPG {

/// @brief Registro de uma ação num turno de batalha.
struct LogBatalha {
    int    turno;
    string ator;
    string descricao;
};

// ── Tabela de drops ──────────────────────────────────────────────────────────

/**
 * @brief Entrada na tabela de drop de um inimigo.
 * @note Taxa em porcentagem (0–100).
 */
struct DropEntry {
    string tipoItem;   ///< "arma", "armadura", "pocao", "especial"
    string subtipo;    ///< chave para a factory (ex.: "espadaLonga", "estus")
    string slotExtra;  ///< slot de armadura (vazio se não for armadura)
    int    taxa;       ///< chance de drop em %
};

/**
 * @brief Tabela de drops por tipo de inimigo.
 * Retorna lista de possíveis drops com suas taxas.
 */
static vector<DropEntry> tabelaDrops(const string &tipoInimigo) {
    if(tipoInimigo == "Soldado Oco")
        return {
            {"pocao",    "estus",       "", 40},
            {"arma",     "espadaLonga", "", 15},
        };
    if(tipoInimigo == "Cavaleiro Negro")
        return {
            {"arma",     "espadaLonga", "",         25},
            {"armadura", "elite",       "capacete",  20},
            {"armadura", "elite",       "luvas",     20},
            {"pocao",    "estus",       "",          30},
        };
    if(tipoInimigo == "Catavento (Boss)")
        return {
            {"pocao",    "estusReforcado", "",          60},
            {"especial", "anelVida",       "",          35},
            {"arma",     "alabarda",       "",          20},
        };
    if(tipoInimigo == "Gárgula da Torre Bell (Boss)")
        return {
            {"armadura", "elite", "peitoral", 50},
            {"armadura", "elite", "calças",   50},
            {"pocao",    "estusReforcado", "", 70},
            {"arma",     "alabarda",        "", 30},
        };
    if(tipoInimigo == "Ornstein, o Caçador de Dragões (Boss)")
        return {
            {"arma",     "alabarda",       "", 60},
            {"armadura", "elite",          "capacete", 80},
            {"armadura", "elite",          "peitoral", 80},
            {"armadura", "elite",          "calças",   80},
            {"armadura", "elite",          "luvas",    80},
            {"especial", "anelForca",      "", 50},
            {"pocao",    "estusReforcado", "", 100},
        };
    return {};
}

// ── Batalha PvE ───────────────────────────────────────────────────────────────

/**
 * @brief Gerencia o loop de combate por turnos entre Personagem e Inimigo.
 * @note Log usa std::list para demonstrar uso da estrutura STL pedida.
 */
class Batalha {
    private:
        Personagem&        jogador;
        Inimigo&           inimigo;
        list<LogBatalha>   log;   ///< std::list para histórico de ações
        int                turnoAtual;

        void registrar(const string &ator, const string &msg) {
            log.push_back({turnoAtual, ator, msg});
        }

        void imprimirSeparador() const {
            cout << "──────────────────────────────────────────" << endl;
        }

        int lerInt() {
            int v;
            while(!(cin >> v)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "> ";
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }

        void menuAcaoJogador() {
            cout << "\n[TURNO " << turnoAtual << "] " << jogador.getNome() << " — escolha:\n"
                 << "  1. Atacar\n"
                 << "  2. Usar Habilidade\n"
                 << "  3. Usar Estus\n"
                 << "  4. Usar poção do inventário\n"
                 << "> ";

            switch(lerInt()) {
                case 1:
                    jogador.atacar(inimigo);
                    registrar(jogador.getNome(), "atacou " + inimigo.getNome());
                    break;
                case 2:
                    menuHabilidade();
                    break;
                case 3:
                    jogador.usarEstus();
                    registrar(jogador.getNome(), "usou Estus Flask");
                    break;
                case 4:
                    menuItem();
                    break;
                default:
                    cout << "Ação inválida — atacando por padrão." << endl;
                    jogador.atacar(inimigo);
                    registrar(jogador.getNome(), "atacou " + inimigo.getNome());
            }
        }

        void menuHabilidade() {
            const auto& habs = jogador.getHabilidades();
            if(habs.empty()) { cout << "Sem habilidades.\n"; return; }
            cout << "Habilidades:\n";
            for(int i = 0; i < (int)habs.size(); i++)
                cout << "  " << (i+1) << ". " << *habs[i] << "\n";
            cout << "> ";
            int idx = lerInt() - 1;
            jogador.usarHabilidade(idx, inimigo);
            registrar(jogador.getNome(), "usou habilidade");
        }

        void menuItem() {
            Pocao* p = jogador.getInventario().getPocao();
            if(p && p->temCargas()) {
                p->usar();
                jogador.curar(p->getCura());
                cout << jogador.getNome() << " usou " << p->getNome()
                     << " — +" << p->getCura() << " HP!" << endl;
                registrar(jogador.getNome(), "usou " + p->getNome());
            } else {
                cout << "Nenhum consumível disponível." << endl;
            }
        }

        /**
         * @brief Processa drops de itens após vitória, inspirado em Dark Souls.
         * Rola chance para cada entrada da tabela de drops do inimigo.
         */
        void processarDrops() {
            vector<DropEntry> drops = tabelaDrops(inimigo.getNome());
            if(drops.empty()) return;

            cout << "\n  ── Itens encontrados ──\n";
            bool algumDrop = false;

            for(const DropEntry &d : drops) {
                int roll = rand() % 100;
                if(roll < d.taxa) {
                    algumDrop = true;
                    Item* novoItem = nullptr;
                    try {
                        if(d.tipoItem == "pocao")
                            novoItem = ItemFactory::criarPocao(d.subtipo);
                        else if(d.tipoItem == "arma")
                            novoItem = ItemFactory::criarArma(d.subtipo);
                        else if(d.tipoItem == "armadura")
                            novoItem = ItemFactory::criarArmadura(d.subtipo, d.slotExtra);
                        else if(d.tipoItem == "especial")
                            novoItem = ItemFactory::criarItemEspecial(d.subtipo);
                    } catch(...) { continue; }

                    if(novoItem) {
                        cout << "  ★ Drop: ";
                        novoItem->descrever();
                        if(!jogador.getInventario().adicionarItem(novoItem)) {
                            cout << "  (inventário cheio — item perdido)\n";
                            delete novoItem;
                        }
                    }
                }
            }
            if(!algumDrop) cout << "  (nenhum item dropado)\n";
        }

    public:
        Batalha(Personagem &j, Inimigo &i)
            : jogador(j), inimigo(i), turnoAtual(0) {}

        /**
         * @brief Executa o loop de batalha por turnos.
         * @return true se o jogador venceu.
         */
        bool executar() {
            cout << "\n╔══════════════════════════════════════════╗\n"
                 << "║          BATALHA INICIADA! (PvE)         ║\n"
                 << "╚══════════════════════════════════════════╝\n";
            cout << jogador << " vs " << inimigo << "\n";
            imprimirSeparador();

            while(jogador.estaVivo() && inimigo.estaVivo()) {
                turnoAtual++;
                menuAcaoJogador();
                jogador.recuperarStamina(25);
                jogador.reduzirCooldowns();

                if(!inimigo.estaVivo()) break;
                imprimirSeparador();
                inimigo.atacar(jogador);
                registrar(inimigo.getNome(), "atacou " + jogador.getNome());

                cout << "\n» " << jogador << "\n» " << inimigo << "\n";
                imprimirSeparador();
            }

            return resolverFim();
        }

        /// @brief Resolve desfecho da batalha, concede exp e processa drops.
        bool resolverFim() {
            cout << "\n╔══════════════════════════════════════════╗\n";
            if(jogador.estaVivo()) {
                cout << "║  VITÓRIA! " << jogador.getNome() << " sobreviveu!\n";
                cout << "╚══════════════════════════════════════════╝\n";
                jogador.ganharExp(inimigo.getExpRecompensa());
                cout << "  Almas obtidas: " << inimigo.getAlmasRecompensa() << "\n";
                processarDrops();
                jogador.recarregarEstus();
                return true;
            } else {
                cout << "║  DERROTA — " << jogador.getNome() << " foi abatido!\n";
                cout << "╚══════════════════════════════════════════╝\n";
                cout << "  \"YOU DIED\"\n";
                return false;
            }
        }

        void exibirLog() const {
            cout << "\n=== LOG DA BATALHA ===\n";
            for(const auto &e : log)
                cout << "[Turno " << e.turno << "] " << e.ator << ": " << e.descricao << "\n";
        }
};

// ── Batalha PvP ───────────────────────────────────────────────────────────────

/**
 * @brief RF-5: Combate por turnos entre dois Personagens.
 * @note Log também usa std::list.
 */
class BatalhaPvP {
    private:
        Personagem&      p1;
        Personagem&      p2;
        list<LogBatalha> log;
        int              turnoAtual;

        void registrar(const string &ator, const string &msg) {
            log.push_back({turnoAtual, ator, msg});
        }

        void imprimirSeparador() const {
            cout << "──────────────────────────────────────────" << endl;
        }

        int lerInt() {
            int v;
            while(!(cin >> v)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "> ";
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }

        void menuAcao(Personagem &atacante, Personagem &alvo) {
            cout << "\n[TURNO " << turnoAtual << "] "
                 << atacante.getNome() << " (" << atacante.getHPAtual() << " HP) — escolha:\n"
                 << "  1. Atacar\n"
                 << "  2. Usar Habilidade\n"
                 << "  3. Usar Estus\n"
                 << "> ";

            switch(lerInt()) {
                case 1:
                    atacante.atacar(alvo);
                    registrar(atacante.getNome(), "atacou " + alvo.getNome());
                    break;
                case 2: {
                    const auto& habs = atacante.getHabilidades();
                    if(habs.empty()) { cout << "Sem habilidades.\n"; break; }
                    cout << "Habilidades:\n";
                    for(int i = 0; i < (int)habs.size(); i++)
                        cout << "  " << (i+1) << ". " << *habs[i] << "\n";
                    cout << "> ";
                    int idx = lerInt() - 1;
                    atacante.usarHabilidade(idx, alvo);
                    registrar(atacante.getNome(), "usou habilidade contra " + alvo.getNome());
                    break;
                }
                case 3:
                    atacante.usarEstus();
                    registrar(atacante.getNome(), "usou Estus Flask");
                    break;
                default:
                    atacante.atacar(alvo);
                    registrar(atacante.getNome(), "atacou (padrão) " + alvo.getNome());
            }
            atacante.recuperarStamina(15);
            atacante.reduzirCooldowns();
        }

    public:
        BatalhaPvP(Personagem &a, Personagem &b)
            : p1(a), p2(b), turnoAtual(0) {}

        /**
         * @brief Executa batalha PvP por turnos.
         * @return true se p1 venceu.
         */
        bool executar() {
            cout << "\n╔══════════════════════════════════════════╗\n"
                 << "║          BATALHA PvP!                    ║\n"
                 << "╚══════════════════════════════════════════╝\n";
            cout << p1 << "\nvs\n" << p2 << "\n";
            imprimirSeparador();

            while(p1.estaVivo() && p2.estaVivo()) {
                turnoAtual++;
                menuAcao(p1, p2);
                if(!p2.estaVivo()) break;
                imprimirSeparador();
                menuAcao(p2, p1);
                cout << "\n» " << p1 << "\n» " << p2 << "\n";
                imprimirSeparador();
            }

            cout << "\n╔══════════════════════════════════════════╗\n";
            Personagem* vencedor = p1.estaVivo() ? &p1 : &p2;
            Personagem* perdedor = p1.estaVivo() ? &p2 : &p1;
            cout << "║  VENCEDOR: " << vencedor->getNome() << "!\n";
            cout << "╚══════════════════════════════════════════╝\n";

            int expGanha = perdedor->getNivel() * 300;
            cout << "  " << vencedor->getNome() << " ganhou " << expGanha << " de experiência!\n";
            vencedor->ganharExp(expGanha);
            vencedor->recarregarEstus();
            if(!perdedor->estaVivo()) perdedor->curar(1);

            return p1.estaVivo();
        }

        void exibirLog() const {
            cout << "\n=== LOG DA BATALHA PvP ===\n";
            for(const auto &e : log)
                cout << "[Turno " << e.turno << "] " << e.ator << ": " << e.descricao << "\n";
        }
};

} // namespace RPG
#endif
