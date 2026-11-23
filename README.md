# AmigoDaVizinhança

Plataforma comunitária para registro, triagem e acompanhamento da resolução de problemas urbanos na vizinhança (iluminação pública, pavimentação e descarte irregular de resíduos) junto à Associação de Moradores.

## Arquitetura do Repositório

- `backend-api/` — API RESTful desenvolvida em Java com Spring Boot e integração com SQL Server.
- `database/` — Modelagem conceitual/lógica, dicionário de dados e scripts DDL, DML e rotinas de automação (SQL Server).
- `triage-queue-c/` — Módulo de processamento de fila de atendimento (FIFO), ordenação e busca binária implementado em C.
- `prototype-ihm/` — Protótipo navegável (Figma), análise de heurísticas de Nielsen e relatórios de testes com usuários.
- `docs/` — Documento de Arquitetura de Software, levantamento de requisitos e entregáveis consolidados.

## Equipe de Desenvolvimento

- Arthur Souza da Silva
- Cleverson Rafael
- Giovanni Victor
- Huallyson Felipe Francisco de Souza Assis
- João Paulo Roberto Barbosa de Lima
- Mateus Rafael Padilha
- Rafael Alves de Santana