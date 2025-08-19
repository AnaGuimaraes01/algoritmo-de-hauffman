#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

typedef struct {
    No* nos[TAM_MAX];
    int tamanho;
} FilaPrioridade;

No* criarNo(char caractere, int frequencia) {
    No *novo = (No*)malloc(sizeof(No));
    novo->caractere = caractere;
    novo->frequencia = frequencia;
    novo->esquerda = novo->direita = NULL;
    return novo;
}

void inicializarFila(FilaPrioridade *fila) { fila->tamanho = 0; }

void inserirFila(FilaPrioridade *fila, No *no) {
    fila->nos[fila->tamanho++] = no;
    for (int i = fila->tamanho - 1; i > 0; i--) {
        if (fila->nos[i]->frequencia < fila->nos[i - 1]->frequencia ||
           (fila->nos[i]->frequencia == fila->nos[i - 1]->frequencia &&
            fila->nos[i]->caractere < fila->nos[i - 1]->caractere)) {
            No* temp = fila->nos[i];
            fila->nos[i] = fila->nos[i - 1];
            fila->nos[i - 1] = temp;
        } else break;
    }
}

No* removerMenor(FilaPrioridade *fila) { return fila->nos[--fila->tamanho]; }

void contarFrequencias(const char *texto, int frequencias[]) {
    for (int i = 0; i < TAM_MAX; i++) frequencias[i] = 0;
    for (int i = 0; texto[i] != '\0'; i++) frequencias[(unsigned char)texto[i]]++;
}

No* construirArvore(int frequencias[]) {
    FilaPrioridade fila;
    inicializarFila(&fila);
    for (int i = 0; i < TAM_MAX; i++)
        if (frequencias[i] > 0)
            inserirFila(&fila, criarNo((char)i, frequencias[i]));
    while (fila.tamanho > 1) {
        No *esq = removerMenor(&fila);
        No *dir = removerMenor(&fila);
        if (esq->frequencia > dir->frequencia) { No* temp = esq; esq = dir; dir = temp; }
        No *novo = criarNo('\0', esq->frequencia + dir->frequencia);
        novo->esquerda = esq;
        novo->direita = dir;
        inserirFila(&fila, novo);
    }
    return removerMenor(&fila);
}

void gerarCodigos(No *raiz, char *codigo, int nivel, CodigoHuffman tabela[], int *indice) {
    if (!raiz) return;
    if (!raiz->esquerda && !raiz->direita) {
        codigo[nivel] = '\0';
        tabela[*indice].caractere = raiz->caractere;
        strcpy(tabela[*indice].codigo, codigo);
        (*indice)++;
        return;
    }
    if (raiz->esquerda) { codigo[nivel] = '0'; gerarCodigos(raiz->esquerda, codigo, nivel + 1, tabela, indice); }
    if (raiz->direita) { codigo[nivel] = '1'; gerarCodigos(raiz->direita, codigo, nivel + 1, tabela, indice); }
}

void exibirTabelaHuffman(CodigoHuffman tabela[], int tamanho) {
    printf("\nTabela de Codigos Huffman:\n");
    for (int i = 0; i < tamanho; i++)
        printf("%c: %s\n", tabela[i].caractere, tabela[i].codigo);
}

void exibirCodigoCompleto(const char *texto, CodigoHuffman tabela[], int tamanho) {
    printf("\nCodigo completo do texto: ");
    for (int i = 0; texto[i] != '\0'; i++) {
        for (int j = 0; j < tamanho; j++) {
            if (tabela[j].caractere == texto[i]) {
                printf("%s", tabela[j].codigo);
                break;
            }
        }
    }
    printf("\n");
}

void descomprimirTextoManual(No *raiz) {
    char codigoBits[5000];
    printf("\nDigite o codigo de Huffman para descomprimir: ");
    fgets(codigoBits, sizeof(codigoBits), stdin);
    codigoBits[strcspn(codigoBits, "\n")] = '\0';

    No *atual = raiz;
    printf("\nTexto descomprimido:\n");
    for (int i = 0; codigoBits[i] != '\0'; i++) {
        if (codigoBits[i] == '0') atual = atual->esquerda;
        else if (codigoBits[i] == '1') atual = atual->direita;
        if (!atual->esquerda && !atual->direita) {
            printf("%c", atual->caractere);
            atual = raiz;
        }
    }
    printf("\n");
}

void exibirFrequencias(int frequencias[]) {
    printf("\nFrequencia de cada caractere:\n");
    for (int i = 0; i < TAM_MAX; i++)
        if (frequencias[i] > 0)
            printf("%c: %d\n", i, frequencias[i]);
}

void liberarArvore(No *raiz) {
    if (!raiz) return;
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    free(raiz);
}
