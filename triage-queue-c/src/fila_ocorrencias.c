#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "../include/fila_ocorrencias.h"

// Captura data e hora atual no formato dd/mm/aaaa HH:MM (Estorias 2, 3 e 5)
static void obterDataHoraAtual(char *destino, int tamanho) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(destino, tamanho, "%d/%m/%Y %H:%M", tm_info);
}

// Converte "dd/mm/aaaa HH:MM" em numero inteiro comparavel para ordenacao
static long long converterDataHoraParaNumero(const char *data_hora) {
    int d = 0, m = 0, a = 0, h = 0, min = 0;
    if (sscanf(data_hora, "%d/%d/%d %d:%d", &d, &m, &a, &h, &min) >= 3) {
        return ((long long)a * 100000000LL) +
               ((long long)m * 1000000LL) +
               ((long long)d * 10000LL) +
               ((long long)h * 100LL) + min;
    }
    return 0;
}

void lerStringObrigatoria(const char *prompt, char *destino, int tamanho_max) {
    while (1) {
        printf("%s", prompt);
        if (fgets(destino, tamanho_max, stdin) != NULL) {
            destino[strcspn(destino, "\n")] = '\0';

            bool apenas_espacos = true;
            for (int i = 0; destino[i] != '\0'; i++) {
                if (!isspace((unsigned char)destino[i])) {
                    apenas_espacos = false;
                    break;
                }
            }

            if (strlen(destino) > 0 && !apenas_espacos) {
                break;
            }
        }
        printf("[ERRO] Campo obrigatorio! Nao pode ficar vazio.\n");
    }
}

FilaOcorrencias* criarFila(int capacidade_inicial) {
    FilaOcorrencias *fila = (FilaOcorrencias*) malloc(sizeof(FilaOcorrencias));
    if (fila == NULL) {
        printf("[ERRO CRITICO] Falha ao alocar memoria para a fila.\n");
        return NULL;
    }

    fila->dados = (Ocorrencia*) malloc(capacidade_inicial * sizeof(Ocorrencia));
    if (fila->dados == NULL) {
        free(fila);
        printf("[ERRO CRITICO] Falha ao alocar vetor de ocorrencias.\n");
        return NULL;
    }

    fila->tamanho = 0;
    fila->capacidade = capacidade_inicial;
    fila->proximo_protocolo = 1000;
    return fila;
}

void liberarFila(FilaOcorrencias *fila) {
    if (fila != NULL) {
        if (fila->dados != NULL) {
            free(fila->dados);
        }
        free(fila);
    }
}

bool enfileirarOcorrencia(FilaOcorrencias *fila, Ocorrencia nova) {
    if (fila == NULL) return false;

    if (fila->tamanho == fila->capacidade) {
        int nova_capacidade = fila->capacidade * 2;
        Ocorrencia *temp = (Ocorrencia*) realloc(fila->dados, nova_capacidade * sizeof(Ocorrencia));
        if (temp == NULL) {
            printf("[ERRO] Memoria insuficiente para expandir a fila.\n");
            return false;
        }
        fila->dados = temp;
        fila->capacidade = nova_capacidade;
    }

    // Valores automaticos definidos pela Estoria 2
    nova.id_ocorrencia = fila->proximo_protocolo;
    fila->proximo_protocolo += 10;
    nova.id_equipe = 0;

    obterDataHoraAtual(nova.data_registro, sizeof(nova.data_registro));
    strcpy(nova.status, "Pendente");
    strcpy(nova.prioridade, "Nao definida");
    strcpy(nova.data_atribuicao, "Nao atribuida");
    strcpy(nova.prazo_estimado, "Nao definido");
    strcpy(nova.solucao_descricao, "Nenhuma solucao registrada");
    strcpy(nova.data_conclusao, "Pendente");

    fila->dados[fila->tamanho] = nova;
    fila->tamanho++;
    return true;
}

