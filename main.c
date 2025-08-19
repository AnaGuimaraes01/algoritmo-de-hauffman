#include <stdio.h>
#include <string.h>
#include "huffman.h"

int main() {
    char texto[1000];
    int frequencias[TAM_MAX];
    CodigoHuffman tabela[TAM_MAX];
    char codigo[50] = {0};
    int indice = 0;

    printf("Digite o texto para comprimir: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = '\0';

    printf("\nTexto original: %s\n", texto);

    contarFrequencias(texto, frequencias);
    exibirFrequencias(frequencias);

    No *raiz = construirArvore(frequencias);
    gerarCodigos(raiz, codigo, 0, tabela, &indice);

    exibirCodigosOrdenados(tabela, indice);
    comprimirTexto(texto, tabela, indice);

    // Criar o texto comprimido em string para teste da descompressão
    char textoBits[5000] = {0};
    for (int i = 0; texto[i] != '\0'; i++) {
        for (int j = 0; j < indice; j++) {
            if (tabela[j].caractere == texto[i]) {
                strcat(textoBits, tabela[j].codigo);
                break;
            }
        }
    }

    descomprimirTexto(textoBits, raiz);

    liberarArvore(raiz);

    return 0;
}
