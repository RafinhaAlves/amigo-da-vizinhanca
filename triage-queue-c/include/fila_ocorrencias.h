#ifndef FILA_OCORRENCIAS_H
#define FILA_OCORRENCIAS_H

#include <stdbool.h>

// Estrutura alinhada com a tabela OCORRENCIA e Estorias 2, 3, 5 e 6
typedef struct {
    int id_ocorrencia;           // Protocolo unico (IDENTITY 1000, 10)
    int id_morador;              // FK: Morador solicitante
    int id_categoria;            // FK: Categoria do problema
    int id_equipe;               // FK: Equipe atribuida (0 = Nenhuma)
    char titulo[100];
    char descricao[300];
    char endereco_problema[200];
    char data_registro[25];      // Automatico: dd/mm/aaaa HH:MM (Estoria 2)
    char status[20];             // Pendente, Em Andamento, Concluido, Cancelado
    char prioridade[15];         // Baixa, Media, Alta (Estoria 3)
    char data_atribuicao[25];    // Automatico ao atribuir equipe (Estoria 3)
    char prazo_estimado[15];     // Data prevista: dd/mm/aaaa (Estoria 3)
    char solucao_descricao[300]; // Obrigatorio ao concluir (Estoria 5)
    char data_conclusao[25];     // Automatico ao concluir (Estoria 5)
} Ocorrencia;

// TAD: Fila Sequencial Dinamica (Alocacao Contigua na Heap)
typedef struct {
    Ocorrencia *dados;
    int tamanho;
    int capacidade;
    int proximo_protocolo;
} FilaOcorrencias;

// Ciclo de Vida e Memoria
FilaOcorrencias* criarFila(int capacidade_inicial);
void liberarFila(FilaOcorrencias *fila);

// Operacoes Fundamentais (FIFO)
bool enfileirarOcorrencia(FilaOcorrencias *fila, Ocorrencia nova);
bool desenfileirarOcorrencia(FilaOcorrencias *fila, Ocorrencia *removida);

// Visualizacao, Filtros e Fluxo de Status (Estorias 3, 5 e 6)
void listarOcorrencias(const FilaOcorrencias *fila);
void filtrarOcorrencias(const FilaOcorrencias *fila);
void exibirDetalhesOcorrencia(const Ocorrencia *oc);
void atualizarStatusOcorrencia(FilaOcorrencias *fila, int protocolo);

// Algoritmos de Ordenacao e Busca
void ordenarPorProtocolo(FilaOcorrencias *fila);      // Bubble Sort
void ordenarPorTitulo(FilaOcorrencias *fila);         // Selection Sort
void ordenarPorDataRecente(FilaOcorrencias *fila);    // Selection Sort (Estoria 6)
int buscarPorProtocoloBinaria(FilaOcorrencias *fila, int protocolo);

// Persistencia em Disco (.txt)
bool salvarFilaEmArquivo(const FilaOcorrencias *fila, const char *caminho);
bool carregarFilaDeArquivo(FilaOcorrencias *fila, const char *caminho);

// Utilitario de Entrada Segura
void lerStringObrigatoria(const char *prompt, char *destino, int tamanho_max);

#endif