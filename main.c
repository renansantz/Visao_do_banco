#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "meu_banco.h"

int main() {
    int opcao;

    srand(time(NULL));

    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1 - Abrir o banco\n");
        printf("2 - Consultar historico\n");
        printf("3 - Efetuar sorteio\n");
        printf("4 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                abrirBanco();
                break;
            case 2:
                if (bancoAberto == 1) consultarHistorico();
                else printf("O banco precisa estar aberto!\n");
                break;
            case 3:
                if (bancoAberto == 1) efetuarSorteio();
                else printf("O banco precisa estar aberto!\n");
                break;
            case 4:
                printf("Saindo do sistema...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 4);

    return 0;
}
