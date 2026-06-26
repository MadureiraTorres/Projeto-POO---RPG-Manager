/**
 * @file factories.h
 * @brief Factories para criação de itens e personagens.
 */
#ifndef FACTORIES_H
#define FACTORIES_H

#include "classes.h"
#include "inimigo.h"
#include <stdexcept>
using namespace std;

namespace RPG {

// ── ItemFactory ───────────────────────────────────────────────────────────────

/**
 * @brief Cria itens de Dark Souls 1 por tipo/nome.
 * Métodos estáticos — não requer instância.
 */
class ItemFactory {
    public:
        /**
         * @brief Cria arma pelo tipo.
         * Tipos: "espadaLonga", "machado", "alabarda", "adaga", "arcoLongo", "marteloGrande"
         */
        static Arma* criarArma(const string &tipo) {
            if(tipo == "espadaLonga")
                return new Arma("Espada Longa", "Espada padrão dos soldados de Astora.",
                                8, 1000, 80, 10, 10, "Espada");
            if(tipo == "machado")
                return new Arma("Machado de Guerra", "Machado pesado de dois gumes.",
                                12, 900, 95, 14, 4, "Machado");
            if(tipo == "alabarda")
                return new Arma("Alabarda", "Arma de haste com lâmina curva. Alcance imenso.",
                                14, 1200, 90, 12, 8, "Alabarda");
            if(tipo == "adaga")
                return new Arma("Adaga", "Lâmina curta ideal para golpes rápidos.",
                                3, 600, 55, 4, 14, "Adaga");
            if(tipo == "arcoLongo")
                return new Arma("Arco Longo", "Arco padrão de alcance médio.",
                                5, 800, 65, 4, 16, "Arco");
            if(tipo == "marteloGrande")
                return new Arma("Grande Martelo", "Martelo colossal. Esmaga tudo.",
                                20, 1500, 130, 18, 2, "Martelo");
            if(tipo == "katana")
                return new Arma("Katana Uchigatana", "Espada japonesa de Undead Merchant.",
                                6, 1800, 90, 8, 18, "Katana");
            throw invalid_argument("Arma desconhecida: " + tipo);
        }

        /**
         * @brief Cria armadura pelo tipo e slot.
         * Tipos: "cavaleiro", "elite", "feiticeiro", "ladino", "clerigo"
         */
        static Armadura* criarArmadura(const string &tipo, const string &slot) {
            if(tipo == "cavaleiro") {
                if(slot == "capacete")  return new Armadura("Elmo do Cavaleiro",  "Elmo pesado de aço.", 8, 600, 40, 15, 20, "capacete");
                if(slot == "peitoral") return new Armadura("Peitoral do Cavaleiro","Armadura pesada.",    18,1400, 80, 30, 40, "peitoral");
                if(slot == "calças")   return new Armadura("Calças do Cavaleiro",  "Grevas reforçadas.",  10, 800, 50, 20, 25, "calças");
                if(slot == "luvas")    return new Armadura("Luvas do Cavaleiro",   "Manoplas de aço.",    6,  500, 30, 15, 15, "luvas");
            }
            if(tipo == "elite") {
                if(slot == "capacete")  return new Armadura("Elmo de Elite",      "Elmo do Cavaleiro Negro.", 10, 900, 55, 20, 30, "capacete");
                if(slot == "peitoral") return new Armadura("Peitoral de Elite",   "Armadura de elite.",       22,1800,100, 40, 55, "peitoral");
                if(slot == "calças")   return new Armadura("Calças de Elite",     "Grevas de elite.",         12,1000, 65, 25, 35, "calças");
                if(slot == "luvas")    return new Armadura("Luvas de Elite",      "Manoplas de elite.",        8, 700, 40, 20, 20, "luvas");
            }
            if(tipo == "feiticeiro") {
                if(slot == "capacete")  return new Armadura("Capuz do Feiticeiro","Capuz leve.",     2, 400, 10, 35, 5, "capacete");
                if(slot == "peitoral") return new Armadura("Robe do Feiticeiro", "Robe encantado.", 5, 900, 15, 60,10, "peitoral");
                if(slot == "calças")   return new Armadura("Calças do Feiticeiro","Calças leves.",  3, 500, 10, 40, 5, "calças");
                if(slot == "luvas")    return new Armadura("Luvas do Feiticeiro","Luvas de seda.",  2, 350,  8, 30, 5, "luvas");
            }
            throw invalid_argument("Armadura desconhecida: " + tipo + "/" + slot);
        }

        /**
         * @brief Cria poção pelo tipo.
         * Tipos: "estus", "humanidade", "estusReforçado"
         */
        static Pocao* criarPocao(const string &tipo) {
            if(tipo == "estus")
                return new Pocao("Frasco Estus", "Bênção de Estus, a chama da vida.",
                                 1, 0, 150, 5);
            if(tipo == "estusReforcado")
                return new Pocao("Frasco Estus +5", "Frasco reforçado com fragmento de Estus.",
                                 1, 0, 400, 10);
            if(tipo == "humanidade")
                return new Pocao("Humanidade", "Restaura a forma humana e recupera HP.",
                                 1, 0, 999, 1);
            throw invalid_argument("Poção desconhecida: " + tipo);
        }

