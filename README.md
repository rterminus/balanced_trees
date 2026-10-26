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

- Implementação das duas estruturas em header com template. Para os testes,
  foram utilizadas estruturas com `<int>`.
- `insert` e `remove` retornam `bool`, informando se a árvore foi alterada.
  Duplicatas são ignoradas e as árvores não são copiáveis, contendo ponteiros
  próprios.
- Interface interativa via Terminal (CLI) contemplando:
  - Inserção e remoção de elementos dinâmicos
  - Busca de elementos
  - Impressão em ordem
  - Renderizador visual da hierarquia da árvore (nós direitos aparecem primeiro
    e nós esquerdos abaixo, verticalmente; na rubro-negra, `[R]` = vermelho e
    `[B]` = preto)
  - Verificação dos invariantes da árvore
- Após cada operação, a árvore é comparada com um
  `std::set` e seus invariantes são verificados:
  - AVL: ordem BST, fator de balanceamento em `[-1, 1]` e altura armazenada correta.
  - Rubro-Negra: raiz preta, nenhum nó vermelho com filho vermelho, mesma
    altura-negra em todos os caminhos, ordem BST e ponteiros `parent` consistentes.
  - Cobre casos de borda, como árvore vazia, duplicatas, remoção de inexistente,
    além de sequências ordenadas e operações aleatórias com várias sementes.
- **Benchmark comparativo AVL vs Rubro-Negra:**
  - Inserção ascendente, descendente e aleatória de 10.000, 50.000 e 100.000
    elementos, e remoção aleatória de metade para cada.
  - Métricas: tempo (ms e µs/operação), altura, rotações e rotações/operação
  - 5 repetições por cenário, semente fixa e reprodutível e mesmos dados para
    as duas árvores.
  - Tabela no terminal e exportação em CSV para geração de gráficos.

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

> Para medir tempo de forma fiel, compile com otimização
> (`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`).

### Modos de execução

| Comando                                    | O que faz                                                   |
| ------------------------------------------ | ----------------------------------------------------------- |
| `./build/balanced_trees`                   | Menu interativo                                             |
| `./build/balanced_trees --test`            | Roda apenas os testes de corretude                          |
| `./build/balanced_trees --bench [arquivo]` | Testes + benchmark; CSV em `arquivo` (padrão `results.csv`) |

O benchmark gera dois arquivos:

- `results.csv`: uma linha por execução
  (`scenario,n,ops,tree,repeat,time_ms,height,rotations,size,valid`)
- `results_summary.csv`: médias por cenário/tamanho/árvore
  (`time_ms_mean`, `time_ms_sd`, `us_per_op`, `height_mean`, `rotations_mean`,
  `rotations_per_op`, `valid`)

Cenários: `asc_insert`, `desc_insert`, `rand_insert` e `rand_remove`. A coluna
`valid` indica se os invariantes e o tamanho final estavam corretos.

## Geração de Gráficos

Os gráficos são gerados a partir do CSV agregado do benchmark pelo script
`scripts/plot_results.py`, que depende de `pandas`, `numpy` e `matplotlib`.

1. Gere os dados do benchmark:

```bash
./build/balanced_trees --bench results.csv
```

2. Instale as dependências e rode o script:

```bash
pip install pandas numpy matplotlib
python scripts/plot_results.py results_summary.csv
```

Serão gerados três arquivos PDF no diretório atual, cada um com um subplot por
cenário (inserção ascendente, descendente, aleatória e remoção aleatória):

- `grafico_tempos.pdf`: tempo de execução (ms) e curva teórica $O(n \log n)$
- `grafico_alturas.pdf`: altura da árvore e limites teóricos de AVL e Rubro-Negra
- `grafico_rotacoes.pdf`: número de rotações e curva teórica $O(n)$

## Estrutura do Projeto

- `scripts/`: Scripts auxiliares.
  - `plot_results.py`: Script de geração de gráficos com base no sumário dos resultados.
- `include/`: Definição e implementação das árvores, utilizando template e
  contando com as operações clássicas e métodos auxiliares de manipulação,
  visualização e verificação de invariantes.
  - `avl.hpp`
  - `rb.hpp`
- `src/`:
  - `main.cpp`: Ponto de entrada contendo a CLI, os testes de corretude e o
    benchmark com exportação CSV.
