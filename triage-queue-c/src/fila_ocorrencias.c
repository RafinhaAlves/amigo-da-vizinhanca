#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/fila_ocorrencias.h"

//utilitários de entrada e validação

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    } else {
        destino[0] = '\0';
    }
}

// garante que campos obrigatórios não sejam vazios nem compostos apenas por espaços (Estória 2)
void lerStringObrigatoria(const char *rotulo, char *destino, int tamanho) {
    int valido = 0;
    while (!valido) {
        printf("%s", rotulo);
        lerString(destino, tamanho);

        for (size_t i = 0; destino[i] != '\0'; i++) {
            if (!isspace((unsigned char)destino[i])) {
                valido = 1;
                break;
            }
        }

        if (!valido) {
            printf("[ERRO] Este campo e obrigatorio e nao pode ficar vazio!\n");
        }
    }
}

// gerenciamento de memoria (heap)

void inicializarFila(FilaOcorrencias *fila, int capacidadeInicial) {
    fila->dados = (Ocorrencia *)malloc(capacidadeInicial * sizeof(Ocorrencia));
    if (fila->dados == NULL) {
        printf("[ERRO CRITICO] Falha na alocacao de memoria!\n");
        exit(1);
    }
    fila->tamanho = 0;
    fila->capacidade = capacidadeInicial;
    fila->proximo_protocolo = 1000;
}

void liberarFila(FilaOcorrencias *fila) {
    free(fila->dados);
    fila->dados = NULL;
    fila->tamanho = 0;
    fila->capacidade = 0;
}

static int garantirCapacidade(FilaOcorrencias *fila) {
    if (fila->tamanho == fila->capacidade) {
        fila->capacidade *= 2;
        Ocorrencia *temp = (Ocorrencia *)realloc(fila->dados, fila->capacidade * sizeof(Ocorrencia));
        if (temp == NULL) {
            printf("[ERRO] Falha ao expandir capacidade da fila na memoria!\n");
            return 0;
        }
        fila->dados = temp;
    }
    return 1;
}

// --- OPERAÇÕES DE FILA (FIFO) E CRUD ---

int enfileirarOcorrencia(FilaOcorrencias *fila, int id_morador, const char *categoria,
                         const char *titulo, const char *descricao,
                         const char *endereco, const char *data, int silencioso) {
    if (strlen(titulo) == 0 || strlen(descricao) == 0 || strlen(endereco) == 0) {
        if (!silencioso) {
            printf("[ERRO] Titulo, descricao e endereco do problema sao obrigatorios.\n");
        }
        return 0;
    }

    if (!garantirCapacidade(fila)) {
        return 0;
    }

    Ocorrencia nova;
    nova.id_ocorrencia = fila->proximo_protocolo;
    fila->proximo_protocolo += 10;
    nova.id_morador = id_morador;

    strncpy(nova.categoria, categoria, sizeof(nova.categoria) - 1);
    nova.categoria[sizeof(nova.categoria) - 1] = '\0';

    strncpy(nova.titulo, titulo, sizeof(nova.titulo) - 1);
    nova.titulo[sizeof(nova.titulo) - 1] = '\0';

    strncpy(nova.descricao, descricao, sizeof(nova.descricao) - 1);
    nova.descricao[sizeof(nova.descricao) - 1] = '\0';

    strncpy(nova.endereco_problema, endereco, sizeof(nova.endereco_problema) - 1);
    nova.endereco_problema[sizeof(nova.endereco_problema) - 1] = '\0';

    strncpy(nova.data_registro, data, sizeof(nova.data_registro) - 1);
    nova.data_registro[sizeof(nova.data_registro) - 1] = '\0';

    strcpy(nova.status, "Pendente");
    strcpy(nova.solucao_descricao, "Aguardando atendimento");

    fila->dados[fila->tamanho] = nova;
    fila->tamanho++;

    if (!silencioso) {
        printf("\n[OK] Ocorrencia registrada! Protocolo gerado: #%d (Status: %s)\n",
               nova.id_ocorrencia, nova.status);
    }
    return 1;
}

int desenfileirarOcorrencia(FilaOcorrencias *fila, Ocorrencia *removida) {
    if (fila->tamanho == 0) {
        return 0;
    }

    *removida = fila->dados[0];

    for (int i = 0; i < fila->tamanho - 1; i++) {
        fila->dados[i] = fila->dados[i + 1];
    }
    fila->tamanho--;
    return 1;
}

