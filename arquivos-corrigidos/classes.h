/**
 * @file classes.h
 * @brief Classes de personagem jogáveis inspiradas em Dark Souls 1.
 *
 * BUGS CORRIGIDOS:
 *  1. Piromante::exibirStatus() imprimia a linha de mana APÓS o fechamento
 *     da caixa "└──...┘", ficando visualmente fora do quadro de status.
 *     Corrigido: exibirStatus() sobrescrito para imprimir toda a caixa
 *     internamente, incluindo a linha de mana antes do rodapé.
 */
#ifndef CLASSES_H
#define CLASSES_H

#include "personagem.h"
using namespace std;

namespace RPG {

// ── Cavaleiro ────────────────────────────────────────────────────────────────

/**
 * @brief Cavaleiro — tanque, alta força e vitalidade.
 * Habilidade especial: Ataque Pesado.
 */
class Cavaleiro : public Personagem {
    public:
        Cavaleiro(const string &nm, Raca* raca)
            : Personagem(nm, 1, 220, 13, 11, 9, 9, 11, raca) {
            adicionarHabilidade(new AtaquePesado());
            adicionarHabilidade(new IronFlesh());
        }

        string getClasse() const override { return "Cavaleiro"; }

        void levelUp() override {
            Personagem::levelUp();
            forca      += 1;   // cavaleiros ganham força extra no level up
            aumentarHPMax(25); // e HP adicional
        }
};

// ── Piromante ─────────────────────────────────────────────────────────────────

/**
 * @brief Piromante — dano mágico, começa com piromância equipada.
 * Único que usa mana além de stamina.
 */
class Piromante : public Personagem {
    private:
        int mana;
        int manaMax;

    public:
        Piromante(const string &nm, Raca* raca)
            : Personagem(nm, 1, 120, 10, 9, 12, 8, 10, raca),
              mana(80), manaMax(80) {
            adicionarHabilidade(new Piromancia());
            adicionarHabilidade(new AlmaTorrencial());
        }

        string getClasse() const override { return "Piromante"; }

        int getMana()    const { return mana; }
        int getManaMax() const { return manaMax; }

        void levelUp() override {
            Personagem::levelUp();
            inteligencia += 1;
            manaMax      += 20;
            mana          = manaMax;
        }

        // FIX: mana era impresso DEPOIS do fechamento da caixa "└──┘"
        // Agora a caixa inteira é montada aqui, com mana dentro do quadro.
        void exibirStatus() const override {
            cout << "┌─────────────────────────────────────────┐\n"
                 << "│ " << nome << " [" << raca->getNome() << "]\n"
                 << "│ Nível: "     << nivel
                 << " | Classe: "  << getClasse() << "\n"
                 << "│ HP:      "   << hpAtual << "/" << hpMax << "\n"
                 << "│ Stamina: "   << stamina << "/" << staminaMax << "\n"
                 << "│ Mana:    "   << mana << "/" << manaMax << "\n"
                 << "│ Estus:   "   << estusCargas << "/" << estusCargasMax << "\n"
                 << "│ EXP:     "   << exp << "/" << expProxNivel << "\n"
                 << "│ Força:   "   << forca
                 << " | Dex: "     << dex
                 << " | Intel: "   << inteligencia
                 << " | Fé: "      << fe << "\n"
                 << "│ Equipamento:\n";
            inventario.exibirEquipamento();
            cout << "└─────────────────────────────────────────┘" << endl;
        }
};

// ── Ladino ────────────────────────────────────────────────────────────────────

/**
 * @brief Ladino — alta dex e velocidade. Golpes críticos devastadores.
 */
class Ladino : public Personagem {
    private:
        bool furtivo;

    public:
        Ladino(const string &nm, Raca* raca)
            : Personagem(nm, 1, 140, 9, 15, 10, 9, 9, raca), furtivo(false) {
            adicionarHabilidade(new PunhaladaCostas());
        }

        string getClasse() const override { return "Ladino"; }
        bool   isFurtivo() const { return furtivo; }
        void   ativarFurtivo() { furtivo = true; cout << getNome() << " entra em modo furtivo!" << endl; }

        void levelUp() override {
            Personagem::levelUp();
            dex += 1;
        }

        void atacar(Entidade &alvo) override {
            if(furtivo) {
                // golpe furtivo: dobra dano
                Arma* arma = inventario.getArmaEquipada();
                int dano = arma ? arma->calcularDano(forca, dex) * 2 : dex * 4;
                cout << getNome() << " emerge das sombras — GOLPE FURTIVO em "
                     << alvo.getNome() << " causando " << dano << " de dano!" << endl;
                alvo.receberDano(dano);
                furtivo = false;
            } else {
                Personagem::atacar(alvo);
            }
        }
};

// ── Clérigo ───────────────────────────────────────────────────────────────────

/**
 * @brief Clérigo — suporte, cura e milagres. Alta fé.
 */
class Clerigo : public Personagem {
    public:
        Clerigo(const string &nm, Raca* raca)
            : Personagem(nm, 1, 160, 11, 8, 9, 14, 10, raca) {
            adicionarHabilidade(new MiraculoCura());
            adicionarHabilidade(new AtaquePesado());
            estusCargasMax += 2;   // clérigos têm mais estus
            estusCargas     = estusCargasMax;
        }

        string getClasse() const override { return "Clérigo"; }

        void levelUp() override {
            Personagem::levelUp();
            fe               += 1;
            estusRecuperacao += 10;
        }
};

} // namespace RPG
#endif
