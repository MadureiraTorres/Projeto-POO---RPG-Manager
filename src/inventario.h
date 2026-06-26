/**
 * @file inventario.h
 * @brief Gerenciamento de inventário com composição de itens, peso e equipamentos.
 *
 * Melhorias:
 * - Usa std::map<string,Item*> para busca O(1) por nome
 * - operator[] para acesso direto por nome
 * - operator+= para adicionar item
 * - Mantém vector<Item*> para manter ordem de inserção (iteração no save)
 */
#ifndef INVENTARIO_H
#define INVENTARIO_H

#include "item.h"
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

namespace RPG {

/**
 * @brief Inventário com lista de itens, peso máximo e slots de equipamento.
 *
 * Composição: Personagem possui um Inventario.
 * Usa vector para manter ordem e map para busca rápida.
 */
class Inventario {
    private:
        vector<Item*>        itens;
        map<string, Item*>   indice;   ///< busca O(log n) por nome
        int                  pesoMax;
        int                  pesoAtual;

        Arma*     armaEquipada  = nullptr;
        Armadura* capaceteEquip = nullptr;
        Armadura* peitoralEquip = nullptr;
        Armadura* calcasEquip   = nullptr;
        Armadura* luvEquip      = nullptr;

    public:
        /// @brief Constrói inventário com peso máximo configurável.
        explicit Inventario(int pesoMax = 50) : pesoMax(pesoMax), pesoAtual(0) {}

        ~Inventario() {
            for(Item* i : itens) delete i;
        }

        // ── itens ─────────────────────────────────────────────────────────────

        /**
         * @brief Adiciona item ao inventário se houver peso disponível.
         * @param item Ponteiro para o item (inventário assume propriedade).
         * @return true se adicionado; false se sobrepeso ou nulo.
         */
        bool adicionarItem(Item* item) {
            if(!item) return false;
            if(pesoAtual + item->getPeso() > pesoMax) {
                cout << "Inventário cheio! Peso máximo: " << pesoMax << endl;
                return false;
            }
            itens.push_back(item);
            indice[item->getNome()] = item;
            pesoAtual += item->getPeso();
            return true;
        }

        /// @brief Alias operator+= para adicionarItem.
        Inventario& operator+=(Item* item) { adicionarItem(item); return *this; }

        /**
         * @brief Acessa item por nome via operador [].
         * @param nome Nome do item.
         * @return Ponteiro ao item ou nullptr se não encontrado.
         */
        Item* operator[](const string &nome) const {
            auto it = indice.find(nome);
            return (it != indice.end()) ? it->second : nullptr;
        }

        /**
         * @brief Remove e destrói item do inventário pelo nome.
         * @param nomeItem Nome do item a remover.
         * @return true se encontrado e removido.
         */
        bool removerItem(const string &nomeItem) {
            auto it = find_if(itens.begin(), itens.end(),
                [&](Item* i){ return i->getNome() == nomeItem; });
            if(it == itens.end()) return false;
            pesoAtual -= (*it)->getPeso();
            indice.erase(nomeItem);
            delete *it;
            itens.erase(it);
            return true;
        }

        /// @brief Busca item por nome. @return Ponteiro ou nullptr.
        Item* buscarItem(const string &nomeItem) const {
            return (*this)[nomeItem];
        }

        // ── equipar ───────────────────────────────────────────────────────────

        /**
         * @brief Equipa arma pelo nome.
         * @param nomeArma Nome da arma no inventário.
         * @return true se equipada com sucesso.
         */
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

        /**
         * @brief Equipa armadura no slot correspondente.
         * @param nomeArmadura Nome da armadura no inventário.
         * @return true se equipada com sucesso.
         */
        bool equiparArmadura(const string &nomeArmadura) {
            Item* item = buscarItem(nomeArmadura);
            if(!item || item->getTipo() != "Armadura") {
                cout << "Armadura não encontrada: " << nomeArmadura << endl;
                return false;
            }
            Armadura* arm = static_cast<Armadura*>(item);
            string slot = arm->getSlot();
            if(slot == "capacete")   capaceteEquip = arm;
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

        // ── getters ───────────────────────────────────────────────────────────

        Arma* getArmaEquipada() const { return armaEquipada; }

        /// @brief Soma defesa física de todas as peças equipadas.
        int getDefesaTotal() const {
            int def = 0;
            if(capaceteEquip)  def += capaceteEquip->getDefesaFisica();
            if(peitoralEquip)  def += peitoralEquip->getDefesaFisica();
            if(calcasEquip)    def += calcasEquip->getDefesaFisica();
            if(luvEquip)       def += luvEquip->getDefesaFisica();
            return def;
        }

        /// @brief Retorna primeira poção encontrada no inventário, ou nullptr.
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
        const map<string,Item*>& getIndice() const { return indice; }
};

} // namespace RPG
#endif
