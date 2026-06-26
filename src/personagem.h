/**
 * @file personagem.h
 * @brief Classe Personagem: herda Entidade, possui Inventario e Habilidades.
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

        // CORRIGIDO: protegido para subclasses (Clerigo) acessarem
        int estusCargasMax;
        int estusCargas;
        int estusRecuperacao;

        // raça — Personagem é dono (composição via ponteiro)
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

        virtual ~Personagem() {
            for(Habilidade* h : habilidades) delete h;
            delete raca;  // CORRIGIDO: Personagem é dono da raça
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
        int    getEstusCargasMax() const { return estusCargasMax; }
        Raca*  getRaca()         const { return raca; }
        Inventario& getInventario()    { return inventario; }
        const Inventario& getInventario() const { return inventario; }
        const vector<Habilidade*>& getHabilidades() const { return habilidades; }

        // ── setters para carregamento de save ──────────────────────────────────
        void setForca(int v)        { forca = v; }
        void setDex(int v)          { dex = v; }
        void setInteligencia(int v) { inteligencia = v; }
        void setFe(int v)           { fe = v; }
        void setExp(int v)          { exp = v; }
        void setExpProxNivel(int v) { expProxNivel = v; }
        void setEstusCargas(int v)  { estusCargas = max(0, min(estusCargasMax, v)); }

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
            cout << getNome() << " ganhou " << qt << " almas de experiência! "
                 << "(" << exp << "/" << expProxNivel
                 << " — descanse na fogueira para subir de nível)" << endl;
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
                dano = arma->calcularDano(forca, dex) - alvo.getHPMax() / 20;
                dano = max(1, dano);
                cout << getNome() << " ataca com " << arma->getNome()
                     << " causando " << dano << " de dano em " << alvo.getNome() << "!" << endl;
            } else {
                dano = forca * 2;
                cout << getNome() << " ataca desarmado causando "
                     << dano << " de dano em " << alvo.getNome() << "!" << endl;
            }
            // reduz pelo equipamento do alvo (se for Personagem)
            Personagem* p = dynamic_cast<Personagem*>(&alvo);
            if(p) dano = max(1, dano - p->getInventario().getDefesaTotal() / 5);
            alvo.receberDano(dano);
        }

        // ── exibição ──────────────────────────────────────────────────────────
        void exibirStatus() const override {
            cout << "┌─────────────────────────────────────────┐\n"
                 << "│ " << nome << " [" << raca->getNome() << "]\n"
                 << "│ Nível: "     << nivel
                 << " | Classe: "   << getClasse() << "\n"
                 << "│ HP:      "   << hpAtual << "/" << hpMax << "\n"
                 << "│ Stamina: "   << stamina << "/" << staminaMax << "\n"
                 << "│ Estus:   "   << estusCargas << "/" << estusCargasMax << "\n"
                 << "│ EXP:     "   << exp << "/" << expProxNivel << "\n"
                 << "│ Força:   "   << forca
                 << " | Dex: "      << dex
                 << " | Intel: "    << inteligencia
                 << " | Fé: "       << fe << "\n"
                 << "│ Equipamento:\n";
            inventario.exibirEquipamento();
            cout << "└─────────────────────────────────────────┘" << endl;
        }

        virtual string getClasse() const = 0;
};

} // namespace RPG
#endif