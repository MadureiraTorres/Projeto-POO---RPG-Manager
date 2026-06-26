/**
 * @file inimigo.h
 * @brief Inimigos de Dark Souls 1 com comportamento via padrão Strategy.
 */
#ifndef INIMIGO_H
#define INIMIGO_H

#include "entidade.h"
#include <cstdlib>
using namespace std;

namespace RPG {

// ── Strategy de comportamento de IA ──────────────────────────────────────────

class ComportamentoIA {
    public:
        virtual ~ComportamentoIA() = default;
        virtual void agir(Entidade &self, Entidade &alvo) = 0;
        virtual string getNome() const = 0;
};

/** @brief IA agressiva: sempre ataca. */
class IAgressiva : public ComportamentoIA {
    public:
        string getNome() const override { return "Agressiva"; }
        void agir(Entidade &self, Entidade &alvo) override {
            int dano = self.getNivel() * 15 + 20;
            cout << self.getNome() << " ataca furiosamente causando " << dano << " de dano!" << endl;
            alvo.receberDano(dano);
        }
};

/** @brief IA cautelosa: ataca com 60% de chance, caso contrário recua. */
class IACautelosa : public ComportamentoIA {
    public:
        string getNome() const override { return "Cautelosa"; }
        void agir(Entidade &self, Entidade &alvo) override {
            if(rand() % 10 < 6) {
                int dano = self.getNivel() * 10 + 15;
                cout << self.getNome() << " ataca cautelosamente causando " << dano << " de dano!" << endl;
                alvo.receberDano(dano);
            } else {
                cout << self.getNome() << " recua e aguarda uma abertura..." << endl;
            }
        }
};

/** @brief IA de Boss: alterna entre ataque pesado e especial. */
class IABoss : public ComportamentoIA {
    private:
        int turno = 0;

    public:
        string getNome() const override { return "Boss"; }
        void agir(Entidade &self, Entidade &alvo) override {
            turno++;
            if(turno % 3 == 0) {
                // ataque especial a cada 3 turnos
                int dano = self.getNivel() * 30 + 60;
                cout << "⚔  " << self.getNome()
                     << " desencadeia ATAQUE ESPECIAL causando " << dano << " de dano!" << endl;
                alvo.receberDano(dano);
            } else {
                int dano = self.getNivel() * 18 + 30;
                cout << self.getNome() << " golpeia com força causando " << dano << " de dano!" << endl;
                alvo.receberDano(dano);
            }
        }
};

// ── Inimigo base ─────────────────────────────────────────────────────────────

/**
 * @brief Inimigo com IA plugável via Strategy.
 */
class Inimigo : public Entidade {
    protected:
        int               expRecompensa;
        int               almasRecompensa;
        ComportamentoIA*  ia;

    public:
        Inimigo(const string &nm, int hp, int lv, int exp, int almas, ComportamentoIA* ia)
            : Entidade(nm, hp, lv), expRecompensa(exp), almasRecompensa(almas), ia(ia) {}

        virtual ~Inimigo() { delete ia; }

        int getExpRecompensa()   const { return expRecompensa; }
        int getAlmasRecompensa() const { return almasRecompensa; }

        void atacar(Entidade &alvo) override {
            if(!estaVivo() || !ia) return;
            ia->agir(*this, alvo);
        }

        void exibirStatus() const override {
            cout << "[ INIMIGO ] " << nome
                 << " | Nível " << nivel
                 << " | HP " << hpAtual << "/" << hpMax
                 << " | IA: " << ia->getNome() << endl;
        }
};

// ── Inimigos concretos de Dark Souls 1 ───────────────────────────────────────

class HollowSoldier : public Inimigo {
    public:
        HollowSoldier() : Inimigo("Soldado Oco", 180, 1, 200, 150, new IACautelosa()) {}
};

class BlackKnight : public Inimigo {
    public:
        BlackKnight() : Inimigo("Cavaleiro Negro", 450, 5, 800, 600, new IAgressiva()) {}
};

class PinwheelBoss : public Inimigo {
    public:
        PinwheelBoss() : Inimigo("Catavento (Boss)", 1200, 8, 2000, 1500, new IABoss()) {}
};

class GargoyleBoss : public Inimigo {
    public:
        GargoyleBoss() : Inimigo("Gárgula da Torre Bell (Boss)", 1800, 10, 3500, 2500, new IABoss()) {}
};

class OrnsSteinBoss : public Inimigo {
    public:
        OrnsSteinBoss() : Inimigo("Ornstein, o Caçador de Dragões (Boss)", 3500, 20, 20000, 15000, new IABoss()) {}
};

} // namespace RPG
#endif