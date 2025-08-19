#include <stdio.h>
#include <string.h>
#include "huffman.h"

int main() {
    char texto[1000];
    int frequencias[TAM_MAX];
    CodigoHuffman tabela[TAM_MAX];
    char codigo[50] = {0};
    int indice = 0;
    char codigoCompleto[5000];

    // ETAPA 1: GERAR TABELA
    printf("Digite o texto para gerar a tabela de Huffman: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = '\0';

    printf("\nTexto original: %s\n", texto);

    contarFrequencias(texto, frequencias);
    exibirFrequencias(frequencias);

    No *raiz = construirArvore(frequencias);
    gerarCodigos(raiz, codigo, 0, tabela, &indice);

    exibirTabelaHuffman(tabela, indice);

    // ETAPA 2: GERAR CÓDIGO COMPLETO DO TEXTO
    gerarCodigoCompleto(texto, tabela, indice, codigoCompleto);
    printf("\nCodigo completo do texto: %s\n", codigoCompleto);

    // ETAPA 3: DESCOMPRIMIR
    descomprimirTexto(codigoCompleto, raiz);

    liberarArvore(raiz);
    return 0;
}
