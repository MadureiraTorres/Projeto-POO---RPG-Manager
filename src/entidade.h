/**
 * @file entidade.h
 * @brief Classe abstrata raiz de todas as entidades do RPG Manager.
 */
#ifndef ENTIDADE_H
#define ENTIDADE_H

#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

namespace RPG {

/**
 * @brief Base abstrata para todo ser do jogo (personagens e inimigos).
 */
class Entidade {
    protected:
        string nome;
        int    hpAtual;
        int    hpMax;
        int    nivel;

    public:
        Entidade(const string &nm, int hp, int lv)
            : nome(nm), hpAtual(hp), hpMax(hp), nivel(lv) {
            if(hp <= 0)  throw invalid_argument("HP deve ser positivo");
            if(lv <= 0)  throw invalid_argument("Nível deve ser positivo");
        }

        virtual ~Entidade() = default;

        // getters
        string getNome()    const { return nome; }
        int    getHPAtual() const { return hpAtual; }
        int    getHPMax()   const { return hpMax; }
        int    getNivel()   const { return nivel; }

        // setters protegidos via métodos
        void setNome(const string &nm) { nome = nm; }
        void setNivel(int lv) {
            if(lv <= 0) throw invalid_argument("Nível deve ser positivo");
            nivel = lv;
        }

        bool estaVivo() const { return hpAtual > 0; }

        void receberDano(int dano) {
            if(dano <= 0 || !estaVivo()) return;
            hpAtual = max(0, hpAtual - dano);
        }

        void curar(int cura) {
            if(cura <= 0 || !estaVivo()) return;
            hpAtual = min(hpMax, hpAtual + cura);
        }

        void aumentarHPMax(int bonus) {
            hpMax   += bonus;
            hpAtual += bonus;
        }

        // interface virtual pura
        virtual void exibirStatus() const = 0;
        virtual void atacar(Entidade &alvo) = 0;

        // sobrecarga de operadores
        bool operator==(const Entidade &o) const { return nome == o.nome && nivel == o.nivel; }
        bool operator< (const Entidade &o) const { return nivel < o.nivel; }

        friend ostream &operator<<(ostream &os, const Entidade &e) {
            os << "[" << e.nome << " | Nível " << e.nivel
               << " | HP " << e.hpAtual << "/" << e.hpMax << "]";
            return os;
        }
};

} // namespace RPG
#endif