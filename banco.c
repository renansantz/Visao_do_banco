#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meu_banco.h"

/* variaveis globais do banco (declaradas com extern no meu_banco.h) */
struct Cliente clientes[MAX_CLIENTES];
int qtdClientes = 0;
int bancoAberto = 0;

/* guarda quem ganhou o sorteio */
int qtdContemplados = 0;
int contemplado[MAX_CLIENTES];      /* posicao do cliente no vetor */
float premioContemplado[MAX_CLIENTES];


/* ===================== FUNCOES AUXILIARES ===================== */

float saldoTotalCliente(int i) {
    return clientes[i].saldoCorrente + clientes[i].saldoSalario + clientes[i].saldoPoupanca;
}

/* Limite do emprestimo: 25% de dez vezes o saldo total */
float limiteEmprestimo(int i) {
    return 0.25 * (10 * saldoTotalCliente(i));
}

/* Pode pedir emprestimo: tem conta corrente, tem saldo na poupanca
   e ainda nao fez nenhum emprestimo */
int aptoEmprestimo(int i) {
    if (clientes[i].temCorrente == 1 && clientes[i].temPoupanca == 1 &&
        clientes[i].saldoPoupanca > 0 && clientes[i].fezEmprestimo == 0) {
        return 1;
    }
    return 0;
}

/* Pode comprar titulo: saldo da conta salario entre 1000 e 1500 */
int aptoTitulo(int i) {
    if (clientes[i].temSalario == 1 &&
        clientes[i].saldoSalario >= 1000 && clientes[i].saldoSalario <= 1500) {
        return 1;
    }
    return 0;
}

void registrarMovimento(int i, char operacao[], int tipoConta, float valor) {
    int q = clientes[i].qtdMovimentos;
    if (q < MAX_MOVIMENTOS) {
        strcpy(clientes[i].historico[q].operacao, operacao);
        clientes[i].historico[q].tipoConta = tipoConta;
        clientes[i].historico[q].valor = valor;
        clientes[i].qtdMovimentos++;
    }
}

int buscarCliente(int numero) {
    int i;
    for (i = 0; i < qtdClientes; i++) {
        if (clientes[i].numero == numero) {
            return i;
        }
    }
    return -1;   /* nao encontrou */
}


/* ===================== CADASTRO ===================== */

void cadastrarCliente() {
    int i = qtdClientes;

    printf("Nome do cliente: ");
    scanf(" %[^\n]", clientes[i].nome);

    clientes[i].numero = 1000 + i;

    /* escolhe as contas, sem permitir corrente + salario juntas */
    do {
        printf("Abrir conta corrente? (1-Sim 0-Nao): ");
        scanf("%d", &clientes[i].temCorrente);
        printf("Abrir conta salario?  (1-Sim 0-Nao): ");
        scanf("%d", &clientes[i].temSalario);
        printf("Abrir conta poupanca? (1-Sim 0-Nao): ");
        scanf("%d", &clientes[i].temPoupanca);

        if (clientes[i].temCorrente == 1 && clientes[i].temSalario == 1) {
            printf("Nao e permitido ter conta salario e conta corrente ao mesmo tempo!\n");
        }
        if (clientes[i].temCorrente == 0 && clientes[i].temSalario == 0 && clientes[i].temPoupanca == 0) {
            printf("O cliente precisa ter pelo menos uma conta!\n");
        }
    } while ((clientes[i].temCorrente == 1 && clientes[i].temSalario == 1) ||
             (clientes[i].temCorrente == 0 && clientes[i].temSalario == 0 && clientes[i].temPoupanca == 0));

    /* todos os saldos comecam zerados */
    clientes[i].saldoCorrente = 0;
    clientes[i].saldoSalario = 0;
    clientes[i].saldoPoupanca = 0;
    clientes[i].fezEmprestimo = 0;
    clientes[i].temTitulo = 0;
    clientes[i].qtdMovimentos = 0;

    printf("Contas criadas: ");
    if (clientes[i].temCorrente == 1) printf("%d-01  ", clientes[i].numero);
    if (clientes[i].temSalario == 1)  printf("%d-02  ", clientes[i].numero);
    if (clientes[i].temPoupanca == 1) printf("%d-03  ", clientes[i].numero);
    printf("\n");

    qtdClientes++;
}


/* ===================== OPERACOES DO CLIENTE ===================== */

/* Pergunta a conta e confere se o cliente tem ela.
   Retorna 1, 2 ou 3, ou 0 se a conta nao existir */
int escolherConta(int i) {
    int tipo;
    printf("Qual conta? (1-Corrente 2-Salario 3-Poupanca): ");
    scanf("%d", &tipo);

    if ((tipo == 1 && clientes[i].temCorrente == 1) ||
        (tipo == 2 && clientes[i].temSalario == 1) ||
        (tipo == 3 && clientes[i].temPoupanca == 1)) {
        return tipo;
    }
    printf("O cliente nao possui essa conta.\n");
    return 0;
}

