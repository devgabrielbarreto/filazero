#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <sqlext.h>
#include <locale.h>



	// --- 1. A ESTRUTURA DA FILA (SIMPLIFICADA COM VETOR) ---
	typedef struct {
	    char nome[100];
	    char cpf[15];
	    char telefone[16];
	} Paciente;
	
	// Cria um vetor fixo de 100 posições. 
	Paciente fila[100]; 
	int totalNaFila = 0; // Contador numérico para saber quantas pessoas estão esperando
	
	SQLHDBC dbc; 


// --- 2. FUNÇÕES DO BANCO DE DADOS (MANTIDAS IGUAIS) ---
void salvarPacienteNoBanco(char *nome, char *cpf, char *telefone) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    char query[300];
    sprintf(query, "INSERT INTO Pacientes (Nome, CPF, Telefone) VALUES ('%s', '%s', '%s')", nome, cpf, telefone);
    SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    printf(">> Dados do paciente gravados no SQL Server.\n");
}

void registrarConsultaNoBanco(char *cpf, int id_medico) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    char query[500];
    sprintf(query, "INSERT INTO Consultas (DataHora, StatusConsulta, ID_Paciente, ID_Medico) VALUES (GETDATE(), 'Em Atendimento', (SELECT ID_Paciente FROM Pacientes WHERE CPF = '%s'), %d)", cpf, id_medico);
    SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    printf(">> [SISTEMA] Consulta registada na base de dados com sucesso!\n");
}

