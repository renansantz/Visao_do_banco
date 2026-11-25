#ifndef MEU_BANCO_H
#define MEU_BANCO_H

#define MAX_CLIENTES 100
#define MAX_MOVIMENTOS 50

/* Uma movimentacao do historico do cliente */
struct Movimento {
    char operacao[20];   /* Deposito, Saque, Emprestimo, Premio */
    int tipoConta;       /* 1 - corrente, 2 - salario, 3 - poupanca */
    float valor;
};

/* Carteira digital do cliente */
struct Cliente {
    char nome[50];
    int numero;              /* comeca em 1000 */

    int temCorrente;         /* 1 = tem a conta, 0 = nao tem */
    int temSalario;
    int temPoupanca;

    float saldoCorrente;
    float saldoSalario;
    float saldoPoupanca;

    int fezEmprestimo;       /* 1 = ja fez emprestimo */
    int temTitulo;           /* 1 = comprou titulo de capitalizacao */

    struct Movimento historico[MAX_MOVIMENTOS];
    int qtdMovimentos;
};

/* Variaveis globais: ficam criadas no banco.c.
   O "extern" avisa aos outros arquivos que elas existem la. */
extern struct Cliente clientes[MAX_CLIENTES];
extern int qtdClientes;
extern int bancoAberto;

/* ---------- funcoes auxiliares ---------- */
float saldoTotalCliente(int i);
float limiteEmprestimo(int i);
int aptoEmprestimo(int i);
int aptoTitulo(int i);
void registrarMovimento(int i, char operacao[], int tipoConta, float valor);
int buscarCliente(int numero);

/* ---------- cadastro e operacoes do cliente ---------- */
void cadastrarCliente();
int escolherConta(int i);
void depositar(int i);
void sacar(int i);
void solicitarEmprestimo(int i);
void comprarTitulo(int i);
void verExtrato(int i);
void acessarConta();

/* ---------- opcoes do menu principal ---------- */
void abrirBanco();
void consultarHistorico();
void efetuarSorteio();

#endif