bool desenfileirarOcorrencia(FilaOcorrencias *fila, Ocorrencia *removida) {
    if (fila == NULL || fila->tamanho == 0) {
        return false;
    }

    if (removida != NULL) {
        *removida = fila->dados[0];
    }

    for (int i = 0; i < fila->tamanho - 1; i++) {
        fila->dados[i] = fila->dados[i + 1];
    }

    fila->tamanho--;
    return true;
}

void exibirDetalhesOcorrencia(const Ocorrencia *oc) {
    if (oc == NULL) return;
    printf("\n----------------------------------------------------\n");
    printf("Protocolo:       #%d\n", oc->id_ocorrencia);
    printf("Titulo:          %s\n", oc->titulo);
    printf("Status:          %s | Prioridade: %s\n", oc->status, oc->prioridade);
    printf("Morador ID:      %d | Categoria ID: %d | Equipe ID: %d\n",
           oc->id_morador, oc->id_categoria, oc->id_equipe);
    printf("Endereco:        %s\n", oc->endereco_problema);
    printf("Data Registro:   %s\n", oc->data_registro);
    printf("Data Atribuicao: %s | Prazo Estimado: %s\n",
           oc->data_atribuicao, oc->prazo_estimado);
    printf("Data Conclusao:  %s\n", oc->data_conclusao);
    printf("Descricao:       %s\n", oc->descricao);
    printf("Solucao:         %s\n", oc->solucao_descricao);
    printf("----------------------------------------------------\n");
}

void listarOcorrencias(const FilaOcorrencias *fila) {
    if (fila == NULL || fila->tamanho == 0) {
        printf("\n[AVISO] Nenhuma ocorrencia cadastrada na fila.\n");
        return;
    }

    printf("\n=== LISTA DE OCORRENCIAS (%d registros) ===\n", fila->tamanho);
    for (int i = 0; i < fila->tamanho; i++) {
        printf("[%d] Prot: #%d | %s | Cat: %d | Status: %s | Prioridade: %s | Reg: %s\n",
               i + 1,
               fila->dados[i].id_ocorrencia,
               fila->dados[i].titulo,
               fila->dados[i].id_categoria,
               fila->dados[i].status,
               fila->dados[i].prioridade,
               fila->dados[i].data_registro);
    }
}

// Estoria 6: Consulta filtrada por Categoria ou Status
void filtrarOcorrencias(const FilaOcorrencias *fila) {
    if (fila == NULL || fila->tamanho == 0) {
        printf("\n[AVISO] Nenhuma ocorrencia cadastrada.\n");
        return;
    }

    printf("\n--- FILTRAR OCORRENCIAS (ESTORIA 6) ---\n");
    printf("1 - Filtrar por Categoria (ID)\n");
    printf("2 - Filtrar por Status\n");
    printf("Opcao: ");

    int op;
    if (scanf("%d", &op) != 1) {
        while (getchar() != '\n');
        printf("[ERRO] Opcao invalida.\n");
        return;
    }
    while (getchar() != '\n');

    int encontrados = 0;

    if (op == 1) {
        int cat_id;
        printf("Digite o ID da Categoria: ");
        if (scanf("%d", &cat_id) != 1) {
            while (getchar() != '\n');
            printf("[ERRO] ID invalido.\n");
            return;
        }
        while (getchar() != '\n');

        printf("\n=== OCORRENCIAS DA CATEGORIA %d ===\n", cat_id);
        for (int i = 0; i < fila->tamanho; i++) {
            if (fila->dados[i].id_categoria == cat_id) {
                printf("Prot: #%d | %s | Status: %s | Reg: %s\n",
                       fila->dados[i].id_ocorrencia,
                       fila->dados[i].titulo,
                       fila->dados[i].status,
                       fila->dados[i].data_registro);
                encontrados++;
            }
        }
    } else if (op == 2) {
        char status_busca[20];
        lerStringObrigatoria("Digite o Status (Pendente, Em Andamento, Concluido, Cancelado): ",
                             status_busca, sizeof(status_busca));

        printf("\n=== OCORRENCIAS COM STATUS '%s' ===\n", status_busca);
        for (int i = 0; i < fila->tamanho; i++) {
            if (strcmp(fila->dados[i].status, status_busca) == 0) {
                printf("Prot: #%d | %s | Prioridade: %s | Reg: %s\n",
                       fila->dados[i].id_ocorrencia,
                       fila->dados[i].titulo,
                       fila->dados[i].prioridade,
                       fila->dados[i].data_registro);
                encontrados++;
            }
        }
    } else {
        printf("[ERRO] Opcao invalida.\n");
        return;
    }

    if (encontrados == 0) {
        printf("[AVISO] Nenhuma ocorrencia encontrada para esse filtro.\n");
    }
}

