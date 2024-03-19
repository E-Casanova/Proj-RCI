# Chat Sobre Anel RCI

## Descrição do Projeto
Este projeto de Rede de Computadores e Internet (RCI) visa desenvolver uma aplicação de chat que opera dentro de uma topologia de rede em anel

## Funcionalidades
- **Topologia em Anel**: Implementação de uma rede em anel onde cada nó se conecta apenas ao seu sucessor e predecessor, formando um círculo de comunicação.
- **Gerenciamento de Nós**: Capacidade de adicionar e remover nós da rede dinamicamente sem interromper o serviço de chat.
- **Encaminhamento de Mensagens**: Utilização de um algoritmo de encaminhamento para determinar o caminho mais curto através do anel, otimizando a entrega de mensagens.
- **Interface de Utilizador**: Comandos para permitir aos utilizadores juntarem-se à rede, enviarem mensagens e saírem do chat.

## Começando
### Pré-requisitos
- GCC para compilação do código fonte
- Sistema operacional Linux ou compatível

## Comandos da Interface de Usuário
- `join (j) ring id`: Entrar em um anel especificado com um ID único.
- `direct join (dj) id succid succIP succTCP`: Entrar diretamente em um anel sem consultar o servidor de registros.
- `chord (c)`: Estabelecer uma corda para otimizar o caminho de mensagens.
- `leave (l)`: Sair do anel.
- `message (m) dest message`: Enviar uma mensagem para um destino específico no anel.

## Estrutura do Projeto
- `user_commands.c`: Implementa funções para processar comandos do utilizador
- `server_client.c`: Implementa a lógica de conexão entre nós (cliente/servidor).
- `node.c`: Contém funções para gerenciamento de nós na rede em anel.
- `main.c`: Ponto de entrada da aplicação.
