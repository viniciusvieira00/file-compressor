# Projeto de Compressão e Descompressão de Arquivos com Algoritmo de Huffman

Este projeto implementa um sistema de compressão e descompressão de arquivos utilizando o algoritmo de Huffman. O objetivo é reduzir o tamanho dos arquivos para armazenamento e transmissão eficiente. O projeto inclui uma interface de linha de comando para facilitar o uso por parte dos usuários.

## Funcionalidades

- **Compressão de Arquivos**: Reduz o tamanho de arquivos usando codificação de Huffman.
- **Descompressão de Arquivos**: Restaura arquivos comprimidos ao seu estado original.
- **Interface de Usuário Simples**: Interface de linha de comando para facilitar a operação de compressão e descompressão.

## Algoritmo de Huffman

O algoritmo de Huffman é uma técnica de compressão de dados sem perda que utiliza uma árvore binária para representar caracteres com comprimentos de código variável, baseados na frequência de ocorrência. Caracteres mais frequentes são representados por códigos mais curtos, enquanto caracteres menos frequentes recebem códigos mais longos.

### Funcionamento do Algoritmo

1. **Construção da Árvore de Huffman**:
   - Contagem da frequência de cada caractere no arquivo.
   - Criação de uma árvore binária onde cada folha representa um caractere.
   - Combinação de árvores menores em uma única árvore que representa a codificação de Huffman.

2. **Compressão**:
   - Substituição de cada caractere no arquivo original pelo código binário correspondente da árvore de Huffman.

3. **Descompressão**:
   - Leitura da árvore de Huffman e do arquivo comprimido.
   - Reconstrução do arquivo original utilizando a árvore de Huffman para decodificar os dados.

## Como Executar o Projeto

### Pré-requisitos

- Compilador C (GCC recomendado)
- Sistema operacional compatível com a execução de binários compilados (Linux, macOS, Windows com ambiente adequado)

### Passos para Compilar e Executar

1. Clone o repositório:
   ```bash
   git clone https://github.com/viniciusvieira00/file-compressor.git
   cd file-compressor/src/
   ```

2. Compile os arquivos:
   ```bash
   gcc main.c compressor.c decompressor.c common.c -o huffman_compressor -lm
   ```

   Este comando compilará todos os arquivos fonte (`main.c`, `compressor.c`, `decompressor.c`, `common.c`) e criará o executável `huffman_compressor`. O flag `-lm` é utilizado para linkar a biblioteca matemática padrão, se necessário.

3. Execute o programa:
   ```bash
   ./huffman_compressor
   ```

### Uso sem Compilação

Caso o usuário não queira compilar o programa por conta própria, pode usar o executável pré-compilado `huffman_compressor` disponível na pasta do projeto. Para executá-lo, siga os passos abaixo:

1. Navegue até o diretório onde o executável está localizado:
   ```bash
   cd file-compressor/src/
   ```

2. Execute o programa:
   ```bash
   ./huffman_compressor
   ```

### Uso

Após executar o programa, siga as instruções na tela para comprimir ou descomprimir um arquivo. Você será solicitado a:

1. Escolher uma operação (compressão ou descompressão).
2. Inserir o nome do arquivo de entrada (e o nome do arquivo de saída, se desejar).

A interface fornecerá informações sobre o tamanho original do arquivo, o tamanho comprimido e a taxa de compressão obtida.

## Estrutura do Projeto

- `src/`: Contém os arquivos-fonte do projeto.
- `main.c`: Implementa a interface de usuário e integra as funções de compressão e descompressão.
- `compressor.c`: Contém as funções para compressão de arquivos usando o algoritmo de Huffman.
- `decompressor.c`: Contém as funções para descompressão de arquivos comprimidos.
- `common.c`: Implementa funções comuns utilizadas tanto no processo de compressão quanto de descompressão.
- `compressor.h`, `decompressor.h`, `common.h`: Cabeçalhos que definem as interfaces das funções utilizadas no projeto.

### Vídeo de Utilização

Para entender melhor como utilizar o projeto de compressão e descompressão de arquivos com o algoritmo de Huffman, assista ao vídeo de demonstração no YouTube. O vídeo guia você pelos passos necessários para comprimir e descomprimir arquivos, além de demonstrar a interface de linha de comando do programa.

Você pode assistir ao vídeo diretamente abaixo:

<iframe width="560" height="315" src="https://www.youtube.com/embed/mEPHPr-cEJU?si=R5oDR3WQRYTRuFKF" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

O vídeo cobre os seguintes tópicos:

1. **Introdução ao Programa**: Explicação do propósito do projeto e suas funcionalidades principais.
2. **Passos para Compilação**: Mostra como compilar os arquivos fonte para gerar o executável.
3. **Execução do Programa**: Demonstra como executar o programa para comprimir e descomprimir arquivos.
4. **Exemplo Prático**: Passo a passo de compressão e descompressão de um arquivo exemplo, incluindo detalhes sobre a entrada e saída de dados, bem como informações sobre o tamanho e a taxa de compressão.

Assista ao vídeo para uma visão completa de como usar o projeto!

## Contribuidores do Projeto

- [Vinicius Vieira](https://github.com/viniciusvieira00), Matrícula: 190118059
- [Luciano Machado](https://github.com/Hierophylax), Matrícula: 180126130