void depositar(int i) {
    int tipo = escolherConta(i);
    float valor;

    if (tipo == 0) return;

    printf("Valor do deposito: R$ ");
    scanf("%f", &valor);
    if (valor <= 0) {
        printf("Valor invalido.\n");
        return;
    }

    if (tipo == 1) clientes[i].saldoCorrente += valor;
    if (tipo == 2) clientes[i].saldoSalario += valor;
    if (tipo == 3) clientes[i].saldoPoupanca += valor;

    registrarMovimento(i, "Deposito", tipo, valor);
    printf("Deposito realizado!\n");
}

void sacar(int i) {
    int tipo = escolherConta(i);
    float valor;

    if (tipo == 0) return;

    printf("Valor do saque: R$ ");
    scanf("%f", &valor);
    if (valor <= 0) {
        printf("Valor invalido.\n");
        return;
    }

    /* A conta corrente pode ficar negativa.
       Salario e poupanca precisam ter saldo. */
    if (tipo == 1) {
        clientes[i].saldoCorrente -= valor;
    } else if (tipo == 2 && valor <= clientes[i].saldoSalario) {
        clientes[i].saldoSalario -= valor;
    } else if (tipo == 3 && valor <= clientes[i].saldoPoupanca) {
        clientes[i].saldoPoupanca -= valor;
    } else {
        printf("Saldo insuficiente.\n");
        return;
    }

    registrarMovimento(i, "Saque", tipo, valor);
    printf("Saque realizado!\n");
}

void solicitarEmprestimo(int i) {
    float valor;

    if (clientes[i].fezEmprestimo == 1) {
        printf("O cliente ja fez um emprestimo. So e permitido um por cliente.\n");
        return;
    }
    if (aptoEmprestimo(i) == 0) {
        printf("Emprestimo negado: e preciso ter conta corrente e saldo na poupanca.\n");
        return;
    }

    printf("Limite disponivel: R$ %.2f\n", limiteEmprestimo(i));
    printf("Valor do emprestimo: R$ ");
    scanf("%f", &valor);

    if (valor <= 0 || valor > limiteEmprestimo(i)) {
        printf("Valor invalido ou acima do limite.\n");
        return;
    }

    clientes[i].saldoCorrente += valor;     /* o dinheiro cai na conta corrente */
    clientes[i].fezEmprestimo = 1;
    registrarMovimento(i, "Emprestimo", 1, valor);
    printf("Emprestimo aprovado!\n");
}

void comprarTitulo(int i) {
    if (clientes[i].temTitulo == 1) {
        printf("O cliente ja possui titulo de capitalizacao.\n");
    } else if (aptoTitulo(i) == 0) {
        printf("Compra negada: e preciso ter entre R$ 1000,00 e R$ 1500,00 na conta salario.\n");
    } else {
        clientes[i].temTitulo = 1;
        printf("Titulo de capitalizacao adquirido!\n");
    }
}

void verExtrato(int i) {
    int m;

    printf("\n== Extrato de %s ==\n", clientes[i].nome);
    if (clientes[i].temCorrente == 1) printf("Corrente %d-01 | R$ %.2f\n", clientes[i].numero, clientes[i].saldoCorrente);
    if (clientes[i].temSalario == 1)  printf("Salario  %d-02 | R$ %.2f\n", clientes[i].numero, clientes[i].saldoSalario);
    if (clientes[i].temPoupanca == 1) printf("Poupanca %d-03 | R$ %.2f\n", clientes[i].numero, clientes[i].saldoPoupanca);

    printf("-- Historico --\n");
    for (m = 0; m < clientes[i].qtdMovimentos; m++) {
        printf("%s | conta %d-0%d | R$ %.2f\n",
               clientes[i].historico[m].operacao,
               clientes[i].numero,
               clientes[i].historico[m].tipoConta,
               clientes[i].historico[m].valor);
    }
}

