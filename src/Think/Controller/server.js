// Importing required libraries
const fs = require('fs').promises;
const http = require('http');
const WebSocket = require('ws');

// Creating HTTP server
const server = http.createServer();

// Creating WebSocket server associated with HTTP server for web client
const wssWeb = new WebSocket.Server({ noServer: true });

// Creating WebSocket server associated with HTTP server for Python client
const wssPython = new WebSocket.Server({ noServer: true });

// Counter for the number of connections
let connectionCount = 0;

// Variable to keep track of the connection status
let connected;
let webSocket;

// Event for Web Client Connection
wssWeb.on('connection', (ws) => {
  webSocket = ws;
  console.log('Web client connected');
});

// Event for Python Client Connection
wssPython.on('connection', (ws) => {
  connected = true;
  
  // Increment the connection count
  // Only allow one connection at a time
  connectionCount++;
  if (connectionCount > 1) {
    wssPython.close();
    return;
  }

  console.log('Python client connected');

  ws.on('message', (message) => {
    const data = JSON.parse(message);
    // Decodifica a imagem
    const imgBuffer = Buffer.from(data.frame, 'base64');
    // Envie a imagem como uma string base64 para o cliente web
    if (webSocket && webSocket.readyState === WebSocket.OPEN) {
      webSocket.send(imgBuffer.toString('base64'));
    }
    // console.log(imgBuffer.slice(0, 10).toString('hex'))
  });
  
  

  // ON CLOSE Event
  // This event triggers when the Python client disconnects from the server
  ws.on('close', () => {
    console.log('Python client disconnected');
    connected = false;
    connectionCount--;
  });
});

// Handle upgrade of requests for different paths
server.on('upgrade', function upgrade(request, socket, head) {
  const pathname = new URL(request.url, 'http://localhost').pathname;

  if (pathname === '/web') {
    wssWeb.handleUpgrade(request, socket, head, function done(ws) {
      wssWeb.emit('connection', ws, request);
    });
  } else if (pathname === '/python') {
    wssPython.handleUpgrade(request, socket, head, function done(ws) {
      wssPython.emit('connection', ws, request);
    });
  } else {
    socket.destroy();
  }
});

// Start listening on the specified port
server.listen(8080, () => {
  console.log('Server listening on http://192.168.137.1:8080');
});
