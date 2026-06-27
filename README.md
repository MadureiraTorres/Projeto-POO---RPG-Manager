# Projeto POO - RPG Manager

> Sistema de gerenciamento de personagens para RPG de terminal, inspirado no universo de **Dark Souls 1**.  
> Desenvolvido em **C++17** com persistência em SQLite e testes automatizados via Google Test.
 
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![SQLite](https://img.shields.io/badge/SQLite-3-lightgrey?logo=sqlite)
![GTest](https://img.shields.io/badge/Tests-Google%20Test-brightgreen)
![CMake](https://img.shields.io/badge/Build-CMake%203.14%2B-red)
![License](https://img.shields.io/badge/License-MIT-yellow)
 
---
 
## Sumário
 
- [Sobre o projeto](#sobre-o-projeto)
- [Pré-requisitos](#pré-requisitos)
- [Instalação e build](#instalação-e-build)
- [Como jogar](#como-jogar)
- [Personagens disponíveis](#personagens-disponíveis)
- [Estrutura do projeto](#estrutura-do-projeto)
- [Rodando os testes](#rodando-os-testes)
- [Como estender o projeto](#como-estender-o-projeto)
---
 
## Sobre o projeto
 
O RPG Manager simula um loop de aventura no universo de Dark Souls 1: criar um personagem,
explorar, combater inimigos, descansar em fogueiras para subir de nível e salvar o
progresso em banco de dados local.
 
**Conceitos aplicados:**
 
- Herança e polimorfismo (`Entidade` → `Personagem` → classes concretas)
- Padrão **Strategy** — comportamento de IA dos inimigos plugável em tempo de execução
- Padrão **Factory Method** — criação de itens, personagens e inimigos desacoplada
- Padrão **Template Method** — `levelUp()` e `descrever()` com variação controlada por subclasse
- **Composição** sobre herança — `Inventario` e `Habilidade` como membros, não ancestrais
- Persistência relacional com **SQLite3** via prepared statements
- Testes automatizados com **Google Test**
---
 
## Pré-requisitos
 
| Dependência | Versão mínima | Ubuntu/Debian | Arch Linux | macOS (Homebrew) |
|---|---|---|---|---|
| g++ | 9+ (C++17) | `sudo apt install g++` | `sudo pacman -S gcc` | `brew install gcc` |
| CMake | 3.14+ | `sudo apt install cmake` | `sudo pacman -S cmake` | `brew install cmake` |
| SQLite3 | 3.x | `sudo apt install libsqlite3-dev` | `sudo pacman -S sqlite` | `brew install sqlite` |
| Google Test | qualquer | `sudo apt install libgtest-dev` | `sudo pacman -S gtest` | `brew install googletest` |
 
---
 
## Instalação e build
 
### Jogo principal
 
```bash
# 1. Clone o repositório
git clone https://github.com/seu-usuario/rpg-manager.git
cd rpg-manager
 
# 2. Compile
g++ -std=c++17 main.cpp src/sqlite3.o -o rpg_maker
 
# 3. Execute
./rpg_manager
```
 
### Build com CMake (opcional)
 
```bash
mkdir build && cd build
cmake ..
make
./rpg_manager
```
 
O arquivo de save (`rpg_save.db`) é criado automaticamente no diretório de execução
na primeira vez que o jogo é iniciado.
 
---
 
## Como jogar
 
Ao iniciar, o menu principal oferece três opções:
 
```
1. Novo jogo   — cria personagem escolhendo classe e raça
2. Carregar    — lista saves disponíveis e retoma a partir do último estado
3. Sair
```
 
**Loop de jogo:**
 
```
Criar personagem → Explorar → Combate por turnos → Fogueira → Salvar → ...
```
 
**Durante o combate**, em cada turno você pode:
- Atacar com a arma equipada
- Usar uma habilidade (com custo de stamina e cooldown)
- Usar Frasco Estus para recuperar HP
- Tentar fugir
**Na fogueira**, é possível:
- Restaurar HP e Estus completamente
- Gastar EXP para subir de nível, escolhendo o atributo a fortalecer (+3)
- Ver o status completo do personagem
---
 
## Personagens disponíveis
 
### Classes
 
| Classe | HP | Foco | Habilidades iniciais |
|---|---|---|---|
| Cavaleiro | 220 | Tanque — alta Força/Vitalidade | Ataque Pesado, Iron Flesh |
| Piromante | 120 | Dano mágico — Inteligência | Grande Bola de Fogo, Alma Torrencial |
| Ladino | 140 | Velocidade — alta Destreza | Punhalada pelas Costas |
| Clérigo | 160 | Suporte — alta Fé | Emissão de Luz, Ataque Pesado |
 
### Raças
 
| Raça | Bônus principal | Penalidade | Perfil |
|---|---|---|---|
| Humano | +1 em todos os atributos | — | Versátil |
| Morto-Vivo | +100 HP, +2 Força | — | Tanque físico |
| Pygmy | +3 Intel, +3 Fé | -1 Força | Mago / Clérigo |
| Dragão Antigo | +150 HP, +4 Força | -3 Intel, -3 Fé | Brutalidade pura |
 
### Inimigos
 
| Inimigo | HP | Nível | EXP |
|---|---|---|---|
| Soldado Oco | 180 | 1 | 200 |
| Cavaleiro Negro | 450 | 5 | 800 |
| Catavento *(Boss)* | 1200 | 8 | 2000 |
| Gárgula da Torre Bell *(Boss)* | 1800 | 10 | 3500 |
| Ornstein, o Caçador de Dragões *(Boss)* | 3500 | 20 | 20000 |
 
---
 
## Estrutura do projeto
 
```
rpg-manager/
├── main.cpp              # Entry point
├── src/
│   ├── entidade.h        # Classe abstrata raiz
│   ├── personagem.h      # Personagem jogável base
│   ├── classes.h         # Cavaleiro, Piromante, Ladino, Clérigo
│   ├── raca.h            # Humano, Morto-Vivo, Pygmy, Dragão Antigo
│   ├── inimigo.h         # Inimigos + Strategy de IA
│   ├── habilidade.h      # Habilidades com cooldown
│   ├── item.h            # Arma, Armadura, Poção, ItemEspecial
│   ├── inventario.h      # Inventário com peso e slots
│   ├── factories.h       # ItemFactory, PersonagemFactory, InimigoFactory
│   ├── fogueira.h        # Sistema de descanso e level up
│   ├── batalha.h         # Motor de combate por turnos
│   ├── menu.h            # Loop principal da UI
│   └── persistencia.h    # Serialização SQLite3
├── tests/
│   ├── test_rpg.cpp      # Suite Google Test
│   └── CMakeLists.txt    # Build dos testes
└── rpg_save.db           # Gerado automaticamente em runtime
```
 
---
 
## Rodando os testes
 
```bash
cd tests
mkdir build && cd build
cmake ..
make
./rpg_tests
```
 
Saída esperada:
 
```
[==========] Running X tests from Y test suites.
[  PASSED  ] X tests.
```
 
A suite cobre: contratos de `Entidade`, factories (incluindo entradas inválidas),
mecânica de inventário e peso, cooldown de habilidades, progressão de level up
por classe, round-trip de save/load no SQLite e comportamento de IA dos inimigos.
 
---
 
## Como estender o projeto
 
### Adicionar um novo inimigo
 
1. Crie uma subclasse de `Inimigo` em `inimigo.h`, passando a IA desejada:
```cpp
class DragonSlayer : public Inimigo {
public:
    DragonSlayer()
        : Inimigo("Matador de Dragões", 2800, 15, 12000, 9000, new IABoss()) {}
};
```
 
2. Registre-o em `InimigoFactory::criarInimigo()` em `factories.h`:
```cpp
if(tipo == "dragonSlayer") return new DragonSlayer();
```
 
### Adicionar um novo item
 
1. Implemente o item em `item.h` (ou crie subclasse de `Arma`, `Armadura`, etc.)
2. Registre-o na factory correspondente em `ItemFactory` dentro de `factories.h`
### Adicionar uma nova habilidade
 
1. Herde de `Habilidade` em `habilidade.h` e implemente `usar()` e `getTipo()`
2. Adicione ao construtor da classe de personagem desejada em `classes.h`
### Adicionar uma nova IA
 
1. Herde de `ComportamentoIA` em `inimigo.h` e implemente `agir()`
2. Use-a ao construir qualquer `Inimigo`
---
 
## Documentação técnica
 
Para detalhes sobre todas as decisões de arquitetura, padrões de projeto aplicados
e estratégia de testes, consulte o
[Relatório Técnico](docs/relatorio_tecnico_rpg.pdf).