void acessarConta() {
    int numero, i, opcao;

    printf("Numero do cliente (ex.: 1000): ");
    scanf("%d", &numero);

    i = buscarCliente(numero);
    if (i == -1) {
        printf("Cliente nao encontrado.\n");
        return;
    }

    do {
        printf("\n--- Cliente: %s (%d) ---\n", clientes[i].nome, clientes[i].numero);

        /* alerta toda vez que acessar, enquanto a corrente estiver negativa */
        if (clientes[i].temCorrente == 1 && clientes[i].saldoCorrente < 0) {
            printf("*** ALERTA: sua conta corrente esta negativa! Saldo: R$ %.2f ***\n",
                   clientes[i].saldoCorrente);
        }

        printf("1 - Depositar\n");
        printf("2 - Sacar\n");
        printf("3 - Solicitar emprestimo\n");
        printf("4 - Comprar titulo de capitalizacao\n");
        printf("5 - Ver extrato\n");
        printf("0 - Sair da conta\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: depositar(i); break;
            case 2: sacar(i); break;
            case 3: solicitarEmprestimo(i); break;
            case 4: comprarTitulo(i); break;
            case 5: verExtrato(i); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}


/* ===================== MENU PRINCIPAL ===================== */

void abrirBanco() {
    int n, i, opcao;

    if (bancoAberto == 0) {
        bancoAberto = 1;
        printf("\nBanco aberto!\n");
        printf("Quantos clientes deseja cadastrar? ");
        scanf("%d", &n);

        for (i = 0; i < n && qtdClientes < MAX_CLIENTES; i++) {
            printf("\n-- Cliente %d --\n", i + 1);
            cadastrarCliente();
        }
    }

    do {
        printf("\n--- BANCO ABERTO ---\n");
        printf("1 - Acessar conta de cliente\n");
        printf("0 - Voltar ao menu principal\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            acessarConta();
        }
    } while (opcao != 0);
}

void consultarHistorico() {
    int i;
    float total = 0;

    for (i = 0; i < qtdClientes; i++) {
        total += saldoTotalCliente(i);
    }

    printf("\n== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", qtdClientes, total);

    printf("== Clientes Aptos a Emprestimos ==\n");
    for (i = 0; i < qtdClientes; i++) {
        if (aptoEmprestimo(i) == 1) {
            printf("%s|%d-01|R$ %.2f\n", clientes[i].nome, clientes[i].numero, limiteEmprestimo(i));
        }
    }

    printf("== Clientes com Saldo Negativo na Conta Corrente ==\n");
    for (i = 0; i < qtdClientes; i++) {
        if (clientes[i].temCorrente == 1 && clientes[i].saldoCorrente < 0) {
            printf("%s|%d-01|R$ %.2f\n", clientes[i].nome, clientes[i].numero, clientes[i].saldoCorrente);
        }
    }

    printf("== Clientes Aptos a Compra de Titulos ==\n");
    for (i = 0; i < qtdClientes; i++) {
        if (aptoTitulo(i) == 1 && clientes[i].temTitulo == 0) {
            printf("%s|%d-02|R$ %.2f\n", clientes[i].nome, clientes[i].numero, clientes[i].saldoSalario);
        }
    }

    printf("== Clientes Contemplado no Sorteio ==\n");
    for (i = 0; i < qtdContemplados; i++) {
        int c = contemplado[i];
        printf("%s|%d-02|R$ %.2f\n", clientes[c].nome, clientes[c].numero, premioContemplado[i]);
    }
}

void efetuarSorteio() {
    int ordem[MAX_CLIENTES];
    int i, j, aux;
    int n, fib, a, b;
    int posicao, vencedor;
    float premio;

    if (qtdClientes < 5) {
        printf("Sao necessarios pelo menos 5 clientes para o sorteio.\n");
        return;
    }

    /* ordena as posicoes dos clientes do maior saldo para o menor (bubble sort) */
    for (i = 0; i < qtdClientes; i++) {
        ordem[i] = i;
    }
    for (i = 0; i < qtdClientes - 1; i++) {
        for (j = 0; j < qtdClientes - 1 - i; j++) {
            if (saldoTotalCliente(ordem[j]) < saldoTotalCliente(ordem[j + 1])) {
                aux = ordem[j];
                ordem[j] = ordem[j + 1];
                ordem[j + 1] = aux;
            }
        }
    }

    /* mostra os 5 maiores e confere se todos tem titulo */
    printf("\n== 5 maiores saldos ==\n");
    for (i = 0; i < 5; i++) {
        printf("%d - %s | R$ %.2f\n", i, clientes[ordem[i]].nome, saldoTotalCliente(ordem[i]));
    }
    for (i = 0; i < 5; i++) {
        if (clientes[ordem[i]].temTitulo == 0) {
            printf("Sorteio cancelado: %s nao possui titulo de capitalizacao.\n", clientes[ordem[i]].nome);
            return;
        }
    }

    /* Sorteio por Fibonacci: sorteia um n, calcula o n-esimo termo
       e usa o resto da divisao por 5 para achar o vencedor.
       (ajustar aqui se o algoritmo visto em sala for diferente) */
    n = rand() % 20 + 10;     /* n entre 10 e 29 */
    a = 0;
    b = 1;
    for (i = 0; i < n; i++) {
        fib = a + b;
        a = b;
        b = fib;
    }
    fib = a;
    posicao = fib % 5;
    vencedor = ordem[posicao];

    printf("n = %d | Fibonacci(%d) = %d | %d %% 5 = %d\n", n, n, fib, fib, posicao);

    /* premio: dez vezes o saldo da conta salario, depositado nela */
    premio = 10 * clientes[vencedor].saldoSalario;
    clientes[vencedor].saldoSalario += premio;
    registrarMovimento(vencedor, "Premio", 2, premio);

    contemplado[qtdContemplados] = vencedor;
    premioContemplado[qtdContemplados] = premio;
    qtdContemplados++;

    printf("Vencedor: %s | Premio: R$ %.2f\n", clientes[vencedor].nome, premio);
}