void atualizarStatusOcorrencia(FilaOcorrencias *fila, int protocolo) {
    int idx = buscarPorProtocoloBinaria(fila, protocolo);
    if (idx == -1) {
        printf("[ERRO] Protocolo #%d nao encontrado.\n", protocolo);
        return;
    }

    Ocorrencia *oc = &fila->dados[idx];

    // Regra Estoria 5: Imutabilidade de ocorrencias finalizadas
    if (strcmp(oc->status, "Concluido") == 0 || strcmp(oc->status, "Cancelado") == 0) {
        printf("[BLOQUEADO] Ocorrencia #%d ja esta '%s' e nao pode ser alterada.\n",
               oc->id_ocorrencia, oc->status);
        return;
    }

    printf("\nOcorrencia #%d - Status atual: %s | Prioridade atual: %s\n",
           oc->id_ocorrencia, oc->status, oc->prioridade);
    printf("Escolha a acao (Fluxo: Pendente -> Em Andamento -> Concluido/Cancelado):\n");
    printf("1 - Atribuir Equipe e Iniciar Atendimento (Em Andamento - Estoria 3)\n");
    printf("2 - Finalizar Ocorrencia com Solucao (Concluido - Estoria 5)\n");
    printf("3 - Cancelar Ocorrencia (Cancelado - Estoria 5)\n");
    printf("Opcao: ");

    int op;
    if (scanf("%d", &op) != 1) {
        while (getchar() != '\n');
        printf("[ERRO] Entrada invalida.\n");
        return;
    }
    while (getchar() != '\n');

    if (op == 1) {
        // Estoria 3: Atribuicao de equipe, prazo, prioridade e mudanca para Em Andamento
        printf("ID da Equipe de Manutencao responsavel: ");
        if (scanf("%d", &oc->id_equipe) != 1 || oc->id_equipe <= 0) {
            while (getchar() != '\n');
            printf("[ERRO] ID de equipe invalido.\n");
            return;
        }
        while (getchar() != '\n');

        lerStringObrigatoria("Prazo estimado de conclusao (dd/mm/aaaa): ",
                             oc->prazo_estimado, sizeof(oc->prazo_estimado));

        printf("Prioridade da atribuicao (1-Baixa | 2-Media | 3-Alta): ");
        int op_prio;
        if (scanf("%d", &op_prio) == 1) {
            while (getchar() != '\n');
            if (op_prio == 1) strcpy(oc->prioridade, "Baixa");
            else if (op_prio == 3) strcpy(oc->prioridade, "Alta");
            else strcpy(oc->prioridade, "Media");
        } else {
            while (getchar() != '\n');
            strcpy(oc->prioridade, "Media");
        }

        obterDataHoraAtual(oc->data_atribuicao, sizeof(oc->data_atribuicao));
        strcpy(oc->status, "Em Andamento");
        printf("[SUCESSO] Equipe #%d atribuida em %s. Status: 'Em Andamento' (Prioridade: %s).\n",
               oc->id_equipe, oc->data_atribuicao, oc->prioridade);

    } else if (op == 2) {
        // Regra Estoria 5: Exige passagem por 'Em Andamento' antes de 'Concluido'
        if (strcmp(oc->status, "Em Andamento") != 0) {
            printf("[BLOQUEADO] Fluxo invalido! A ocorrencia precisa estar 'Em Andamento' antes de ser concluida.\n");
            return;
        }

        lerStringObrigatoria("Descreva a solucao aplicada (obrigatorio): ",
                             oc->solucao_descricao, sizeof(oc->solucao_descricao));
        strcpy(oc->status, "Concluido");
        obterDataHoraAtual(oc->data_conclusao, sizeof(oc->data_conclusao));
        printf("[SUCESSO] Ocorrencia #%d concluida em %s!\n", oc->id_ocorrencia, oc->data_conclusao);

    } else if (op == 3) {
        strcpy(oc->status, "Cancelado");
        printf("[SUCESSO] Ocorrencia #%d cancelada.\n", oc->id_ocorrencia);
    } else {
        printf("[ERRO] Opcao invalida.\n");
    }
}

