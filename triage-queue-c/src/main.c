#include <stdio.h>
#include <stdlib.h>
#include "../include/fila_ocorrencias.h"

#define ARQUIVO_DADOS "ocorrencias.txt"

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int lerInteiro(const char *prompt) {
    int valor;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &valor) == 1) {
            limparBuffer();
            return valor;
        }
        limparBuffer();
        printf("[ERRO] Digite um numero inteiro valido.\n");
    }
}

void exibirMenu() {
    printf("\n====================================================\n");
    printf("   AMIGO DA VIZINHANCA - MODULO DE TRIAGEM (C)\n");
    printf("====================================================\n");
    printf("1. Registrar Nova Ocorrencia (Enfileirar - Estoria 2)\n");
    printf("2. Despachar Proxima Ocorrencia da Fila (FIFO)\n");
    printf("3. Listar Todas as Ocorrencias (Estoria 6)\n");
    printf("4. Filtrar Ocorrencias por Categoria/Status (Estoria 6)\n");
    printf("5. Buscar Ocorrencia por Protocolo (Busca Binaria)\n");
    printf("6. Atribuir Equipe / Atualizar Status / Concluir (Est. 3/5)\n");
    printf("7. Ordenar Ocorrencias por Protocolo (Bubble Sort)\n");
    printf("8. Ordenar Ocorrencias por Titulo (Selection Sort)\n");
    printf("9. Ordenar por Data Mais Recente (Estoria 6)\n");
    printf("10. Salvar Backup Manual (.txt)\n");
    printf("0. Salvar e Sair\n");
    printf("====================================================\n");
}

int main() {
    FilaOcorrencias *fila = criarFila(5);
    if (fila == NULL) return 1;

    if (carregarFilaDeArquivo(fila, ARQUIVO_DADOS)) {
        printf("[SISTEMA] %d ocorrencia(s) carregada(s) de '%s'.\n", fila->tamanho, ARQUIVO_DADOS);
    } else {
        printf("[SISTEMA] Nenhum arquivo anterior encontrado. Iniciando fila vazia.\n");
    }

    int opcao;
    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: {
                Ocorrencia nova;
                printf("\n--- NOVO REGISTRO DE OCORRENCIA (ESTORIA 2) ---\n");
                nova.id_morador = lerInteiro("ID do Morador solicitante: ");
                nova.id_categoria = lerInteiro("ID da Categoria (1-Iluminacao, 2-Via, 3-Lixo): ");
                lerStringObrigatoria("Titulo do problema: ", nova.titulo, sizeof(nova.titulo));
                lerStringObrigatoria("Descricao detalhada: ", nova.descricao, sizeof(nova.descricao));
                lerStringObrigatoria("Endereco do problema: ", nova.endereco_problema, sizeof(nova.endereco_problema));

                if (enfileirarOcorrencia(fila, nova)) {
                    Ocorrencia *salva = &fila->dados[fila->tamanho - 1];
                    printf("\n[SUCESSO] Ocorrencia registrada em %s! Protocolo: #%d\n",
                           salva->data_registro, salva->id_ocorrencia);
                }
                break;
            }
            case 2: {
                Ocorrencia atendida;
                if (desenfileirarOcorrencia(fila, &atendida)) {
                    printf("\n[TRIAGEM] Proxima ocorrencia removida da fila para despacho:\n");
                    exibirDetalhesOcorrencia(&atendida);
                } else {
                    printf("\n[AVISO] A fila de triagem esta vazia.\n");
                }
                break;
            }
            case 3:
                listarOcorrencias(fila);
                break;

            case 4:
                filtrarOcorrencias(fila);
                break;

            case 5: {
                int prot = lerInteiro("\nDigite o numero do protocolo para buscar: ");
                int idx = buscarPorProtocoloBinaria(fila, prot);
                if (idx != -1) {
                    printf("\n[ENCONTRADO] Registro localizado via Busca Binaria:\n");
                    exibirDetalhesOcorrencia(&fila->dados[idx]);
                } else {
                    printf("\n[AVISO] Protocolo #%d nao encontrado.\n", prot);
                }
                break;
            }
            case 6: {
                int prot = lerInteiro("\nDigite o protocolo da ocorrencia para gerenciar: ");
                atualizarStatusOcorrencia(fila, prot);
                break;
            }
            case 7:
                ordenarPorProtocolo(fila);
                printf("\n[ORDENADO] Lista ordenada por Protocolo (Bubble Sort).\n");
                listarOcorrencias(fila);
                break;

            case 8:
                ordenarPorTitulo(fila);
                printf("\n[ORDENADO] Lista ordenada alfabeticamente por Titulo (Selection Sort).\n");
                listarOcorrencias(fila);
                break;

            case 9:
                ordenarPorDataRecente(fila);
                printf("\n[ORDENADO] Lista ordenada por Data Mais Recente (Estoria 6).\n");
                listarOcorrencias(fila);
                break;

            case 10:
                if (salvarFilaEmArquivo(fila, ARQUIVO_DADOS)) {
                    printf("\n[BACKUP] Dados salvos com sucesso em '%s'.\n", ARQUIVO_DADOS);
                }
                break;

            case 0:
                salvarFilaEmArquivo(fila, ARQUIVO_DADOS);
                printf("\n[SISTEMA] Dados salvos. Liberando memoria Heap e encerrando...\n");
                break;

            default:
                printf("\n[ERRO] Opcao invalida.\n");
        }
    } while (opcao != 0);

    liberarFila(fila);
    return 0;
}