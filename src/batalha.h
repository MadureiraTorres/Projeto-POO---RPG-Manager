/**
 * @file batalha.h
 * @brief Sistema de batalha por turnos com log de ações.
 *
 * Classes:
 * - Batalha: combate Personagem vs Inimigo (PvE)
 * - BatalhaPvP: combate Personagem vs Personagem (PvP) — RF-5
 */
#ifndef BATALHA_H
#define BATALHA_H

#include "personagem.h"
#include "inimigo.h"
#include <vector>
#include <string>
#include <limits>
using namespace std;

namespace RPG {

/** @brief Registro de uma ação num turno de batalha. */
struct LogBatalha {
    int    turno;
    string ator;
    string descricao;
};

// ── Batalha PvE ───────────────────────────────────────────────────────────────

/**
 * @brief Gerencia o loop de combate por turnos entre Personagem e Inimigo.
 */
class Batalha {
    private:
        Personagem&        jogador;
        Inimigo&           inimigo;
        vector<LogBatalha> log;
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

        bool resolverFim() {
            cout << "\n╔══════════════════════════════════════════╗\n";
            if(jogador.estaVivo()) {
                cout << "║  VITÓRIA! " << jogador.getNome() << " sobreviveu!\n";
                cout << "╚══════════════════════════════════════════╝\n";
                jogador.ganharExp(inimigo.getExpRecompensa());
                cout << "  Almas obtidas: " << inimigo.getAlmasRecompensa() << "\n";
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
 * @brief RF-5: Combate por turnos entre dois Personagens controlados por humanos.
 * Cada jogador escolhe sua ação a cada turno.
 */
class BatalhaPvP {
    private:
        Personagem&        p1;
        Personagem&        p2;
        vector<LogBatalha> log;
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

        /**
         * @brief Processa ação de um personagem contra o outro.
         * @param atacante Quem age neste turno.
         * @param alvo     Quem recebe a ação.
         */
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
         * @return true se p1 (personagem ativo) venceu.
         */
        bool executar() {
            cout << "\n╔══════════════════════════════════════════╗\n"
                 << "║          BATALHA PvP!                    ║\n"
                 << "╚══════════════════════════════════════════╝\n";
            cout << p1 << "\nvs\n" << p2 << "\n";
            imprimirSeparador();

            while(p1.estaVivo() && p2.estaVivo()) {
                turnoAtual++;

                // turno de p1
                menuAcao(p1, p2);
                if(!p2.estaVivo()) break;

                imprimirSeparador();

                // turno de p2
                menuAcao(p2, p1);

                cout << "\n» " << p1 << "\n» " << p2 << "\n";
                imprimirSeparador();
            }

            // resultado
            cout << "\n╔══════════════════════════════════════════╗\n";
            Personagem* vencedor = p1.estaVivo() ? &p1 : &p2;
            Personagem* perdedor = p1.estaVivo() ? &p2 : &p1;
            cout << "║  VENCEDOR: " << vencedor->getNome() << "!\n";
            cout << "╚══════════════════════════════════════════╝\n";

            // exp mínima ao vencedor
            int expGanha = perdedor->getNivel() * 300;
            cout << "  " << vencedor->getNome() << " ganhou " << expGanha << " de experiência!\n";
            vencedor->ganharExp(expGanha);
            vencedor->recarregarEstus();
            // restaura HP do perdedor para 1 (está "derrotado", não morto em PvP)
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
