# Amazon Engineering Challenge: Árvores Balanceadas

## Autor

**Luiz Renan da Silva Pontes** - 20250031601

## Descrição

Este repositório contém a implementação e apresentação do trabalho da Unidade
2 da disciplina IMD0039 — **Amazon Engineering Challenge: Balanced Trees**.

O projeto simula um cenário de Black Friday: alto volume de vendas e necessidade
de um sistema robusto e eficiente para lidar com inserções, remoções e buscas.
Para tal, foi implementado, como especificado, as árvores de busca binárias AVL
e Rubro-Negra em C++17, sem bibliotecas externas.

Para fins de avaliação, o código já está devidamente compilado em `build/`.
Basta executar `./build/balanced_trees` e interagir com o programa. Para a
apresentação, checar `docs/`.

## Estruturas Implementadas

- **Árvore AVL:** Balanceamento estrito baseado na diferença de altura das subárvores.
- **Árvore Rubro-Negra (Clássica):** Balanceamento flexível baseado em regras
  lógicas de coloração de nós.

## Funcionalidades

- Implementação das duas estruturas em header com template (para os testes
  foram utilizadas estruturas com `<int>`)
- Interface interativa via Terminal (CLI) contemplando:
  - Inserção e remoção de elementos dinâmicos
  - Busca de elementos
  - Impressão em ordem
  - Renderizador visual da hierarquia da árvore (nós direitos aparecem primeiro
    e nós esquerdos abaixo, verticalmente)
- Motor de testes de estresse automatizado, com cenários crescentes, decrescentes
  e aleatórios com até 10.000 nós

## Pré-requisitos

Para compilar o projeto, é necessário ter instalado no sistema:

- Compilador com suporte a C++17 (GCC ou Clang, de preferência)
- CMake (versão 3.10 ou superior)

## Compilação e Execução

O projeto utiliza o CMake como sistema de build. Execute os seguintes comandos:

1. Clone o repositório:

```bash
git clone https://github.com/rterminus/balanced_trees.git
cd balanced_trees
```

2. Crie o diretório de compilação e gere os arquivos de build:

```bash
cmake -S . -B build
```

3. Compile o código-fonte:

```bash
cmake --build build
```

4. Execute o programa:

```bash
./build/balanced_trees
```

## Estrutura do Projeto

- `include/`: Definição e implementação das árvores, utilizando template e
  contando com as operações clássicas e métodos auxiliares de manipulação
  e visualização.
  - `avl.hpp`
  - `rb.hpp`
- `src/`:
  - `main.cpp`: Ponto de entrada contendo a CLI e os cenários de simulação de estresse.
