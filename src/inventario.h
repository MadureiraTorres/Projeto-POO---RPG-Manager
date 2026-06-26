/**
 * @file inventario.h
 * @brief Gerenciamento de inventário com composição de itens, peso e equipamentos.
 */
#ifndef INVENTARIO_H
#define INVENTARIO_H

#include "item.h"
#include <vector>
#include <algorithm>
using namespace std;

namespace RPG {

/**
 * @brief Inventário com lista de itens, peso máximo e slots de equipamento.
 * Composição: Personagem possui um Inventario.
 */
class Inventario {
    private:
        vector<Item*> itens;
        int           pesoMax;
        int           pesoAtual;

        // slots de equipamento ativos
        Arma*     armaEquipada    = nullptr;
        Armadura* capaceteEquip   = nullptr;
        Armadura* peitoralEquip   = nullptr;
        Armadura* calcasEquip     = nullptr;
        Armadura* luvEquip        = nullptr;

    public:
        explicit Inventario(int pesoMax = 50) : pesoMax(pesoMax), pesoAtual(0) {}

        ~Inventario() {
            for(Item* i : itens) delete i;
        }

        // ── itens ─────────────────────────────────────────────────────────────

        bool adicionarItem(Item* item) {
            if(!item) return false;
            if(pesoAtual + item->getPeso() > pesoMax) {
                cout << "Inventário cheio! Peso máximo: " << pesoMax << endl;
                return false;
            }
            itens.push_back(item);
            pesoAtual += item->getPeso();
            return true;
        }

        bool removerItem(const string &nomeItem) {
            auto it = find_if(itens.begin(), itens.end(),
                [&](Item* i){ return i->getNome() == nomeItem; });
            if(it == itens.end()) return false;
            pesoAtual -= (*it)->getPeso();
            delete *it;
            itens.erase(it);
            return true;
        }

        Item* buscarItem(const string &nomeItem) const {
            auto it = find_if(itens.begin(), itens.end(),
                [&](Item* i){ return i->getNome() == nomeItem; });
            return (it != itens.end()) ? *it : nullptr;
        }

        // ── equipar ───────────────────────────────────────────────────────────

        bool equiparArma(const string &nomeArma) {
            Item* item = buscarItem(nomeArma);
            if(!item || item->getTipo() != "Arma") {
                cout << "Arma não encontrada: " << nomeArma << endl;
                return false;
            }
            armaEquipada = static_cast<Arma*>(item);
            cout << "Arma equipada: " << armaEquipada->getNome() << endl;
            return true;
        }

        bool equiparArmadura(const string &nomeArmadura) {
            Item* item = buscarItem(nomeArmadura);
            if(!item || item->getTipo() != "Armadura") {
                cout << "Armadura não encontrada: " << nomeArmadura << endl;
                return false;
            }
            Armadura* arm = static_cast<Armadura*>(item);
            string slot = arm->getSlot();
            if(slot == "capacete")  capaceteEquip = arm;
            else if(slot == "peitoral") peitoralEquip = arm;
            else if(slot == "calças")   calcasEquip   = arm;
            else if(slot == "luvas")    luvEquip      = arm;
            cout << "Armadura equipada [" << slot << "]: " << arm->getNome() << endl;
            return true;
        }

        void desequiparArma() {
            if(armaEquipada) cout << "Desequipada: " << armaEquipada->getNome() << endl;
            armaEquipada = nullptr;
        }

        // ── getters de equipamento ─────────────────────────────────────────────

        Arma*     getArmaEquipada()  const { return armaEquipada; }
        int       getDefesaTotal()   const {
            int def = 0;
            if(capaceteEquip)  def += capaceteEquip->getDefesaFisica();
            if(peitoralEquip)  def += peitoralEquip->getDefesaFisica();
            if(calcasEquip)    def += calcasEquip->getDefesaFisica();
            if(luvEquip)       def += luvEquip->getDefesaFisica();
            return def;
        }

        // ── consumíveis ───────────────────────────────────────────────────────

        Pocao* getPocao() const {
            for(Item* i : itens)
                if(i->getTipo() == "Consumível") return static_cast<Pocao*>(i);
            return nullptr;
        }

        // ── exibição ──────────────────────────────────────────────────────────

        void listarItens() const {
            if(itens.empty()) { cout << "  (inventário vazio)" << endl; return; }
            cout << "  Peso: " << pesoAtual << "/" << pesoMax << endl;
            for(Item* i : itens) i->descrever();
        }

        void exibirEquipamento() const {
            cout << "  Arma:      " << (armaEquipada  ? armaEquipada->getNome()  : "—") << "\n"
                 << "  Capacete:  " << (capaceteEquip ? capaceteEquip->getNome() : "—") << "\n"
                 << "  Peitoral:  " << (peitoralEquip ? peitoralEquip->getNome() : "—") << "\n"
                 << "  Calças:    " << (calcasEquip   ? calcasEquip->getNome()   : "—") << "\n"
                 << "  Luvas:     " << (luvEquip      ? luvEquip->getNome()      : "—") << endl;
        }

        int getPesoAtual() const { return pesoAtual; }
        int getPesoMax()   const { return pesoMax; }
        const vector<Item*>& getItens() const { return itens; }
};

} // namespace RPG
#endif