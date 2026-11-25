#include <stdio.h>
#include <string.h>
#include "../include/fila_ocorrencias.h"

int main(void) {
    FilaOcorrencias fila;
    inicializarFila(&fila, 5);
    carregarFilaDeArquivo(&fila, ARQUIVO_DADOS);

    int opcao;
    do {
        printf("\n=== AMIGODAVIZINHANCA - GESTAO DE FILA (C) ===\n");
        printf("1. Registrar Ocorrencia (Enfileirar / Create)\n");
        printf("2. Listar Fila de Ocorrencias (Read)\n");
        printf("3. Despachar Proxima da Fila (Desenfileirar FIFO)\n");
        printf("4. Buscar Ocorrencia por Protocolo (Busca Binaria)\n");
        printf("5. Atualizar Status / Concluir Ocorrencia (Update)\n");
        printf("6. Remover Ocorrencia por Protocolo (Delete)\n");
        printf("7. Ordenar por Titulo (Selection Sort)\n");
        printf("8. Reordenar por Protocolo (Bubble Sort)\n");
        printf("9. Salvar Backup Manual em TXT\n");
        printf("0. Salvar e Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("[ERRO] Digite apenas o numero da opcao.\n");
            continue;
        }
        limparBuffer();

        if (opcao == 1) {
            int id_morador, cat_op;
            char categoria[50], titulo[MAX_STR], descricao[MAX_DESC], endereco[MAX_STR], data[20];

            printf("ID do Morador: ");
            if (scanf("%d", &id_morador) != 1) {
                limparBuffer();
                printf("[ERRO] ID de morador invalido.\n");
                continue;
            }
            limparBuffer();

            printf("Categoria (1-Iluminacao publica | 2-Buracos em vias | 3-Descarte de lixo): ");
            if (scanf("%d", &cat_op) != 1) {
                limparBuffer();
                printf("[ERRO] Categoria invalida.\n");
                continue;
            }
            limparBuffer();

            if (cat_op == 1) strcpy(categoria, "Iluminacao publica");
            else if (cat_op == 2) strcpy(categoria, "Buracos em vias");
            else if (cat_op == 3) strcpy(categoria, "Descarte de lixo");
            else {
                printf("[ERRO] Opcao de categoria inexistente.\n");
                continue;
            }

            lerStringObrigatoria("Titulo do problema: ", titulo, MAX_STR);
            lerStringObrigatoria("Descricao detalhada: ", descricao, MAX_DESC);
            lerStringObrigatoria("Endereco do problema: ", endereco, MAX_STR);
            lerStringObrigatoria("Data de registro (dd/mm/aaaa): ", data, 20);

            enfileirarOcorrencia(&fila, id_morador, categoria, titulo, descricao, endereco, data, 0);

        } else if (opcao == 2) {
            listarOcorrencias(&fila);

        } else if (opcao == 3) {
            Ocorrencia atendida;
            if (desenfileirarOcorrencia(&fila, &atendida)) {
                printf("\n[DESPACHADA - FIFO] Protocolo #%d (%s) saiu da fila de espera!\n",
                       atendida.id_ocorrencia, atendida.titulo);
            } else {
                printf("\nFila vazia! Nenhuma ocorrencia para despachar.\n");
            }

        } else if (opcao == 4) {
            int prot;
            printf("Digite o numero do protocolo (ex: 1000, 1010): ");
            if (scanf("%d", &prot) != 1) {
                limparBuffer();
                printf("[ERRO] Protocolo invalido.\n");
                continue;
            }
            limparBuffer();

            int idx = buscaBinariaProtocolo(&fila, prot);
            if (idx != -1) {
                Ocorrencia o = fila.dados[idx];
                printf("\n[ENCONTRADA] Protocolo #%d | Titulo: %s | Status: %s | Local: %s\n",
                       o.id_ocorrencia, o.titulo, o.status, o.endereco_problema);
            } else {
                printf("\n[NAO ENCONTRADA] Protocolo #%d inexistente.\n", prot);
            }

        } else if (opcao == 5) {
            int prot;
            printf("Digite o protocolo da ocorrencia para atualizar: ");
            if (scanf("%d", &prot) != 1) {
                limparBuffer();
                printf("[ERRO] Protocolo invalido.\n");
                continue;
            }
            limparBuffer();
            atualizarStatusOcorrencia(&fila, prot);

        } else if (opcao == 6) {
            int prot;
            printf("Digite o protocolo da ocorrencia para remover: ");
            if (scanf("%d", &prot) != 1) {
                limparBuffer();
                printf("[ERRO] Protocolo invalido.\n");
                continue;
            }
            limparBuffer();
            removerOcorrenciaPorProtocolo(&fila, prot);

        } else if (opcao == 7) {
            ordenarPorTitulo(&fila);
            printf("\n[OK] Ocorrencias ordenadas alfabeticamente por titulo!\n");
            listarOcorrencias(&fila);

        } else if (opcao == 8) {
            ordenarPorProtocolo(&fila);
            printf("\n[OK] Ocorrencias reordenadas por numero de protocolo!\n");
            listarOcorrencias(&fila);

        } else if (opcao == 9) {
            salvarFilaEmArquivo(&fila, ARQUIVO_DADOS);
        }

    } while (opcao != 0);

    salvarFilaEmArquivo(&fila, ARQUIVO_DADOS);
    liberarFila(&fila);
    return 0;
}