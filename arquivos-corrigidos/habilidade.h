/**
 * @file habilidade.h
 * @brief Sistema de habilidades ativas e passivas inspiradas em Dark Souls 1.
 *
 * BUGS CORRIGIDOS:
 *  1. AlmaTorrencial::usar() não chamava resetarCooldown() após ser usada,
 *     tornando a habilidade reutilizável infinitamente sem cooldown.
 *     Corrigido: resetarCooldown() adicionado ao final de usar().
 */
#ifndef HABILIDADE_H
#define HABILIDADE_H

#include "entidade.h"
using namespace std;

namespace RPG {

/**
 * @brief Classe abstrata base para todas as habilidades.
 */
class Habilidade {
    protected:
        string nome;
        string descricao;
        int    custoStamina;
        int    cooldownMax;
        int    cooldownAtual;

    public:
        Habilidade(const string &nm, const string &desc, int stamina, int cd)
            : nome(nm), descricao(desc), custoStamina(stamina),
              cooldownMax(cd), cooldownAtual(0) {}

        virtual ~Habilidade() = default;

        string getNome()        const { return nome; }
        string getDescricao()   const { return descricao; }
        int    getCustoStamina()const { return custoStamina; }
        bool   podeUsar()       const { return cooldownAtual == 0; }

        void reduzirCooldown() { if(cooldownAtual > 0) cooldownAtual--; }
        void resetarCooldown() { cooldownAtual = cooldownMax; }

        virtual void usar(Entidade &usuario, Entidade &alvo) = 0;
        virtual string getTipo() const = 0;

        friend ostream &operator<<(ostream &os, const Habilidade &h) {
            os << "[" << h.getTipo() << "] " << h.nome << " — " << h.descricao
               << " (stamina: " << h.custoStamina << ", CD: " << h.cooldownMax << ")";
            return os;
        }
};

// ── Ofensivas ─────────────────────────────────────────────────────────────────

/** @brief Ataque pesado — alto dano, alto custo de stamina. */
class AtaquePesado : public Habilidade {
    public:
        AtaquePesado() : Habilidade(
            "Ataque Pesado",
            "Golpe poderoso com as duas mãos. Alto dano, abre guarda.",
            40, 1) {}

        string getTipo() const override { return "Ofensiva"; }

        void usar(Entidade &usuario, Entidade &alvo) override {
            int dano = usuario.getNivel() * 25 + 30;
            cout << usuario.getNome() << " desfere um ATAQUE PESADO em "
                 << alvo.getNome() << " causando " << dano << " de dano!" << endl;
            alvo.receberDano(dano);
            resetarCooldown();
        }
};

/** @brief Piromancia — dano de fogo baseado em inteligência (simulado via nível). */
class Piromancia : public Habilidade {
    public:
        Piromancia() : Habilidade(
            "Grande Bola de Fogo",
            "Projétil de fogo intenso. Causa dano em área.",
            35, 2) {}

        string getTipo() const override { return "Ofensiva"; }

        void usar(Entidade &usuario, Entidade &alvo) override {
            int dano = usuario.getNivel() * 20 + 40;
            cout << usuario.getNome() << " lança GRANDE BOLA DE FOGO em "
                 << alvo.getNome() << " causando " << dano << " de dano de fogo!" << endl;
            alvo.receberDano(dano);
            resetarCooldown();
        }
};

/** @brief Punhalada pelas costas — dano crítico se atacar primeiro no turno. */
class PunhaladaCostas : public Habilidade {
    public:
        PunhaladaCostas() : Habilidade(
            "Punhalada pelas Costas",
            "Crítico devastador ao posicionar atrás do inimigo.",
            20, 3) {}

        string getTipo() const override { return "Ofensiva"; }

        void usar(Entidade &usuario, Entidade &alvo) override {
            int dano = usuario.getNivel() * 35 + 20;
            cout << usuario.getNome() << " executa PUNHALADA PELAS COSTAS em "
                 << alvo.getNome() << " — CRÍTICO! " << dano << " de dano!" << endl;
            alvo.receberDano(dano);
            resetarCooldown();
        }
};

// ── Defensivas ────────────────────────────────────────────────────────────────

/** @brief Iron Flesh — aumenta defesa temporariamente, reduz mobilidade. */
class IronFlesh : public Habilidade {
    public:
        IronFlesh() : Habilidade(
            "Iron Flesh",
            "Piromancia defensiva: torna a carne em ferro por um turno.",
            25, 3) {}

        string getTipo() const override { return "Defensiva"; }

        void usar(Entidade &usuario, Entidade &alvo) override {
            (void)alvo;
            int cura = usuario.getNivel() * 10 + 50;
            cout << usuario.getNome() << " ativa IRON FLESH — absorve "
                 << cura << " de dano como escudo!" << endl;
            usuario.curar(cura);   // simula absorção de dano
            resetarCooldown();
        }
};

// ── Suporte ───────────────────────────────────────────────────────────────────

/** @brief Milagre de cura — restaura HP do usuário. */
class MiraculoCura : public Habilidade {
    public:
        MiraculoCura() : Habilidade(
            "Emissão de Luz",
            "Milagre sagrado que restaura pontos de vida.",
            15, 2) {}

        string getTipo() const override { return "Suporte"; }

        void usar(Entidade &usuario, Entidade &alvo) override {
            (void)alvo;
            int cura = usuario.getNivel() * 15 + 60;
            cout << usuario.getNome() << " conjura EMISSÃO DE LUZ — recupera "
                 << cura << " HP!" << endl;
            usuario.curar(cura);
            resetarCooldown();
        }
};

/** @brief Alma Torrencial — drena vida do inimigo para o usuário. */
class AlmaTorrencial : public Habilidade {
    public:
        AlmaTorrencial() : Habilidade(
            "Alma Torrencial",
            "Magia que drena a alma do inimigo, restaurando HP do usuário.",
            30, 3) {}

        string getTipo() const override { return "Suporte"; }

        void usar(Entidade &usuario, Entidade &alvo) override {
            int drain = usuario.getNivel() * 12 + 30;
            cout << usuario.getNome() << " usa ALMA TORRENCIAL em "
                 << alvo.getNome() << " — drena " << drain << " HP!" << endl;
            alvo.receberDano(drain);
            usuario.curar(drain / 2);
            // FIX: resetarCooldown() estava ausente — habilidade era gratuita infinitamente
            resetarCooldown();
        }
};

} // namespace RPG
#endif
