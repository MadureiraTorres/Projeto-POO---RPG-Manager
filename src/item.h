/**
 * @file item.h
 * @brief Hierarquia de itens do RPG Manager — Dark Souls edition.
 *
 * Melhorias:
 * - operator+ implementado (crafting básico)
 * - operator[] no Inventario (via item.h indireto)
 * - operator+= para Inventario
 * - operator! para verificar uso
 */
#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <string>
using namespace std;

namespace RPG {

/**
 * @brief Classe base abstrata para todos os itens.
 * @note Usa padrão Template Method em descrever().
 */
class Item {
    protected:
        string nome;
        string descricao;
        int    peso;
        int    valor;

    public:
        /**
         * @brief Constrói um Item base.
         * @param nm    Nome do item.
         * @param desc  Descrição de lore.
         * @param peso  Peso em unidades de carga.
         * @param valor Valor em almas.
         */
        Item(const string &nm, const string &desc, int peso, int valor)
            : nome(nm), descricao(desc), peso(peso), valor(valor) {}

        virtual ~Item() = default;

        string getNome()      const { return nome; }
        string getDescricao() const { return descricao; }
        int    getPeso()      const { return peso; }
        int    getValor()     const { return valor; }

        virtual void   descrever() const = 0;
        virtual string getTipo()   const = 0;

        /// @brief Compara valor monetário de dois itens.
        bool operator< (const Item &o) const { return valor < o.valor; }
        /// @brief Igualdade por nome.
        bool operator==(const Item &o) const { return nome == o.nome; }
        /// @brief Verifica se o item ainda tem uso disponível (pode ser sobrescrito).
        virtual bool operator!() const { return false; }

        /**
         * @brief Tenta combinar dois itens (crafting stub).
         * @return nullptr — subclasses podem sobrescrever para crafting real.
         */
        virtual Item* operator+(const Item &) const { return nullptr; }

        friend ostream &operator<<(ostream &os, const Item &i) {
            os << "[" << i.getTipo() << "] " << i.nome
               << " | Peso: " << i.peso << " | Valor: " << i.valor << " almas";
            return os;
        }
};

// ── Arma ─────────────────────────────────────────────────────────────────────

/// @brief Arma equipável. Calcula dano com escalonamento de atributos.
class Arma : public Item {
    private:
        int    danoBase;
        int    escalForca;
        int    escalDex;
        string tipoArma;

    public:
        Arma(const string &nm, const string &desc, int peso, int valor,
             int dano, int esc_forca, int esc_dex, const string &tipo)
            : Item(nm, desc, peso, valor),
              danoBase(dano), escalForca(esc_forca), escalDex(esc_dex), tipoArma(tipo) {}

        int    getDanoBase()   const { return danoBase; }
        int    getEscalForca() const { return escalForca; }
        int    getEscalDex()   const { return escalDex; }
        string getTipoArma()   const { return tipoArma; }
        string getTipo()       const override { return "Arma"; }

        /**
         * @brief Calcula dano final considerando atributos do portador.
         * @param forca Atributo força do portador.
         * @param dex   Atributo destreza do portador.
         * @return Dano total.
         */
        int calcularDano(int forca, int dex) const {
            return danoBase + (forca * escalForca / 10) + (dex * escalDex / 10);
        }

        void descrever() const override {
            cout << *this << "\n"
                 << "  Tipo: " << tipoArma << " | Dano base: " << danoBase
                 << " | Esc. Força: " << escalForca
                 << " | Esc. Dex: "   << escalDex << "\n"
                 << "  \"" << descricao << "\"" << endl;
        }
};

// ── Armadura ─────────────────────────────────────────────────────────────────

/// @brief Armadura equipável em slot específico (capacete, peitoral, calças, luvas).
class Armadura : public Item {
    private:
        int    defesaFisica;
        int    defesaMagica;
        int    defesaFogo;
        string slot;

    public:
        Armadura(const string &nm, const string &desc, int peso, int valor,
                 int def_fis, int def_mag, int def_fogo, const string &slot)
            : Item(nm, desc, peso, valor),
              defesaFisica(def_fis), defesaMagica(def_mag),
              defesaFogo(def_fogo), slot(slot) {}

        int    getDefesaFisica() const { return defesaFisica; }
        int    getDefesaMagica() const { return defesaMagica; }
        int    getDefesaFogo()   const { return defesaFogo; }
        string getSlot()         const { return slot; }
        string getTipo()         const override { return "Armadura"; }

        void descrever() const override {
            cout << *this << "\n"
                 << "  Slot: " << slot
                 << " | Def. Física: "  << defesaFisica
                 << " | Def. Mágica: "  << defesaMagica
                 << " | Def. Fogo: "    << defesaFogo << "\n"
                 << "  \"" << descricao << "\"" << endl;
        }
};

// ── Poção ─────────────────────────────────────────────────────────────────────

/// @brief Consumível com cargas limitadas (Estus Flask e similares).
class Pocao : public Item {
    private:
        int cura;
        int cargasMax;
        int cargasAtuais;

    public:
        Pocao(const string &nm, const string &desc, int peso, int valor,
              int cura, int cargas)
            : Item(nm, desc, peso, valor),
              cura(cura), cargasMax(cargas), cargasAtuais(cargas) {}

        int  getCura()   const { return cura; }
        int  getCargas() const { return cargasAtuais; }
        bool temCargas() const { return cargasAtuais > 0; }
        string getTipo() const override { return "Consumível"; }

        /// @brief operator! retorna true se sem cargas (esgotado).
        bool operator!() const override { return cargasAtuais == 0; }

        /**
         * @brief Consome uma carga da poção.
         * @throws runtime_error Se não houver cargas disponíveis.
         */
        void usar() {
            if(!temCargas()) throw runtime_error("Sem cargas: " + nome);
            cargasAtuais--;
        }

        void recarregar() { cargasAtuais = cargasMax; }

        void descrever() const override {
            cout << *this << "\n"
                 << "  Cura: " << cura
                 << " HP | Cargas: " << cargasAtuais << "/" << cargasMax << "\n"
                 << "  \"" << descricao << "\"" << endl;
        }
};

// ── ItemEspecial ──────────────────────────────────────────────────────────────

/// @brief Item de uso único com efeito especial de lore.
class ItemEspecial : public Item {
    private:
        string efeito;
        bool   usado;

    public:
        ItemEspecial(const string &nm, const string &desc, int peso, int valor,
                     const string &ef)
            : Item(nm, desc, peso, valor), efeito(ef), usado(false) {}

        string getEfeito() const { return efeito; }
        bool   foiUsado()  const { return usado; }
        string getTipo()   const override { return "Especial"; }

        /// @brief operator! retorna true se o item já foi consumido.
        bool operator!() const override { return usado; }

        /**
         * @brief Ativa o item especial (uso único).
         * @throws runtime_error Se o item já foi usado anteriormente.
         */
        void ativar() {
            if(usado) throw runtime_error(nome + " já foi usado.");
            usado = true;
            cout << "✦ " << nome << " ativado! Efeito: " << efeito << endl;
        }

        void descrever() const override {
            cout << *this << "\n"
                 << "  Efeito: " << efeito
                 << " | Status: " << (usado ? "Usado" : "Disponível") << "\n"
                 << "  \"" << descricao << "\"" << endl;
        }
};

} // namespace RPG
#endif