void ordenarPorProtocolo(FilaOcorrencias *fila) {
    if (fila == NULL || fila->tamanho < 2) return;

    for (int i = 0; i < fila->tamanho - 1; i++) {
        bool trocou = false;
        for (int j = 0; j < fila->tamanho - i - 1; j++) {
            if (fila->dados[j].id_ocorrencia > fila->dados[j + 1].id_ocorrencia) {
                Ocorrencia aux = fila->dados[j];
                fila->dados[j] = fila->dados[j + 1];
                fila->dados[j + 1] = aux;
                trocou = true;
            }
        }
        if (!trocou) break;
    }
}

void ordenarPorTitulo(FilaOcorrencias *fila) {
    if (fila == NULL || fila->tamanho < 2) return;

    for (int i = 0; i < fila->tamanho - 1; i++) {
        int menor_idx = i;
        for (int j = i + 1; j < fila->tamanho; j++) {
            if (strcmp(fila->dados[j].titulo, fila->dados[menor_idx].titulo) < 0) {
                menor_idx = j;
            }
        }
        if (menor_idx != i) {
            Ocorrencia aux = fila->dados[i];
            fila->dados[i] = fila->dados[menor_idx];
            fila->dados[menor_idx] = aux;
        }
    }
}

// Estoria 6: Selection Sort por data de registro (mais recentes primeiro)
void ordenarPorDataRecente(FilaOcorrencias *fila) {
    if (fila == NULL || fila->tamanho < 2) return;

    for (int i = 0; i < fila->tamanho - 1; i++) {
        int mais_recente_idx = i;
        long long data_mais_recente = converterDataHoraParaNumero(fila->dados[i].data_registro);

        for (int j = i + 1; j < fila->tamanho; j++) {
            long long data_atual = converterDataHoraParaNumero(fila->dados[j].data_registro);
            if (data_atual > data_mais_recente) {
                mais_recente_idx = j;
                data_mais_recente = data_atual;
            }
        }

        if (mais_recente_idx != i) {
            Ocorrencia aux = fila->dados[i];
            fila->dados[i] = fila->dados[mais_recente_idx];
            fila->dados[mais_recente_idx] = aux;
        }
    }
}

int buscarPorProtocoloBinaria(FilaOcorrencias *fila, int protocolo) {
    if (fila == NULL || fila->tamanho == 0) return -1;

    ordenarPorProtocolo(fila);

    int inicio = 0;
    int fim = fila->tamanho - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;

        if (fila->dados[meio].id_ocorrencia == protocolo) {
            return meio;
        } else if (fila->dados[meio].id_ocorrencia < protocolo) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    return -1;
}

