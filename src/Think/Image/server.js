const fs = require('fs').promises;
const http = require('http');
const WebSocket = require('ws');

const server = http.createServer();
const wss = new WebSocket.Server({
  server
});

let connectionCount=0

// Vamos ter 3 eventos:
// 1. Evento ON CONNECTION
// 2. Evento ON CLOSE
// 3. Evento ON ERROR

// 1. Evento ON CONNECTION
// Este evento é disparado quando um cliente se conecta ao servidor

let connected;
wss.on('connection', async (ws) => {
	connected = true;
	// Conta o número de conexões
	// Só permite uma conexão por vez
	connectionCount++;
	if (connectionCount > 1) {
		ws.close();
		return;
	}

	console.log('Client connected');
	try {
		// Abre o arquivo synapse, que contém os frames da câmera
		// enviados pelo cliente C++ (que os obtém do servidor no Raspberry Pi)
		const fifoStream = await fs.open('/tmp/synapse', 'r');
		const buffer = Buffer.alloc(1024 * 1024);
		sending = false;
		while (true) {
			// Adiciona um atraso de 33ms - 30fps (fica travado aqui)
			await new Promise(resolve => setTimeout(resolve, 33));
			  // Se o WebSocket não estiver pronto, continue para a próxima iteração
			if (ws.readyState !== WebSocket.OPEN) {
				continue;
			}

			// Lê o frame do arquivo fifo
			const { bytesRead } = await fifoStream.read(buffer, 0, buffer.length);
		
			if (bytesRead === 0) {
				// console.log("Nothing is being sent to the synapse");
				sending = false;
				// break;
			}

			// Se leu algo, pega um pedaço do buffer e envia para o cliente
			const data = buffer.slice(0, bytesRead);
			const base64Data = data.toString('base64'); // Converte os dados para base64
			if (!sending && bytesRead != 0) {
				sending = true;
				console.log("Sending data to the client");
			}

			
			if (ws.readyState === WebSocket.OPEN) {
			  ws.send(base64Data);
			} else {
				break;
			}
		}

		fifoStream.close();
	} catch (error) {
		console.error('Error reading from FIFO:', error);
	}
ws.on('close', async () => {
    await fifoStream.close();
  });
  
  ws.on('close', () => {
    console.log('Client disconnected');
	connected = false;
    connectionCount--;
});
});

// 2. Evento ON CLOSE
// Este evento é disparado quando um cliente se desconecta do servidor
wss.on('error', (error) => {
  console.error('WebSocket error:', error);
});

// 3. Evento ON ERROR
// Este evento é disparado quando ocorre um erro no servidor
server.listen(8081, () => {
  console.log('Server listening on http://localhost:8080');
});