void listarPacientesBanco() {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    char query[] = "SELECT Nome, CPF, Telefone FROM Pacientes";
    SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);
    
    char nome[100], cpf[15], tel[16];
    SQLLEN len;
    SQLBindCol(stmt, 1, SQL_C_CHAR, nome, sizeof(nome), &len);
    SQLBindCol(stmt, 2, SQL_C_CHAR, cpf, sizeof(cpf), &len);
    SQLBindCol(stmt, 3, SQL_C_CHAR, tel, sizeof(tel), &len);
    
    printf("\n--- REGISTROS NO BANCO DE DADOS (SQL SERVER) ---\n");
    int achou = 0;
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        printf("- %s | CPF: %s | Tel: %s\n", nome, cpf, tel);
        achou = 1;
    }
    if (achou == 0) printf("Nenhum paciente cadastrado no banco.\n");
    printf("------------------------------------------------\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void listarMedicos() {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    char query[] = "SELECT ID_Medico, Nome, Especialidade FROM Medicos";
    SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);
    
    int id;
    char nome[100], esp[100];
    SQLLEN len;
    
    SQLBindCol(stmt, 1, SQL_C_LONG, &id, sizeof(int), &len);
    SQLBindCol(stmt, 2, SQL_C_CHAR, nome, sizeof(nome), &len);
    SQLBindCol(stmt, 3, SQL_C_CHAR, esp, sizeof(esp), &len);
    
    printf("\n--- MEDICOS DISPONIVEIS ---\n");
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        printf("ID: %d | %s (%s)\n", id, nome, esp);
    }
    printf("---------------------------\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

// --- 3. FUNÇÕES DA FILA (SIMPLIFICADAS) ---
void enfileirar(char *nome, char *cpf, char *telefone) {
    if (totalNaFila >= 100) {
        printf("\n>> [ERRO] A sala de espera esta lotada!\n");
        return;
    }
    
    // Grava os dados na posição livre do vetor (que é o totalNaFila)
    strcpy(fila[totalNaFila].nome, nome);
    strcpy(fila[totalNaFila].cpf, cpf);
    strcpy(fila[totalNaFila].telefone, telefone);
    
    totalNaFila++; // Aumenta o contador de pessoas na fila
    printf("\n>> %s entrou na fila de espera da clinica!\n", nome);
}

void desenfileirar(int id_medico) {
    if (totalNaFila == 0) {
        printf("\n>> A sala de espera esta vazia!\n");
        return;
    }
    
    // Busca o nome do médico no banco
    char nomeMedico[100] = "Medico";
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    char query[200];
    sprintf(query, "SELECT Nome FROM Medicos WHERE ID_Medico = %d", id_medico);
    if (SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS) == SQL_SUCCESS) {
        SQLLEN len;
        SQLBindCol(stmt, 1, SQL_C_CHAR, nomeMedico, sizeof(nomeMedico), &len);
        SQLFetch(stmt); 
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    // O próximo a ser atendido é sempre quem está na posição 0 do vetor
    printf("\n>> O(A) %s ESTA A CHAMAR: %s (CPF: %s)\n", nomeMedico, fila[0].nome, fila[0].cpf);
    registrarConsultaNoBanco(fila[0].cpf, id_medico);
    
    // Faz a fila "andar": todos os pacientes dão um passo para a frente no vetor
    for (int i = 0; i < totalNaFila - 1; i++) {
        fila[i] = fila[i + 1];
    }
    
    totalNaFila--; // Diminui o contador, pois alguém saiu da fila
}

void mostrarFila() {
    if (totalNaFila == 0) {
        printf("\n>> A fila de espera esta vazia no momento.\n");
        return;
    }
    
    printf("\n--- PACIENTES NA FILA (SALA DE ESPERA) ---\n");
    // Um simples 'for' percorre o contador exibindo quem está na sala
    for (int i = 0; i < totalNaFila; i++) {
        printf("%d. %s (CPF: %s | Tel: %s)\n", i + 1, fila[i].nome, fila[i].cpf, fila[i].telefone);
    }
    printf("------------------------------------------\n");
}

// --- 4. CONECTAR AO SQL SERVER ---
void conectar() {
    SQLHENV env;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    char conexao[] = "DRIVER={SQL Server};SERVER=.\\SQLEXPRESS;DATABASE=FilaZero;Trusted_Connection=yes;";
    SQLDriverConnect(dbc, NULL, (SQLCHAR*)conexao, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
}

// --- 5. MENU PRINCIPAL ---
int main() {
	setlocale(LC_ALL, "Portuguese"); 
	SetConsoleOutputCP(1252);
    SetConsoleCP(1252);
    conectar();
    int opcao = 0;
    char nome[100], cpf[15], telefone[16];

    printf("--- SISTEMA FILAZERO INICIADO ---\n");

    while(opcao != 5) {
        printf("\n1. Novo Paciente (Salva no BD e poe na fila)\n");
        printf("2. Chamar Proximo Paciente (Gera Consulta)\n");
        printf("3. Mostrar Fila de Espera\n");
        printf("4. Ver Registros no Banco de Dados\n");
        printf("5. Sair\n");
        printf("Escolha: ");
        
        scanf(" %d", &opcao); 

        if (opcao == 1) {
            printf("Nome completo: ");
            scanf(" %[^\n]", nome); 
            printf("CPF (ex: 123.456.789-00): ");
            scanf(" %s", cpf);
            printf("Telefone: ");
            scanf(" %[^\n]", telefone);
            
            salvarPacienteNoBanco(nome, cpf, telefone); 
            enfileirar(nome, cpf, telefone);       
        } 
        else if (opcao == 2) {
            if (totalNaFila == 0) { // Agora verifica o contador em vez de ponteiro nulo
                printf("\n>> A sala de espera esta vazia! Nao ha pacientes para chamar.\n");
            } else {
                listarMedicos(); 
                int id_med;
                printf("Digite o ID do medico que vai atender (ex: 1 ou 2): ");
                scanf(" %d", &id_med);
                desenfileirar(id_med);
            }
        }
        else if (opcao == 3) {
            mostrarFila();
        }
        else if (opcao == 4) {
            listarPacientesBanco();
        }
    }
    
    SQLDisconnect(dbc);
    return 0;
}