/*
 * Programa principal para testar a compressão Huffman
 * Fluxo:
 *  1. Lê texto digitado
 *  2. Conta frequências
 *  3. Constrói árvore Huffman
 *  4. Gera códigos
 *  5. Compacta texto
 *  6. Mostra resultados
 */
#include "huffman.h"
#include <stdio.h>
#include <string.h>

int main() {
    char texto[1024];
    int frequencias[256] = {0};  // Frequência de cada byte

    // Entrada do usuário
    printf("Digite o texto que quer comprimir: ");
    if (!fgets(texto, sizeof(texto), stdin)) {
        printf("Erro de leitura!\n");
        return 1;
    }

    // Remove o '\n' do final
    size_t tamanho = strlen(texto);
    if (tamanho>0 && texto[tamanho-1]=='\n') {
        texto[tamanho-1]='\0';
        tamanho--;
    }
    if (tamanho==0) {
        printf("Texto vazio!\n");
        return 0;
    }

    // Conta frequências
    for (size_t i=0;i<tamanho;i++) {
        frequencias[(unsigned char)texto[i]]++;
    }

    // Constrói a árvore
    Node *raiz = construir_arvore_huffman(frequencias);

    // Gera os códigos
    HuffCode codigos[256] = {0};
    if (raiz->folha) {
        // Caso especial: apenas um caractere no texto
        codigos[raiz->byte].code = 0;
        codigos[raiz->byte].bits = 1;
    } else {
        gerar_codigos(raiz, codigos, 0, 0);
    }

    // Mostra tabela de códigos
    printf("\n=== Tabela de Codigos ===\n");
    imprimir_tabela_codigos(codigos, frequencias);

    // Compacta o texto
    BitWriter *bw = bitwriter_init();
    for (size_t i=0;i<tamanho;i++) {
        unsigned char c = texto[i];
        bitwriter_put_bits(bw, codigos[c].code, codigos[c].bits);
    }
    bitwriter_flush(bw);

    // Mostra resultados
    printf("\n=== Resultado ===\n");
    imprimir_bits(bw);
    imprimir_hex(bw);

    // Estatísticas
    int original_bits = tamanho*8;
    int compactado_bits = bw->byte_pos*8 - (bw->bit_pos? (8-bw->bit_pos):0);

    printf("\nOriginal: %d bits\n", original_bits);
    printf("Compactado: %d bits\n", compactado_bits);
    if (original_bits>0) {
        double economia = 100.0*(1.0-(double)compactado_bits/original_bits);
        printf("Economia: %.2f%%\n", economia);
    }

    // Libera memória
    liberar_arvore(raiz);
    liberar_bitwriter(bw);

    return 0;
}
