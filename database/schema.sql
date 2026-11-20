-- 1. CRIAÇÃO DO BANCO DE DADOS
CREATE DATABASE FilaZero;
GO

USE FilaZero;
GO

-- 2. CRIAÇÃO DAS TABELAS
CREATE TABLE Pacientes (
    ID_Paciente INT IDENTITY(1,1) PRIMARY KEY,
    Nome VARCHAR(100) NOT NULL,
    CPF VARCHAR(14) UNIQUE NOT NULL,
    Telefone VARCHAR(15)
);

CREATE TABLE Medicos (
    ID_Medico INT IDENTITY(1,1) PRIMARY KEY,
    Nome VARCHAR(100) NOT NULL,
    CRM VARCHAR(20) UNIQUE NOT NULL,
    Especialidade VARCHAR(50)
);

CREATE TABLE Consultas (
    ID_Consulta INT IDENTITY(1,1) PRIMARY KEY,
    DataHora DATETIME NOT NULL,
    StatusConsulta VARCHAR(20) DEFAULT 'Aguardando', 
    ID_Paciente INT, 
    ID_Medico INT,   
    FOREIGN KEY (ID_Paciente) REFERENCES Pacientes(ID_Paciente),
    FOREIGN KEY (ID_Medico) REFERENCES Medicos(ID_Medico)
);
GO

-- 3. INSERINDO DADOS DE TESTE
INSERT INTO Medicos (Nome, CRM, Especialidade)
VALUES ('Dra. Ana Souza', '123456-SP', 'Cardiologia'),
       ('Dr. Carlos Lima', '654321-SP', 'Clínica Geral');

INSERT INTO Pacientes (Nome, CPF, Telefone)
VALUES ('João Silva', '111.222.333-44', '(19) 99999-1111'),
       ('Maria Oliveira', '555.666.777-88', '(19) 98888-2222');

INSERT INTO Consultas (DataHora, StatusConsulta, ID_Paciente, ID_Medico)
VALUES (GETDATE(), 'Aguardando', 1, 1), 
       (GETDATE(), 'Aguardando', 2, 2); 
GO

-- 4. VISUALIZANDO O RESULTADO
SELECT * FROM Pacientes;
SELECT * FROM Medicos;
SELECT * FROM Consultas;