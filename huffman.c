/*
 * Implementação do algoritmo de Huffman
 * Aqui estão todas as funções declaradas no cabeçalho
 */
#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Função para criar um nó novo
 * Recebe um byte, sua frequência e se é folha ou não
 */
Node* criar_no(int byte, uint32_t freq, bool folha) {
    Node *node = malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Erro: sem memória!\n");
        exit(1);
    }
    node->freq = freq;
    node->byte = byte;
    node->esq = node->dir = NULL;
    node->folha = folha;
    return node;
}

/* --- Heap mínimo --- */

/*
 * Cria um heap vazio com capacidade dada
 */
MinHeap* criar_heap(int capacity) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    heap->nodes = malloc(capacity * sizeof(Node*));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

/*
 * Garante que a propriedade do heap seja mantida:
 * O menor nó fica no topo
 */
void heapify(MinHeap *heap, int idx) {
    int menor = idx;
    int esq = 2*idx + 1, dir = 2*idx + 2;

    if (esq < heap->size && heap->nodes[esq]->freq < heap->nodes[menor]->freq)
        menor = esq;
    if (dir < heap->size && heap->nodes[dir]->freq < heap->nodes[menor]->freq)
        menor = dir;

    if (menor != idx) {
        Node *tmp = heap->nodes[idx];
        heap->nodes[idx] = heap->nodes[menor];
        heap->nodes[menor] = tmp;
        heapify(heap, menor);
    }
}

/*
 * Remove e retorna o nó de menor frequência do heap
 */
Node* pop_min(MinHeap *heap) {
    if (heap->size == 0) return NULL;
    Node *min = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
    return min;
}

/*
 * Insere um novo nó no heap e reordena
 */
void push_heap(MinHeap *heap, Node *node) {
    int i = heap->size++;
    heap->nodes[i] = node;
    while (i > 0 && heap->nodes[(i-1)/2]->freq > heap->nodes[i]->freq) {
        Node *tmp = heap->nodes[i];
        heap->nodes[i] = heap->nodes[(i-1)/2];
        heap->nodes[(i-1)/2] = tmp;
        i = (i-1)/2;
    }
}

/*
 * Libera a memória usada pelo heap
 */
void liberar_heap(MinHeap *heap) {
    free(heap->nodes);
    free(heap);
}

/* --- Construção da árvore de Huffman --- */

/*
 * Cria a árvore Huffman a partir das frequências dos símbolos
 */
Node* construir_arvore_huffman(int freq[]) {
    // Conta quantos símbolos aparecem no texto
    int simbolos = 0;
    for (int i=0;i<256;i++) if (freq[i]>0) simbolos++;

    // Cria o heap e insere todos os símbolos
    MinHeap *heap = criar_heap(simbolos);
    for (int i=0;i<256;i++) {
        if (freq[i]>0) heap->nodes[heap->size++] = criar_no(i, freq[i], true);
    }

    // Ajusta o heap
    for (int i=(heap->size-1)/2;i>=0;i--) heapify(heap,i);

    // Caso especial: texto com apenas um símbolo
    if (heap->size == 1) {
        Node *unico = pop_min(heap);
        Node *raiz = criar_no(-1, unico->freq, false);
        raiz->esq = unico;
        liberar_heap(heap);
        return raiz;
    }

    // Combina sempre os dois nós de menor frequência
    while (heap->size>1) {
        Node *esq = pop_min(heap);
        Node *dir = pop_min(heap);
        Node *pai = criar_no(-1, esq->freq+dir->freq, false);
        pai->esq = esq; pai->dir = dir;
        push_heap(heap,pai);
    }

    // O que sobra é a raiz
    Node *raiz = pop_min(heap);
    liberar_heap(heap);
    return raiz;
}

/* --- Geração dos códigos Huffman --- */

/*
 * Percorre a árvore e gera os códigos binários de cada símbolo
 */
void gerar_codigos(Node *node, HuffCode *codigos, uint32_t code, int depth) {
    if (!node) return;
    if (node->folha) {
        codigos[node->byte].code = code;
        codigos[node->byte].bits = depth;
        return;
    }
    gerar_codigos(node->esq, codigos, (code<<1), depth+1);
    gerar_codigos(node->dir, codigos, (code<<1)|1, depth+1);
}

/* --- Escrita de bits (compactação) --- */

/*
 * Inicializa o escritor de bits
 */
BitWriter* bitwriter_init() {
    BitWriter *bw = malloc(sizeof(BitWriter));
    bw->max_bytes = 1024;
    bw->buffer = calloc(bw->max_bytes,1);
    bw->bit_pos = 0;
    bw->byte_pos = 0;
    return bw;
}

/*
 * Adiciona um código binário ao buffer de saída
 */
void bitwriter_put_bits(BitWriter *bw, uint32_t code, int len) {
    for (int i=len-1;i>=0;i--) {
        int bit = (code>>i)&1;
        bw->buffer[bw->byte_pos] |= bit<<(7-bw->bit_pos);
        bw->bit_pos++;
        if (bw->bit_pos==8) {
            bw->bit_pos=0;
            bw->byte_pos++;
            if (bw->byte_pos>=bw->max_bytes) {
                bw->max_bytes*=2;
                bw->buffer = realloc(bw->buffer,bw->max_bytes);
            }
        }
    }
}

/*
 * Finaliza a escrita, completando o último byte se necessário
 */
void bitwriter_flush(BitWriter *bw) {
    if (bw->bit_pos>0) bw->byte_pos++;
}

/*
 * Libera a memória do escritor de bits
 */
void liberar_bitwriter(BitWriter *bw) {
    free(bw->buffer);
    free(bw);
}

/* --- Impressão dos resultados --- */

/*
 * Retorna caractere visível ou '.' se for não-imprimível
 */
char byte_para_char(int byte) {
    if (byte>=32 && byte<=126) return (char)byte;
    return '.';
}

/*
 * Mostra a tabela de códigos gerada
 */
void imprimir_tabela_codigos(HuffCode *codigos, int freq[]) {
    for (int i=0;i<256;i++) {
        if (freq[i]>0) {
            printf("'%c' (%d): ", byte_para_char(i), i);
            for (int b=codigos[i].bits-1;b>=0;b--) {
                printf("%d",(codigos[i].code>>b)&1);
            }
            printf(" (freq %d)\n", freq[i]);
        }
    }
}

/*
 * Mostra os bits da saída compactada
 */
void imprimir_bits(BitWriter *bw) {
    printf("Bits: ");
    for (int i=0;i<bw->byte_pos;i++) {
        for (int b=7;b>=0;b--) {
            printf("%d",(bw->buffer[i]>>b)&1);
        }
        printf(" ");
    }
    printf("\n");
}

/*
 * Mostra a saída compactada em hexadecimal
 */
void imprimir_hex(BitWriter *bw) {
    printf("Hex: ");
    for (int i=0;i<bw->byte_pos;i++) {
        printf("%02X ", bw->buffer[i]);
    }
    printf("\n");
}

/* --- Liberação da árvore --- */
void liberar_arvore(Node *node) {
    if (!node) return;
    liberar_arvore(node->esq);
    liberar_arvore(node->dir);
    free(node);
}
