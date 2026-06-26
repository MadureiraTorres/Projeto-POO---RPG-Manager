/**
 * @file personagem.h
 * @brief Classe Personagem: herda Entidade, possui Inventario e Habilidades.
 *
 * BUGS CORRIGIDOS:
 *  1. ~Personagem() não deletava Raca* → memory leak. Corrigido: delete raca adicionado.
 *  2. atacar(): usava alvo.getHPMax()/20 como penalidade de dano — sem sentido lógico
 *     (penalizava mais quanto maior o HP do inimigo, causando dano 1 em bosses).
 *     Corrigido: removida a subtração errônea de hpMax; a defesa do alvo já é
 *     aplicada corretamente logo abaixo via getDefesaTotal().
 */
#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include "entidade.h"
#include "raca.h"
#include "habilidade.h"
#include "inventario.h"
#include <vector>
#include <memory>
using namespace std;

namespace RPG {

/**
 * @brief Personagem jogável. Agrega Inventario (composição) e Habilidades.
 */
class Personagem : public Entidade {
    protected:
        // atributos base
        int forca;
        int dex;
        int inteligencia;
        int fe;
        int vitalidade;
        int stamina;
        int staminaMax;
        int exp;
        int expProxNivel;
        int estusCargasMax;
        int estusCargas;
        int estusRecuperacao;

        // raça (composição — Personagem é dono e faz delete)
        Raca* raca;

        // inventário (composição — Personagem é dono)
        Inventario inventario;

        // habilidades (composição)
        vector<Habilidade*> habilidades;

    public:
        Personagem(const string &nm, int lv, int hp,
                   int forca, int dex, int intel, int fe, int vital,
                   Raca* raca)
            : Entidade(nm, hp + raca->bonusHP(), lv),
              forca(forca + raca->bonusForca()),
              dex(dex   + raca->bonusDex()),
              inteligencia(intel + raca->bonusIntel()),
              fe(fe     + raca->bonusFe()),
              vitalidade(vital + raca->bonusVital()),
              stamina(100), staminaMax(100),
              exp(0), expProxNivel(lv * 500),
              estusCargasMax(5), estusCargas(5), estusRecuperacao(150),
              raca(raca), inventario(80) {}

        // FIX 1: destrutor agora libera Raca* (era memory leak)
        virtual ~Personagem() {
            delete raca;
            for(Habilidade* h : habilidades) delete h;
        }

        // ── getters ────────────────────────────────────────────────────────────
        int    getForca()        const { return forca; }
        int    getDex()          const { return dex; }
        int    getInteligencia() const { return inteligencia; }
        int    getFe()           const { return fe; }
        int    getVitalidade()   const { return vitalidade; }
        int    getStamina()      const { return stamina; }
        int    getStaminaMax()   const { return staminaMax; }
        int    getExp()          const { return exp; }
        int    getExpProxNivel() const { return expProxNivel; }
        int    getEstusCargas()  const { return estusCargas; }
        Raca*  getRaca()         const { return raca; }
        Inventario& getInventario()    { return inventario; }
        const Inventario& getInventario() const { return inventario; }
        const vector<Habilidade*>& getHabilidades() const { return habilidades; }

        // ── stamina ────────────────────────────────────────────────────────────
        bool gastarStamina(int custo) {
            if(stamina < custo) return false;
            stamina -= custo;
            return true;
        }

        void recuperarStamina(int qt = 20) {
            stamina = min(staminaMax, stamina + qt);
        }

        // ── estus ──────────────────────────────────────────────────────────────
        void usarEstus() {
            if(estusCargas <= 0) {
                cout << getNome() << " não tem mais Estus!" << endl;
                return;
            }
            estusCargas--;
            curar(estusRecuperacao);
            cout << getNome() << " usa Frasco Estus — +" << estusRecuperacao
                 << " HP! (" << estusCargas << " cargas restantes)" << endl;
        }

        void recarregarEstus() { estusCargas = estusCargasMax; }

        // ── habilidades ────────────────────────────────────────────────────────
        void adicionarHabilidade(Habilidade* h) { habilidades.push_back(h); }

        void usarHabilidade(int idx, Entidade &alvo) {
            if(idx < 0 || idx >= (int)habilidades.size()) {
                cout << "Habilidade inválida." << endl;
                return;
            }
            Habilidade* h = habilidades[idx];
            if(!h->podeUsar()) {
                cout << h->getNome() << " está em cooldown!" << endl;
                return;
            }
            if(!gastarStamina(h->getCustoStamina())) {
                cout << "Stamina insuficiente para " << h->getNome() << "!" << endl;
                return;
            }
            h->usar(*this, alvo);
        }

        void reduzirCooldowns() {
            for(Habilidade* h : habilidades) h->reduzirCooldown();
        }

        // ── experiência / level up ─────────────────────────────────────────────
        void ganharExp(int qt) {
            if(qt <= 0) return;
            exp += qt;
            cout << getNome() << " ganhou " << qt << " almas de experiência!" << endl;
            while(exp >= expProxNivel) {
                exp -= expProxNivel;
                levelUp();
            }
        }

        virtual void levelUp() {
            nivel++;
            expProxNivel = nivel * 500;
            forca       += 2;
            dex         += 2;
            staminaMax  += 10;
            stamina      = staminaMax;
            aumentarHPMax(50);
            cout << "══ LEVEL UP! " << getNome() << " chegou ao nível " << nivel << " ══" << endl;
        }

        // ── ataque base ────────────────────────────────────────────────────────
        void atacar(Entidade &alvo) override {
            if(!estaVivo()) return;
            Arma* arma = inventario.getArmaEquipada();
            int dano;
            if(arma) {
                // FIX 2: removida subtração errônea de alvo.getHPMax()/20
                // (antes causava dano=1 contra bosses com HP alto)
                dano = arma->calcularDano(forca, dex);
                dano = max(1, dano);
                cout << getNome() << " ataca com " << arma->getNome()
                     << " causando " << dano << " de dano em " << alvo.getNome() << "!" << endl;
            } else {
                dano = forca * 2;
                cout << getNome() << " ataca desarmado causando "
                     << dano << " de dano em " << alvo.getNome() << "!" << endl;
            }
            // reduz pela defesa do alvo (se for Personagem)
            Personagem* p = dynamic_cast<Personagem*>(&alvo);
            if(p) dano = max(1, dano - p->getInventario().getDefesaTotal() / 5);
            alvo.receberDano(dano);
        }

        // ── exibição ──────────────────────────────────────────────────────────
        void exibirStatus() const override {
            cout << "┌─────────────────────────────────────────┐\n"
                 << "│ " << nome << " [" << raca->getNome() << "]\n"
                 << "│ Nível: "     << nivel
                 << " | Classe: "  << getClasse() << "\n"
                 << "│ HP:      "   << hpAtual << "/" << hpMax << "\n"
                 << "│ Stamina: "   << stamina << "/" << staminaMax << "\n"
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

        virtual string getClasse() const = 0;
};

} // namespace RPG
#endif
