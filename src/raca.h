/**
 * @file raca.h
 * @brief Raças jogáveis inspiradas no lore de Dark Souls 1.
 */
#ifndef RACA_H
#define RACA_H

#include <string>
using namespace std;

namespace RPG {

/**
 * @brief Base para raças. Encapsula bônus de atributos e descrição de lore.
 */
class Raca {
    public:
        virtual ~Raca() = default;

        virtual string getNome()      const = 0;
        virtual string getLore()      const = 0;
        virtual int    bonusHP()      const = 0;
        virtual int    bonusForca()   const = 0;
        virtual int    bonusDex()     const = 0;
        virtual int    bonusIntel()   const = 0;
        virtual int    bonusFe()      const = 0;
        virtual int    bonusVital()   const = 0;
};

/** @brief Humanos: versáteis, sem bônus extremos. */
class Humano : public Raca {
    public:
        string getNome()    const override { return "Humano"; }
        string getLore()    const override {
            return "Descendentes da Chama Primordial, os humanos carregam "
                   "a Marca Escura — amaldiçoados e abençoados ao mesmo tempo.";
        }
        int bonusHP()     const override { return 50; }
        int bonusForca()  const override { return 1; }
        int bonusDex()    const override { return 1; }
        int bonusIntel()  const override { return 1; }
        int bonusFe()     const override { return 1; }
        int bonusVital()  const override { return 1; }
};

/** @brief Morto-Vivo: resistente, começa com estus extra. */
class MortoVivo : public Raca {
    public:
        string getNome()    const override { return "Morto-Vivo"; }
        string getLore()    const override {
            return "Marcados pela Marca Escura, os Mortos-Vivos ressurgem "
                   "eternamente. Perdem a sanidade a cada morte, mas ganham "
                   "resiliência incomparável.";
        }
        int bonusHP()     const override { return 100; }
        int bonusForca()  const override { return 2; }
        int bonusDex()    const override { return 0; }
        int bonusIntel()  const override { return 0; }
        int bonusFe()     const override { return 0; }
        int bonusVital()  const override { return 2; }
};

/** @brief Pygmy: fraco fisicamente, mas com inteligência e fé elevadas. */
class Pygmy : public Raca {
    public:
        string getNome()    const override { return "Pygmy"; }
        string getLore()    const override {
            return "Descendentes do Pigmeu, senhor da Alma Escura. "
                   "São frágeis de corpo, mas detentores de sabedoria arcana "
                   "e conexão profunda com os milagres.";
        }
        int bonusHP()     const override { return 0; }
        int bonusForca()  const override { return -1; }
        int bonusDex()    const override { return 1; }
        int bonusIntel()  const override { return 3; }
        int bonusFe()     const override { return 3; }
        int bonusVital()  const override { return 0; }
};

/** @brief Dragão Antigo: força e resistência brutais, mas sem magia. */
class DragaoAntigo : public Raca {
    public:
        string getNome()    const override { return "Dragão Antigo"; }
        string getLore()    const override {
            return "Remanescentes da era anterior à Chama, os Dragões Antigos "
                   "possuem escamas de pedra e resistência sobre-humana, "
                   "mas são impermeáveis às artes arcanas.";
        }
        int bonusHP()     const override { return 150; }
        int bonusForca()  const override { return 4; }
        int bonusDex()    const override { return 0; }
        int bonusIntel()  const override { return -3; }
        int bonusFe()     const override { return -3; }
        int bonusVital()  const override { return 3; }
};

} // namespace RPG
#endif