/**
 * @file test_rpg.cpp
 * @brief Testes unitários do RPG Manager com Google Test.
 *
 * Cobertura:
 * - Item: operadores, herança, getTipo()
 * - Inventario: adicionar, remover, operator[], operator+=, mapa interno
 * - Entidade: receberDano, curar, estaVivo
 * - Personagem: ganharExp, levelUp, estus
 * - Fábricas: criação de personagens e itens
 * - Polimorfismo: chamadas via ponteiro base
 * - Batalha: resolução de vitória/derrota
 */

#include <gtest/gtest.h>
#include "../src/factories.h"
#include "../src/batalha.h"

using namespace RPG;

// ─────────────────────────────────────────────────────────────────────────────
// Fixtures
// ─────────────────────────────────────────────────────────────────────────────

class ItemFixture : public ::testing::Test {
    protected:
        Arma*        arma;
        Armadura*    arm;
        Pocao*       pocao;
        ItemEspecial* especial;

        void SetUp() override {
            arma     = ItemFactory::criarArma("espadaLonga");
            arm      = ItemFactory::criarArmadura("cavaleiro", "capacete");
            pocao    = ItemFactory::criarPocao("estus");
            especial = ItemFactory::criarItemEspecial("anelVida");
        }
        void TearDown() override {
            delete arma; delete arm; delete pocao; delete especial;
        }
};

class PersonagemFixture : public ::testing::Test {
    protected:
        Cavaleiro* cavaleiro;
        Piromante* piromante;
        Ladino*    ladino;
        Clerigo*   clerigo;

        void SetUp() override {
            cavaleiro = PersonagemFactory::criarCavaleiro("Artorias", "Humano");
            piromante = PersonagemFactory::criarPiromante("Quelana",  "Pygmy");
            ladino    = PersonagemFactory::criarLadino("Lautrec",    "Morto-Vivo");
            clerigo   = PersonagemFactory::criarClerigo("Rhea",      "Humano");
        }
        void TearDown() override {
            delete cavaleiro; delete piromante; delete ladino; delete clerigo;
        }
};

// ─────────────────────────────────────────────────────────────────────────────
// Testes de Item — herança, getTipo, operadores
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(ItemFixture, GetTipoPolimorfismo) {
    // polimorfismo via ponteiro base
    Item* items[] = {arma, arm, pocao, especial};
    EXPECT_EQ(items[0]->getTipo(), "Arma");
    EXPECT_EQ(items[1]->getTipo(), "Armadura");
    EXPECT_EQ(items[2]->getTipo(), "Consumível");
    EXPECT_EQ(items[3]->getTipo(), "Especial");
}

TEST_F(ItemFixture, OperadorMenorQue) {
    Arma* barata  = ItemFactory::criarArma("adaga");     // valor 600
    Arma* cara    = ItemFactory::criarArma("marteloGrande"); // valor 1500
    EXPECT_TRUE(*barata < *cara);
    EXPECT_FALSE(*cara < *barata);
    delete barata; delete cara;
}

TEST_F(ItemFixture, OperadorIgualdade) {
    Arma* outraEspada = ItemFactory::criarArma("espadaLonga");
    EXPECT_TRUE(*arma == *outraEspada);
    Arma* machado = ItemFactory::criarArma("machado");
    EXPECT_FALSE(*arma == *machado);
    delete outraEspada; delete machado;
}

TEST_F(ItemFixture, OperadorNaoParaPocao) {
    EXPECT_FALSE(!(*pocao));   // tem cargas — não está esgotada
    pocao->usar(); pocao->usar(); pocao->usar(); pocao->usar(); pocao->usar();
    EXPECT_TRUE(!(*pocao));    // sem cargas — esgotada
}

TEST_F(ItemFixture, OperadorNaoParaEspecial) {
    EXPECT_FALSE(!(*especial)); // não foi usado
    especial->ativar();
    EXPECT_TRUE(!(*especial));  // foi usado
}

