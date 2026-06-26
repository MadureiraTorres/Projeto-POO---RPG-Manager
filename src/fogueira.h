/**
 * @file fogueira.h
 * @brief Sistema de Fogueira — ponto de descanso e melhoria de atributos.
 *
 * Inspirado em Dark Souls: sentar na fogueira recupera HP, Estus e
 * permite gastar almas (exp) para melhorar atributos escolhidos pelo jogador.
 *
 * Custo de level up: nivel_atual * 500 almas (mesma fórmula do jogo base).
 */
#ifndef FOGUEIRA_H
#define FOGUEIRA_H

#include "personagem.h"
#include <limits>
using namespace std;

namespace RPG {

/**
 * @brief Gerencia a interação do jogador com a Fogueira.
 *
 * Funcionalidades:
 * - Recuperar HP e Estus completamente
 * - Gastar exp para subir nível escolhendo o atributo a aumentar
 */
class Fogueira {
    private:
        Personagem& jogador;

        int lerInt(const string &prompt = "> ") {
            int v;
            cout << prompt;
            while(!(cin >> v)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Entrada inválida. " << prompt;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }

        /// @brief Exibe custo de level up e exp disponível.
        void exibirCusto() const {
            cout << "\n  EXP atual:          " << jogador.getExp()
                 << "\n  EXP para próx. nív: " << jogador.getExpProxNivel()
                 << "\n  Nível atual:        " << jogador.getNivel() << "\n";
        }

        /**
         * @brief Menu de escolha de atributo para investir no level up.
         * Gasta exp igual ao expProxNivel e aumenta o atributo escolhido +3.
         * O restante do level up (HP, stamina, etc.) é feito normalmente.
         */
        void menuLevelUp() {
            if(jogador.getExp() < jogador.getExpProxNivel()) {
                cout << "\n  Almas insuficientes para subir de nível.\n";
                cout << "  Faltam: " << (jogador.getExpProxNivel() - jogador.getExp()) << " almas.\n";
                return;
            }

            cout << "\n╔═════════════════════════════════════════╗\n"
                 << "║  OFERECER ALMAS À FOGUEIRA              ║\n"
                 << "╚═════════════════════════════════════════╝\n";
            exibirCusto();

            cout << "\n  Escolha o atributo principal a fortalecer (+3 além do bônus base):\n"
                 << "  1. Força      (atual: " << jogador.getForca() << ")\n"
                 << "  2. Destreza   (atual: " << jogador.getDex() << ")\n"
                 << "  3. Inteligência (atual: " << jogador.getInteligencia() << ")\n"
                 << "  4. Fé         (atual: " << jogador.getFe() << ")\n"
                 << "  5. Vitalidade (aumenta HP máximo +75)\n"
                 << "  0. Cancelar\n";

            int escolha = lerInt("> ");
            if(escolha == 0) return;
            if(escolha < 1 || escolha > 5) {
                cout << "  Escolha inválida.\n";
                return;
            }

            // Debita exp e dispara o level up real (polimórfico, respeita bônus da subclasse)
            int custoAtual = jogador.getExpProxNivel();
            jogador.setExp(jogador.getExp() - custoAtual);
            jogador.levelUp();  // aplica bônus base da classe (+força, +dex, +HP, stamina, etc.)

            // Aplica APENAS o bônus extra escolhido pelo jogador (+3 no atributo desejado)
            switch(escolha) {
                case 1:
                    jogador.setForca(jogador.getForca() + 3);
                    cout << "\n  ✦ Força aumentada! (" << jogador.getForca() << ")\n";
                    break;
                case 2:
                    jogador.setDex(jogador.getDex() + 3);
                    cout << "\n  ✦ Destreza aumentada! (" << jogador.getDex() << ")\n";
                    break;
                case 3:
                    jogador.setInteligencia(jogador.getInteligencia() + 3);
                    cout << "\n  ✦ Inteligência aumentada! (" << jogador.getInteligencia() << ")\n";
                    break;
                case 4:
                    jogador.setFe(jogador.getFe() + 3);
                    cout << "\n  ✦ Fé aumentada! (" << jogador.getFe() << ")\n";
                    break;
                case 5:
                    jogador.aumentarHPMax(75);
                    cout << "\n  ✦ Vitalidade aumentada! HP Máx: " << jogador.getHPMax() << "\n";
                    break;
            }

            // Perguntar se quer continuar investindo
            if(jogador.getExp() >= jogador.getExpProxNivel()) {
                cout << "\n  Você ainda tem almas suficientes! Deseja continuar? (1=Sim / 0=Não)\n";
                if(lerInt("> ") == 1) menuLevelUp();
            }
        }

    public:
        /// @brief Constrói fogueira associada a um personagem.
        explicit Fogueira(Personagem &p) : jogador(p) {}

        /**
         * @brief Exibe o menu principal da fogueira e executa a ação escolhida.
         *
         * Opções:
         * - 1: Descansar (recupera HP e Estus)
         * - 2: Oferecer almas (level up com escolha de atributo)
         * - 3: Ver status
         * - 0: Levantar e seguir em frente
         */
        void sentar() {
            cout << "\n╔═════════════════════════════════════════╗\n"
                 << "║  ... " << jogador.getNome() << " senta à Fogueira ...\n"
                 << "╚═════════════════════════════════════════╝\n"
                 << "  A chama crepita. A escuridão recua por um momento.\n";

            bool na_fogueira = true;
            while(na_fogueira) {
                cout << "\n  O que fazer?\n"
                     << "  1. Descansar   (recupera HP e Estus completamente)\n"
                     << "  2. Oferecer almas à chama  (level up — escolher atributo)\n"
                     << "  3. Ver status\n"
                     << "  0. Levantar e seguir em frente\n";

                switch(lerInt("> ")) {
                    case 1:
                        jogador.setHPAtual(jogador.getHPMax());
                        jogador.recarregarEstus();
                        cout << "\n  HP e Estus completamente recuperados.\n"
                             << "  \"Que a Chama não se apague.\"\n";
                        break;
                    case 2:
                        menuLevelUp();
                        break;
                    case 3:
                        jogador.exibirStatus();
                        break;
                    case 0:
                        cout << "\n  " << jogador.getNome()
                             << " se levanta e enfrenta as trevas novamente.\n";
                        na_fogueira = false;
                        break;
                    default:
                        cout << "  Opção inválida.\n";
                }
            }
        }
};

} // namespace RPG
#endif