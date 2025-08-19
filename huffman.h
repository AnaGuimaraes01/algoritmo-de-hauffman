#ifndef HUFFMAN_H
#define HUFFMAN_H

#define TAM_MAX 256

typedef struct No {
    char caractere;
    int frequencia;
    struct No *esquerda, *direita;
} No;

typedef struct {
    char caractere;
    char codigo[50];
} CodigoHuffman;

void contarFrequencias(const char *texto, int frequencias[]);
No* construirArvore(int frequencias[]);
void gerarCodigos(No *raiz, char *codigo, int nivel, CodigoHuffman tabela[], int *indice);
void exibirTabelaHuffman(CodigoHuffman tabela[], int tamanho);
void exibirFrequencias(int frequencias[]);
void exibirCodigoCompleto(const char *texto, CodigoHuffman tabela[], int tamanho);
void descomprimirTextoManual(No *raiz);
void liberarArvore(No *raiz);

#endif