        /**
         * @brief Cria item especial.
         * Tipos: "anelVida", "anelForca", "prismaSoul"
         */
        static ItemEspecial* criarItemEspecial(const string &tipo) {
            if(tipo == "anelVida")
                return new ItemEspecial("Anel de Sacrifício",
                    "Anel que absorve uma morte fatal.", 0, 5000,
                    "Previne morte uma vez, depois se destrói.");
            if(tipo == "anelForca")
                return new ItemEspecial("Anel de Havel",
                    "Anel de pedra do grande Havel.", 0, 8000,
                    "+50% capacidade de carga de peso.");
            if(tipo == "prismaSoul")
                return new ItemEspecial("Pedra Prismática",
                    "Pedra que brilha ao detectar abismos.", 0, 100,
                    "Indica piso seguro emitindo luz.");
            throw invalid_argument("Item especial desconhecido: " + tipo);
        }
};

// ── PersonagemFactory ─────────────────────────────────────────────────────────

/**
 * @brief Cria personagens com classe e raça, equipando itens iniciais.
 */
class PersonagemFactory {
    public:
        static Cavaleiro* criarCavaleiro(const string &nome, const string &nomeRaca) {
            Raca* raca = criarRaca(nomeRaca);
            Cavaleiro* c = new Cavaleiro(nome, raca);
            // equipamento inicial
            c->getInventario().adicionarItem(ItemFactory::criarArma("espadaLonga"));
            c->getInventario().adicionarItem(ItemFactory::criarArmadura("cavaleiro", "capacete"));
            c->getInventario().adicionarItem(ItemFactory::criarArmadura("cavaleiro", "peitoral"));
            c->getInventario().adicionarItem(ItemFactory::criarArmadura("cavaleiro", "calças"));
            c->getInventario().adicionarItem(ItemFactory::criarArmadura("cavaleiro", "luvas"));
            c->getInventario().adicionarItem(ItemFactory::criarPocao("estus"));
            c->getInventario().equiparArma("Espada Longa");
            c->getInventario().equiparArmadura("Elmo do Cavaleiro");
            c->getInventario().equiparArmadura("Peitoral do Cavaleiro");
            c->getInventario().equiparArmadura("Calças do Cavaleiro");
            c->getInventario().equiparArmadura("Luvas do Cavaleiro");
            return c;
        }

        static Piromante* criarPiromante(const string &nome, const string &nomeRaca) {
            Raca* raca = criarRaca(nomeRaca);
            Piromante* p = new Piromante(nome, raca);
            p->getInventario().adicionarItem(ItemFactory::criarArma("adaga"));
            p->getInventario().adicionarItem(ItemFactory::criarArmadura("feiticeiro", "capacete"));
            p->getInventario().adicionarItem(ItemFactory::criarArmadura("feiticeiro", "peitoral"));
            p->getInventario().adicionarItem(ItemFactory::criarArmadura("feiticeiro", "calças"));
            p->getInventario().adicionarItem(ItemFactory::criarArmadura("feiticeiro", "luvas"));
            p->getInventario().adicionarItem(ItemFactory::criarPocao("estus"));
            p->getInventario().equiparArma("Adaga");
            p->getInventario().equiparArmadura("Capuz do Feiticeiro");
            p->getInventario().equiparArmadura("Robe do Feiticeiro");
            p->getInventario().equiparArmadura("Calças do Feiticeiro");
            p->getInventario().equiparArmadura("Luvas do Feiticeiro");
            return p;
        }

        static Ladino* criarLadino(const string &nome, const string &nomeRaca) {
            Raca* raca = criarRaca(nomeRaca);
            Ladino* l = new Ladino(nome, raca);
            l->getInventario().adicionarItem(ItemFactory::criarArma("katana"));
            l->getInventario().adicionarItem(ItemFactory::criarPocao("estus"));
            l->getInventario().equiparArma("Katana Uchigatana");
            return l;
        }

        static Clerigo* criarClerigo(const string &nome, const string &nomeRaca) {
            Raca* raca = criarRaca(nomeRaca);
            Clerigo* c = new Clerigo(nome, raca);
            c->getInventario().adicionarItem(ItemFactory::criarArma("machado"));
            c->getInventario().adicionarItem(ItemFactory::criarArmadura("cavaleiro", "peitoral"));
            c->getInventario().adicionarItem(ItemFactory::criarPocao("estusReforcado"));
            c->getInventario().equiparArma("Machado de Guerra");
            c->getInventario().equiparArmadura("Peitoral do Cavaleiro");
            return c;
        }

    private:
        static Raca* criarRaca(const string &nome) {
            if(nome == "Humano")       return new Humano();
            if(nome == "Morto-Vivo")   return new MortoVivo();
            if(nome == "Pygmy")        return new Pygmy();
            if(nome == "Dragão Antigo")return new DragaoAntigo();
            throw invalid_argument("Raça desconhecida: " + nome);
        }
};

// ── InimigoFactory ────────────────────────────────────────────────────────────

class InimigoFactory {
    public:
        static Inimigo* criarInimigo(const string &tipo) {
            if(tipo == "hollow")       return new HollowSoldier();
            if(tipo == "blackKnight")  return new BlackKnight();
            if(tipo == "pinwheel")     return new PinwheelBoss();
            if(tipo == "gargoyle")     return new GargoyleBoss();
            if(tipo == "ornstein")     return new OrnsSteinBoss();
            throw invalid_argument("Inimigo desconhecido: " + tipo);
        }
};

} // namespace RPG
#endif