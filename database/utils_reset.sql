USE FilaZero;
GO

---- Remove primeiro as consultas devido à restrição de chave estrangeira (FK)
--DELETE FROM Consultas;
--DELETE FROM Pacientes;
--GO

---- Reinicia os geradores automáticos de ID, preservando a tabela Medicos
--DBCC CHECKIDENT ('Consultas', RESEED, 0);
--DBCC CHECKIDENT ('Pacientes', RESEED, 0);
--GO

	


-- 4. VISUALIZANDO O RESULTADO


SELECT * FROM Pacientes;
SELECT * FROM Medicos;
SELECT * FROM Consultas;