void listarOcorrencias(const FilaOcorrencias *fila) {
    if (fila->tamanho == 0) {
        printf("\nNenhuma ocorrencia na fila no momento.\n");
        return;
    }

    printf("\n=== FILA DE OCORRENCIAS (%d registros) ===\n", fila->tamanho);
    for (int i = 0; i < fila->tamanho; i++) {
        Ocorrencia o = fila->dados[i];
        printf("[%d] Protocolo #%d | %s | Cat: %s | Data: %s | Status: %s\n",
               i + 1, o.id_ocorrencia, o.titulo, o.categoria, o.data_registro, o.status);
        printf("    Local: %s | Morador ID: %d\n", o.endereco_problema, o.id_morador);
        printf("    Descricao: %s\n", o.descricao);
        printf("    Solucao: %s\n", o.solucao_descricao);
    }
    printf("==========================================\n");
}

// --- ALGORITMOS DE ORDENAÇÃO E BUSCA BINÁRIA ---

void ordenarPorProtocolo(FilaOcorrencias *fila) {
    for (int i = 0; i < fila->tamanho - 1; i++) {
        for (int j = 0; j < fila->tamanho - i - 1; j++) {
            if (fila->dados[j].id_ocorrencia > fila->dados[j + 1].id_ocorrencia) {
                Ocorrencia aux = fila->dados[j];
                fila->dados[j] = fila->dados[j + 1];
                fila->dados[j + 1] = aux;
            }
        }
    }
}

void ordenarPorTitulo(FilaOcorrencias *fila) {
    for (int i = 0; i < fila->tamanho - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < fila->tamanho; j++) {
            if (strcmp(fila->dados[j].titulo, fila->dados[min_idx].titulo) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Ocorrencia aux = fila->dados[i];
            fila->dados[i] = fila->dados[min_idx];
            fila->dados[min_idx] = aux;
        }
    }
}

int buscaBinariaProtocolo(FilaOcorrencias *fila, int protocolo) {
    ordenarPorProtocolo(fila);

    int inicio = 0;
    int fim = fila->tamanho - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;

        if (fila->dados[meio].id_ocorrencia == protocolo) {
            return meio;
        }
        if (fila->dados[meio].id_ocorrencia < protocolo) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    return -1;
}

// --- UPDATE E DELETE (COM TRAVAS DA ESTÓRIA 4) ---

void atualizarStatusOcorrencia(FilaOcorrencias *fila, int protocolo) {
    int idx = buscaBinariaProtocolo(fila, protocolo);
    if (idx == -1) {
        printf("\n[ERRO] Protocolo #%d nao encontrado.\n", protocolo);
        return;
    }

    Ocorrencia *oc = &fila->dados[idx];

    if (strcmp(oc->status, "Concluido") == 0 || strcmp(oc->status, "Cancelado") == 0) {
        printf("\n[BLOQUEADO] Ocorrencia #%d ja esta '%s' e nao pode ser alterada.\n",
               oc->id_ocorrencia, oc->status);
        return;
    }

    printf("\nStatus atual: %s\n", oc->status);
    printf("Escolha o novo status:\n");
    printf("1 - Em Andamento\n");
    printf("2 - Concluido\n");
    printf("3 - Cancelado\n");
    printf("Opcao: ");
    int op;
    if (scanf("%d", &op) != 1) {
        limparBuffer();
        printf("[ERRO] Entrada invalida.\n");
        return;
    }
    limparBuffer();

    if (op == 1) {
        strcpy(oc->status, "Em Andamento");
        printf("[OK] Status atualizado para 'Em Andamento'.\n");
    } else if (op == 2) {
        char solucao[MAX_DESC];
        lerStringObrigatoria("Descreva a solucao aplicada (obrigatorio): ", solucao, MAX_DESC);
        strcpy(oc->solucao_descricao, solucao);
        strcpy(oc->status, "Concluido");
        printf("[OK] Ocorrencia #%d finalizada como 'Concluido'!\n", oc->id_ocorrencia);
    } else if (op == 3) {
        strcpy(oc->status, "Cancelado");
        strcpy(oc->solucao_descricao, "Ocorrencia cancelada pela administracao.");
        printf("[OK] Ocorrencia #%d cancelada.\n", oc->id_ocorrencia);
    } else {
        printf("[ERRO] Opcao invalida.\n");
    }
}

