#ifndef FILA_OCORRENCIAS_H
#define FILA_OCORRENCIAS_H
#define MAX_STR 100
#define MAX_DESC 250
#define ARQUIVO_DADOS "ocorrencias.txt"

// entidade ocorrencia
typedef struct {
    int id_ocorrencia; // Protocolo (inicia em 1000, incremento de 10)
    int id_morador;
    char categoria[50];
    char titulo[MAX_STR];
    char descricao[MAX_DESC];
    char endereco_problema[MAX_STR];
    char data_registro[20];
    char status[20]; // "Pendente", "Em Andamento", "Concluido", "Cancelado"
    char solucao_descricao[MAX_DESC];
} Ocorrencia;

// estrutura da fila dinâmica
typedef struct {
    Ocorrencia *dados;
    int tamanho;
    int capacidade;
    int proximo_protocolo;
} FilaOcorrencias;

// gerenciamento de memória
void inicializarFila(FilaOcorrencias *fila, int capacidadeInicial);
void liberarFila(FilaOcorrencias *fila);

// persistência em arquivo
void salvarFilaEmArquivo(const FilaOcorrencias *fila, const char *nomeArquivo);
void carregarFilaDeArquivo(FilaOcorrencias *fila, const char *nomeArquivo);

// operações de fila (FIFO - First-In, First-Out) e CRUD
int enfileirarOcorrencia(FilaOcorrencias *fila, int id_morador, const char *categoria, const char *titulo, const char *descricao, const char *endereco, const char *data, int silencioso);
int desenfileirarOcorrencia(FilaOcorrencias *fila, Ocorrencia *removida);
void listarOcorrencias(const FilaOcorrencias *fila);
void atualizarStatusOcorrencia(FilaOcorrencias *fila, int protocolo);
void removerOcorrenciaPorProtocolo(FilaOcorrencias *fila, int protocolo);

// algoritmos de ordenação e busca binária
void ordenarPorProtocolo(FilaOcorrencias *fila);
void ordenarPorTitulo(FilaOcorrencias *fila);
int buscaBinariaProtocolo(FilaOcorrencias *fila, int protocolo);

// utilitários de entrada e carga inicial
void limparBuffer(void);
void lerString(char *destino, int tamanho);
void lerStringObrigatoria(const char *rotulo, char *destino, int tamanho);
void carregarDadosExemplo(FilaOcorrencias *fila);

#endif