/* 
 * Cabeçalho para compressão Huffman
 * Aqui definimos todas as estruturas e funções usadas no algoritmo
 */

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdbool.h>

/* 
 * Estrutura do nó da árvore de Huffman
 * Pode ser:
 *  - Um nó folha: guarda um byte específico
 *  - Um nó interno: aponta para filhos (esquerda/direita)
 */
typedef struct Node {
    uint32_t freq;      // Frequência do símbolo
    int byte;           // Valor do byte (ou -1 se for nó interno)
    struct Node *esq;   // Filho esquerdo
    struct Node *dir;   // Filho direito
    bool folha;         // Indica se é um nó folha
} Node;

/*
 * Estrutura para guardar os códigos gerados
 * Cada símbolo vira um código binário de tamanho variável
 */
typedef struct {
    uint32_t code;      // O código em si (sequência de bits)
    int bits;           // Quantidade de bits do código
} HuffCode;

/*
 * Estrutura auxiliar para escrita de bits
 * Usada na hora de "compactar" de fato o texto
 */
typedef struct {
    uint8_t *buffer;    // Buffer de bytes compactados
    int bit_pos;        // Posição do próximo bit (0-7)
    int byte_pos;       // Quantos bytes já foram preenchidos
    int max_bytes;      // Capacidade máxima do buffer
} BitWriter;

/*
 * Estrutura do heap mínimo
 * Sempre mantém o nó com menor frequência no topo
 */
typedef struct {
    Node **nodes;       // Array de nós
    int size;           // Quantos elementos temos
    int capacity;       // Capacidade máxima
} MinHeap;

/* --- Funções principais --- */

// Criação de nós
Node* criar_no(int byte, uint32_t freq, bool folha);

// Operações do heap mínimo
MinHeap* criar_heap(int capacity);
void heapify(MinHeap *heap, int idx);
Node* pop_min(MinHeap *heap);
void push_heap(MinHeap *heap, Node *node);

// Construção da árvore e geração de códigos
Node* construir_arvore_huffman(int freq[]);
void gerar_codigos(Node *node, HuffCode *codigos, uint32_t code, int depth);

// Escrita de bits compactados
BitWriter* bitwriter_init();
void bitwriter_put_bits(BitWriter *bw, uint32_t code, int len);
void bitwriter_flush(BitWriter *bw);

// Liberação de memória
void liberar_arvore(Node *node);
void liberar_heap(MinHeap *heap);
void liberar_bitwriter(BitWriter *bw);

// Impressão dos resultados
void imprimir_tabela_codigos(HuffCode *codigos, int freq[]);
void imprimir_bits(BitWriter *bw);
void imprimir_hex(BitWriter *bw);
char byte_para_char(int byte);

#endif // HUFFMAN_H