void removerOcorrenciaPorProtocolo(FilaOcorrencias *fila, int protocolo) {
    int idx = buscaBinariaProtocolo(fila, protocolo);
    if (idx == -1) {
        printf("\n[ERRO] Protocolo #%d nao encontrado.\n", protocolo);
        return;
    }

    for (int i = idx; i < fila->tamanho - 1; i++) {
        fila->dados[i] = fila->dados[i + 1];
    }
    fila->tamanho--;
    printf("\n[OK] Ocorrencia #%d removida do sistema.\n", protocolo);
}

// --- PERSISTÊNCIA EM ARQUIVO .TXT (DISCO <-> MEMÓRIA RAM) ---

void salvarFilaEmArquivo(const FilaOcorrencias *fila, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel abrir '%s' para gravacao.\n", nomeArquivo);
        return;
    }

    for (int i = 0; i < fila->tamanho; i++) {
        Ocorrencia o = fila->dados[i];
        fprintf(arquivo, "%d|%d|%s|%s|%s|%s|%s|%s|%s\n",
                o.id_ocorrencia,
                o.id_morador,
                o.categoria,
                o.titulo,
                o.descricao,
                o.endereco_problema,
                o.data_registro,
                o.status,
                o.solucao_descricao);
    }

    fclose(arquivo);
    printf("[PERSISTENCIA] %d registro(s) salvos com sucesso em '%s'.\n",
           fila->tamanho, nomeArquivo);
}

void carregarFilaDeArquivo(FilaOcorrencias *fila, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("[INFO] Arquivo '%s' nao encontrado. Carregando base inicial de exemplo...\n", nomeArquivo);
        carregarDadosExemplo(fila);
        return;
    }

    char linha[1024];
    int maior_protocolo = 990;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        size_t len = strlen(linha);
        if (len > 0 && linha[len - 1] == '\n') {
            linha[len - 1] = '\0';
        }
        if (strlen(linha) == 0) continue;

        char *tokens[9];
        int count = 0;
        char *ptr = strtok(linha, "|");
        while (ptr != NULL && count < 9) {
            tokens[count++] = ptr;
            ptr = strtok(NULL, "|");
        }

        if (count == 9 && garantirCapacidade(fila)) {
            Ocorrencia oc;
            oc.id_ocorrencia = atoi(tokens[0]);
            oc.id_morador = atoi(tokens[1]);

            strncpy(oc.categoria, tokens[2], sizeof(oc.categoria) - 1);
            oc.categoria[sizeof(oc.categoria) - 1] = '\0';

            strncpy(oc.titulo, tokens[3], sizeof(oc.titulo) - 1);
            oc.titulo[sizeof(oc.titulo) - 1] = '\0';

            strncpy(oc.descricao, tokens[4], sizeof(oc.descricao) - 1);
            oc.descricao[sizeof(oc.descricao) - 1] = '\0';

            strncpy(oc.endereco_problema, tokens[5], sizeof(oc.endereco_problema) - 1);
            oc.endereco_problema[sizeof(oc.endereco_problema) - 1] = '\0';

            strncpy(oc.data_registro, tokens[6], sizeof(oc.data_registro) - 1);
            oc.data_registro[sizeof(oc.data_registro) - 1] = '\0';

            strncpy(oc.status, tokens[7], sizeof(oc.status) - 1);
            oc.status[sizeof(oc.status) - 1] = '\0';

            strncpy(oc.solucao_descricao, tokens[8], sizeof(oc.solucao_descricao) - 1);
            oc.solucao_descricao[sizeof(oc.solucao_descricao) - 1] = '\0';

            fila->dados[fila->tamanho] = oc;
            fila->tamanho++;

            if (oc.id_ocorrencia > maior_protocolo) {
                maior_protocolo = oc.id_ocorrencia;
            }
        }
    }

    fila->proximo_protocolo = maior_protocolo + 10;
    fclose(arquivo);
    printf("[PERSISTENCIA] %d registro(s) carregados de '%s' para a memoria.\n",
           fila->tamanho, nomeArquivo);
}

void carregarDadosExemplo(FilaOcorrencias *fila) {
    enfileirarOcorrencia(fila, 1, "Iluminacao publica", "Poste apagado na Rua das Flores",
                         "Poste em frente a praca apagado ha uma semana.", "Rua das Flores, 120", "18/11/2026", 1);
    enfileirarOcorrencia(fila, 2, "Buracos em vias", "Buraco grande na Av. Principal",
                         "Buraco de 1 metro no meio da pista.", "Av. Principal, 845", "17/11/2026", 1);
    enfileirarOcorrencia(fila, 1, "Descarte de lixo", "Lixo acumulado na praca central",
                         "Entulho descartado irregularmente na calcada.", "Praca Central, s/n", "15/11/2026", 1);
}