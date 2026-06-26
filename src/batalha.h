/**
 * @file batalha.h
 * @brief Sistema de batalha por turnos com log de ações.
 */
#ifndef BATALHA_H
#define BATALHA_H

#include "personagem.h"
#include "inimigo.h"
#include <vector>
#include <string>
using namespace std;

namespace RPG {

/** @brief Registro de uma ação num turno de batalha. */
struct LogBatalha {
    int    turno;
    string descricao;
};

/**
 * @brief Gerencia o loop de combate por turnos entre Personagem e Inimigo.
 * Implementa padrão Observer para notificações de eventos (level up, morte, etc.).
 */
class Batalha {
    private:
        Personagem&        jogador;
        Inimigo&           inimigo;
        vector<LogBatalha> log;
        int                turnoAtual;
        bool               encerrada;

        void registrar(const string &msg) {
            log.push_back({turnoAtual, msg});
        }

        void imprimirSeparador() const {
            cout << "──────────────────────────────────────────" << endl;
        }

        // ── ações do jogador ──────────────────────────────────────────────────
        void menuAcaoJogador() {
            cout << "\n[TURNO " << turnoAtual << "] " << jogador.getNome() << " — escolha uma ação:\n"
                 << "  1. Atacar\n"
                 << "  2. Usar Habilidade\n"
                 << "  3. Usar Estus\n"
                 << "  4. Usar item do inventário\n"
                 << "> ";

            int opcao;
            cin >> opcao;

            switch(opcao) {
                case 1:
                    jogador.atacar(inimigo);
                    registrar(jogador.getNome() + " atacou " + inimigo.getNome());
                    break;

                case 2:
                    menuHabilidade();
                    break;

                case 3:
                    jogador.usarEstus();
                    registrar(jogador.getNome() + " usou Estus Flask");
                    break;

                case 4:
                    menuItem();
                    break;

                default:
                    cout << "Ação inválida — atacando por padrão." << endl;
                    jogador.atacar(inimigo);
            }
        }

        void menuHabilidade() {
            const auto& habs = jogador.getHabilidades();
            if(habs.empty()) {
                cout << "Sem habilidades disponíveis." << endl;
                return;
            }
            cout << "Habilidades:\n";
            for(int i = 0; i < (int)habs.size(); i++) {
                cout << "  " << (i+1) << ". " << *habs[i] << "\n";
            }
            cout << "> ";
            int idx; cin >> idx; idx--;
            jogador.usarHabilidade(idx, inimigo);
            registrar(jogador.getNome() + " usou habilidade");
        }

        void menuItem() {
            Pocao* p = jogador.getInventario().getPocao();
            if(p && p->temCargas()) {
                p->usar();
                jogador.curar(p->getCura());
                cout << jogador.getNome() << " usou " << p->getNome()
                     << " — +" << p->getCura() << " HP!" << endl;
                registrar(jogador.getNome() + " usou " + p->getNome());
            } else {
                cout << "Nenhum consumível disponível." << endl;
            }
        }

    public:
        Batalha(Personagem &j, Inimigo &i)
            : jogador(j), inimigo(i), turnoAtual(0), encerrada(false) {}

        /**
         * @brief Executa o loop de batalha por turnos até um dos lados morrer.
         * @return true se o jogador venceu.
         */
        bool executar() {
            cout << "\n╔══════════════════════════════════════════╗\n"
                 << "║          BATALHA INICIADA!               ║\n"
                 << "╚══════════════════════════════════════════╝\n";
            cout << jogador << " vs " << inimigo << "\n";
            imprimirSeparador();

            while(jogador.estaVivo() && inimigo.estaVivo()) {
                turnoAtual++;

                // turno do jogador
                menuAcaoJogador();
                jogador.recuperarStamina(25);
                jogador.reduzirCooldowns();

                if(!inimigo.estaVivo()) break;

                // turno do inimigo
                imprimirSeparador();
                inimigo.atacar(jogador);
                registrar(inimigo.getNome() + " atacou " + jogador.getNome());

                // status resumido
                cout << "\n» " << jogador << "\n» " << inimigo << "\n";
                imprimirSeparador();
            }

            return resolverFim();
        }

        bool resolverFim() {
            encerrada = true;
            cout << "\n╔══════════════════════════════════════════╗\n";
            if(jogador.estaVivo()) {
                cout << "║  VITÓRIA! " << jogador.getNome() << " sobreviveu!       \n";
                cout << "╚══════════════════════════════════════════╝\n";
                jogador.ganharExp(inimigo.getExpRecompensa());
                cout << "  Almas obtidas: " << inimigo.getAlmasRecompensa() << "\n";
                jogador.recarregarEstus();
                return true;
            } else {
                cout << "║  DERROTA — " << jogador.getNome() << " foi abatido!    \n";
                cout << "╚══════════════════════════════════════════╝\n";
                cout << "  \"YOU DIED\"\n";
                return false;
            }
        }

        void exibirLog() const {
            cout << "\n=== LOG DA BATALHA ===\n";
            for(const auto &e : log) {
                cout << "[Turno " << e.turno << "] " << e.descricao << "\n";
            }
        }
};

} // namespace RPG
#endif