bool salvarFilaEmArquivo(const FilaOcorrencias *fila, const char *caminho) {
    if (fila == NULL) return false;

    FILE *arq = fopen(caminho, "w");
    if (arq == NULL) {
        printf("[ERRO] Nao foi possivel abrir '%s' para gravacao.\n", caminho);
        return false;
    }

    fprintf(arq, "PROXIMO_PROTOCOLO|%d\n", fila->proximo_protocolo);

    for (int i = 0; i < fila->tamanho; i++) {
        Ocorrencia *oc = &fila->dados[i];
        fprintf(arq, "%d|%d|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
                oc->id_ocorrencia,
                oc->id_morador,
                oc->id_categoria,
                oc->id_equipe,
                oc->titulo,
                oc->descricao,
                oc->endereco_problema,
                oc->data_registro,
                oc->status,
                oc->prioridade,
                oc->data_atribuicao,
                oc->prazo_estimado,
                oc->solucao_descricao,
                oc->data_conclusao);
    }

    fclose(arq);
    return true;
}

bool carregarFilaDeArquivo(FilaOcorrencias *fila, const char *caminho) {
    if (fila == NULL) return false;

    FILE *arq = fopen(caminho, "r");
    if (arq == NULL) {
        return false;
    }

    char linha[1500];

    if (fgets(linha, sizeof(linha), arq) != NULL) {
        sscanf(linha, "PROXIMO_PROTOCOLO|%d", &fila->proximo_protocolo);
    }

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        Ocorrencia oc;

        char *tokens[14];
        int count = 0;
        char *ptr = strtok(linha, "|");
        while (ptr != NULL && count < 14) {
            tokens[count++] = ptr;
            ptr = strtok(NULL, "|");
        }

        if (count == 14) {
            oc.id_ocorrencia = atoi(tokens[0]);
            oc.id_morador = atoi(tokens[1]);
            oc.id_categoria = atoi(tokens[2]);
            oc.id_equipe = atoi(tokens[3]);

            strncpy(oc.titulo, tokens[4], sizeof(oc.titulo) - 1);
            oc.titulo[sizeof(oc.titulo) - 1] = '\0';

            strncpy(oc.descricao, tokens[5], sizeof(oc.descricao) - 1);
            oc.descricao[sizeof(oc.descricao) - 1] = '\0';

            strncpy(oc.endereco_problema, tokens[6], sizeof(oc.endereco_problema) - 1);
            oc.endereco_problema[sizeof(oc.endereco_problema) - 1] = '\0';

            strncpy(oc.data_registro, tokens[7], sizeof(oc.data_registro) - 1);
            oc.data_registro[sizeof(oc.data_registro) - 1] = '\0';

            strncpy(oc.status, tokens[8], sizeof(oc.status) - 1);
            oc.status[sizeof(oc.status) - 1] = '\0';

            strncpy(oc.prioridade, tokens[9], sizeof(oc.prioridade) - 1);
            oc.prioridade[sizeof(oc.prioridade) - 1] = '\0';

            strncpy(oc.data_atribuicao, tokens[10], sizeof(oc.data_atribuicao) - 1);
            oc.data_atribuicao[sizeof(oc.data_atribuicao) - 1] = '\0';

            strncpy(oc.prazo_estimado, tokens[11], sizeof(oc.prazo_estimado) - 1);
            oc.prazo_estimado[sizeof(oc.prazo_estimado) - 1] = '\0';

            strncpy(oc.solucao_descricao, tokens[12], sizeof(oc.solucao_descricao) - 1);
            oc.solucao_descricao[sizeof(oc.solucao_descricao) - 1] = '\0';

            strncpy(oc.data_conclusao, tokens[13], sizeof(oc.data_conclusao) - 1);
            oc.data_conclusao[sizeof(oc.data_conclusao) - 1] = '\0';

            if (fila->tamanho == fila->capacidade) {
                int nova_cap = fila->capacidade * 2;
                Ocorrencia *temp = (Ocorrencia*) realloc(fila->dados, nova_cap * sizeof(Ocorrencia));
                if (temp == NULL) {
                    printf("[ERRO] Memoria insuficiente ao carregar arquivo.\n");
                    break;
                }
                fila->dados = temp;
                fila->capacidade = nova_cap;
            }
            fila->dados[fila->tamanho++] = oc;
        }
    }

    fclose(arq);
    return true;
}