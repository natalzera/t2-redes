# Trabalho 2 de Redes - Sockets

### Para rodar o projeto
Após baixar os códigos do repositório, execute os comandos em seu terminal:

```bash
$make ms   # para compilar o executável do servidor 
$make mc   # para compilar o executável do cliente

$make rs   # para executar o servidor 
$make rc   # para executar o cliente
```

### Sobre o funcionamento do projeto
1. Os clientes se comunicam com o servidor através de sockets;
2. O servidor consegue gerenciar a comunicação com vários clientes (vários sockets) utilizando threads;
3. Cada cliente pode mandar mensagens em seus sockets que o servidor irá recebê-los em segmentos que informam o id do cliente que está enviando (dado pelo servidor no início da conexão) e o resto da mensagem;
4. O servidor pode repassar os segmentos que recebe (com id do cliente de origem e mensagem) para todos os demais clientes conectados a ele;
5. Para mensagens específicas (como /ping, /connect ou /quit por exemplo), o servidor pode executar comandos ou responder apenas o cliente que a mandou. Dentre elas, temos:
    5.1. /connect solicita conexão com o servidor, ele retornará ao cliente seu id utilizado para conexão;
    5.2. /close solicita o desligamento do servidor, funcionando apenas se nenhuma conexão estiver ativa com ele;
6. O servidor e os clientes possuem uma tabela de nomes que mapeiam cada id de cliente para um nome de usuário específico para identificá-los;
7. Após um cliente solicitar a mudança de nome de usuário ao servidor (com o comando /nickname _nome_), ele informa todos os outros clientes desta mudança para todos atualizarem suas tabelas de nomes;

### Link de explicação do projeto


### Integrantes do grupo
Davi Fagundes Ferreira da Silva
Gabriel Natal Coutinho
Théo Bruno Frey Riffel