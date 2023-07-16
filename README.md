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

Os clientes também podem se conversar usando canais:
8. Para criar/entrar em um canal, precisa-se usar o comando /join _canal_
    8.1 O primeiro que entrar no canal vira o adminstrador
    8.2 As mensagens mandadas no chat do canal são apenas visíveis aos que estão nele, já as mensagens de cliente fora de qualquer canal é disponível a todos.
    8.3 Para mudar de canal, usa-se o comando /join _canal_ também (isso com o canal já criado).

O adminstrador tem comandos exclusivos:
9. Ele pode usar o comando /kick _nomeUsuario_ para retirar um usuário do canal.
10. Ele pode usar o comando /mute _nomeUsuario_ para proibir um usuário de falar.
11. Ele pode usar o comando /unmute _nomeUsuario_ para retirar a proibição acima.

### Link de execução do projeto
https://drive.google.com/drive/folders/1f3j1juO_vfaJpHPIOdYMudd0fbOqkHUc?usp=sharing

### Integrantes do grupo
Davi Fagundes Ferreira da Silva <br>
Gabriel Natal Coutinho <br>
Théo Bruno Frey Riffel <br>