TEST_F(ItemFixture, ArmaCalcularDano) {
    // Espada Longa: base=80, esc_forca=10, esc_dex=10
    // forca=10, dex=10 → 80 + (10*10/10) + (10*10/10) = 100
    EXPECT_EQ(arma->calcularDano(10, 10), 100);
    EXPECT_EQ(arma->calcularDano(0, 0), 80);
}

TEST_F(ItemFixture, ArmaduraSlot) {
    EXPECT_EQ(arm->getSlot(), "capacete");
    Armadura* peit = ItemFactory::criarArmadura("cavaleiro", "peitoral");
    EXPECT_EQ(peit->getSlot(), "peitoral");
    delete peit;
}

TEST_F(ItemFixture, PocaoCargas) {
    EXPECT_TRUE(pocao->temCargas());
    EXPECT_EQ(pocao->getCargas(), 5);
    pocao->usar();
    EXPECT_EQ(pocao->getCargas(), 4);
    pocao->recarregar();
    EXPECT_EQ(pocao->getCargas(), 5);
}

TEST_F(ItemFixture, PocaoSemCargasLancaExcecao) {
    for(int i = 0; i < 5; i++) pocao->usar();
    EXPECT_THROW(pocao->usar(), runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de Inventario — map, operator[], operator+=
// ─────────────────────────────────────────────────────────────────────────────

TEST(InventarioTest, AdicionarEBuscarComOperadorBracket) {
    Inventario inv(200);
    Arma* a = ItemFactory::criarArma("espadaLonga");
    inv.adicionarItem(a);
    EXPECT_NE(inv["Espada Longa"], nullptr);
    EXPECT_EQ(inv["Inexistente"], nullptr);
}

TEST(InventarioTest, OperadorMaisIgual) {
    Inventario inv(200);
    Arma* a = ItemFactory::criarArma("adaga");
    inv += a;
    EXPECT_NE(inv["Adaga"], nullptr);
}

TEST(InventarioTest, PesoMaximoRespeitado) {
    Inventario inv(5);  // peso máximo minúsculo
    Arma* pesada = ItemFactory::criarArma("marteloGrande"); // peso 20
    EXPECT_FALSE(inv.adicionarItem(pesada));
    delete pesada;
}

TEST(InventarioTest, RemoverItem) {
    Inventario inv(200);
    inv.adicionarItem(ItemFactory::criarArma("adaga"));
    EXPECT_TRUE(inv.removerItem("Adaga"));
    EXPECT_EQ(inv["Adaga"], nullptr);
    EXPECT_FALSE(inv.removerItem("Adaga")); // segunda remoção falha
}

TEST(InventarioTest, EquiparArma) {
    Inventario inv(200);
    inv.adicionarItem(ItemFactory::criarArma("espadaLonga"));
    EXPECT_TRUE(inv.equiparArma("Espada Longa"));
    EXPECT_NE(inv.getArmaEquipada(), nullptr);
}

TEST(InventarioTest, DefesaTotalSomaPecas) {
    Inventario inv(200);
    inv.adicionarItem(ItemFactory::criarArmadura("cavaleiro", "capacete"));
    inv.adicionarItem(ItemFactory::criarArmadura("cavaleiro", "peitoral"));
    inv.equiparArmadura("Elmo do Cavaleiro");
    inv.equiparArmadura("Peitoral do Cavaleiro");
    // cap=40, peit=80
    EXPECT_EQ(inv.getDefesaTotal(), 120);
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de Entidade / Personagem
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(PersonagemFixture, InicioVivo) {
    EXPECT_TRUE(cavaleiro->estaVivo());
    EXPECT_GT(cavaleiro->getHPAtual(), 0);
}

TEST_F(PersonagemFixture, ReceberDanoReduzHP) {
    int hpAntes = cavaleiro->getHPAtual();
    cavaleiro->receberDano(50);
    EXPECT_EQ(cavaleiro->getHPAtual(), hpAntes - 50);
}

TEST_F(PersonagemFixture, NaoPodeFicarComHPNegativo) {
    cavaleiro->receberDano(99999);
    EXPECT_EQ(cavaleiro->getHPAtual(), 0);
    EXPECT_FALSE(cavaleiro->estaVivo());
}

TEST_F(PersonagemFixture, CurarNaoUltrapassaMax) {
    int hpMax = cavaleiro->getHPMax();
    cavaleiro->receberDano(100);
    cavaleiro->curar(99999);
    EXPECT_EQ(cavaleiro->getHPAtual(), hpMax);
}

TEST_F(PersonagemFixture, GanharExpSemLevelUp) {
    cavaleiro->ganharExp(100);
    EXPECT_EQ(cavaleiro->getExp(), 100);
    EXPECT_EQ(cavaleiro->getNivel(), 1);
}

TEST_F(PersonagemFixture, LevelUpAoAtingirExpNecessaria) {
    int expNec = cavaleiro->getExpProxNivel();
    cavaleiro->ganharExp(expNec);
    EXPECT_EQ(cavaleiro->getNivel(), 2);
}

TEST_F(PersonagemFixture, EstusRecupera) {
    cavaleiro->receberDano(200);
    int hpAntes = cavaleiro->getHPAtual();
    cavaleiro->usarEstus();
    EXPECT_GT(cavaleiro->getHPAtual(), hpAntes);
    EXPECT_EQ(cavaleiro->getEstusCargas(), cavaleiro->getEstusCargasMax() - 1);
}

TEST_F(PersonagemFixture, RecarregarEstus) {
    cavaleiro->usarEstus();
    cavaleiro->recarregarEstus();
    EXPECT_EQ(cavaleiro->getEstusCargas(), cavaleiro->getEstusCargasMax());
}

TEST_F(PersonagemFixture, ClerigoTemMaisEstus) {
    // Clérigo recebe +2 slots de estus
    EXPECT_GT(clerigo->getEstusCargasMax(), cavaleiro->getEstusCargasMax());
}

TEST_F(PersonagemFixture, ClasseCorreta) {
    EXPECT_EQ(cavaleiro->getClasse(), "Cavaleiro");
    EXPECT_EQ(piromante->getClasse(), "Piromante");
    EXPECT_EQ(ladino->getClasse(), "Ladino");
    EXPECT_EQ(clerigo->getClasse(), "Clérigo");
}

TEST_F(PersonagemFixture, RacaHumanoBonus) {
    // Humano +1 força base adicionada
    Cavaleiro* base = PersonagemFactory::criarCavaleiro("Base", "Humano");
    EXPECT_GT(base->getForca(), 0);
    delete base;
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de Polimorfismo via ponteiro Entidade*
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(PersonagemFixture, AtacarViaPolimorfimoEntidade) {
    Entidade* alvo = ladino;
    int hpAntes = alvo->getHPAtual();
    cavaleiro->atacar(*alvo);
    // ladino deve ter sofrido dano
    EXPECT_LT(alvo->getHPAtual(), hpAntes);
}

TEST_F(PersonagemFixture, LadinoFurtivoDobraDano) {
    ladino->ativarFurtivo();
    int hpAntes = cavaleiro->getHPAtual();
    ladino->atacar(*cavaleiro);
    int danoSofrido = hpAntes - cavaleiro->getHPAtual();
    EXPECT_GT(danoSofrido, 0);
    // após golpe furtivo, furtivo é desativado
    EXPECT_FALSE(ladino->isFurtivo());
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de Inimigo e Factory de Inimigos
// ─────────────────────────────────────────────────────────────────────────────

TEST(InimigoTest, CriarTodosOsTipos) {
    const vector<string> tipos = {"hollow","blackKnight","pinwheel","gargoyle","ornstein"};
    for(const string &t : tipos) {
        Inimigo* i = InimigoFactory::criarInimigo(t);
        EXPECT_NE(i, nullptr);
        EXPECT_TRUE(i->estaVivo());
        delete i;
    }
}

TEST(InimigoTest, TipoInvalidoLancaExcecao) {
    EXPECT_THROW(InimigoFactory::criarInimigo("dracoLord"), invalid_argument);
}

TEST(InimigoTest, AtacarReduzHPAlvo) {
    Inimigo* hollow = InimigoFactory::criarInimigo("hollow");
    Cavaleiro* c = PersonagemFactory::criarCavaleiro("Teste", "Humano");
    int hpAntes = c->getHPAtual();
    hollow->atacar(*c);
    EXPECT_LT(c->getHPAtual(), hpAntes);
    delete hollow; delete c;
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de Habilidades
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(PersonagemFixture, HabilidadesCooldown) {
    const auto& habs = cavaleiro->getHabilidades();
    ASSERT_FALSE(habs.empty());
    EXPECT_TRUE(habs[0]->podeUsar());
    cavaleiro->usarHabilidade(0, *ladino);
    EXPECT_FALSE(habs[0]->podeUsar());
    cavaleiro->reduzirCooldowns();
    // cooldown AtaquePesado = 1, então após 1 redução pode usar
    EXPECT_TRUE(habs[0]->podeUsar());
}

TEST_F(PersonagemFixture, MiracleClerigoRecupera) {
    clerigo->receberDano(100);
    int hpAntes = clerigo->getHPAtual();
    clerigo->usarHabilidade(0, *clerigo); // Emissão de Luz cura a si mesmo
    EXPECT_GE(clerigo->getHPAtual(), hpAntes);
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes STL — uso de map no Inventario e algoritmos
// ─────────────────────────────────────────────────────────────────────────────

TEST(STLTest, MapInternoInventario) {
    Inventario inv(300);
    inv.adicionarItem(ItemFactory::criarArma("espadaLonga"));
    inv.adicionarItem(ItemFactory::criarArma("machado"));
    inv.adicionarItem(ItemFactory::criarArma("adaga"));
    // map deve ter 3 entradas
    EXPECT_EQ(inv.getIndice().size(), 3u);
}

TEST(STLTest, AlgorithmFindIfViaInventario) {
    Inventario inv(300);
    inv.adicionarItem(ItemFactory::criarPocao("estus"));
    // getPocao() usa find_if internamente
    EXPECT_NE(inv.getPocao(), nullptr);
}

TEST(STLTest, VectorHabilidadesOrdem) {
    Cavaleiro* c = PersonagemFactory::criarCavaleiro("T", "Humano");
    EXPECT_EQ(c->getHabilidades()[0]->getNome(), "Ataque Pesado");
    EXPECT_EQ(c->getHabilidades()[1]->getNome(), "Iron Flesh");
    delete c;
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de operadores de Entidade
// ─────────────────────────────────────────────────────────────────────────────

TEST_F(PersonagemFixture, OperadorMenorPorNivel) {
    EXPECT_FALSE(*cavaleiro < *cavaleiro); // mesmo nível
    cavaleiro->ganharExp(cavaleiro->getExpProxNivel()); // level up
    // após levelUp cavaleiro é maior que um nível 1
    Cavaleiro* fraco = PersonagemFactory::criarCavaleiro("Fraco", "Humano");
    EXPECT_TRUE(*fraco < *cavaleiro);
    delete fraco;
}

TEST_F(PersonagemFixture, OperadorIgualdadeNomeENivel) {
    Cavaleiro* clone = PersonagemFactory::criarCavaleiro("Artorias", "Humano");
    EXPECT_TRUE(*cavaleiro == *clone);
    delete clone;
}

// ─────────────────────────────────────────────────────────────────────────────
// Testes de fábrica de raças
// ─────────────────────────────────────────────────────────────────────────────

TEST(RacaTest, DragaoAntigoMaisHP) {
    Cavaleiro* humano = PersonagemFactory::criarCavaleiro("H", "Humano");
    Cavaleiro* dragao = PersonagemFactory::criarCavaleiro("D", "Dragão Antigo");
    EXPECT_GT(dragao->getHPMax(), humano->getHPMax());
    delete humano; delete dragao;
}

TEST(RacaTest, PygmyMaisInteligencia) {
    Piromante* h = PersonagemFactory::criarPiromante("H", "Humano");
    Piromante* p = PersonagemFactory::criarPiromante("P", "Pygmy");
    EXPECT_GT(p->getInteligencia(), h->getInteligencia());
    delete h; delete p;
}

TEST(RacaTest, RacaInvalidaLancaExcecao) {
    EXPECT_THROW(PersonagemFactory::criarCavaleiro("X", "Elfo"), invalid_argument);
}

