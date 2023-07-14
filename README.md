# t2-redes

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

### oq falta
1. Implementar a estrutura de dados dos canais
2. Permitir a transição de usuários entre canais e a comunicação deles
3. Definir um adm (primeiro usuário do canal) 