// Importing required libraries
const fs = require('fs').promises;
const http = require('http');
const WebSocket = require('ws');

// Creating HTTP server
const server = http.createServer();

// Creating WebSocket server associated with HTTP server
const wss = new WebSocket.Server({ server });

// Counter for the number of connections
let connectionCount = 0;

// Variable to keep track of the connection status
let connected;

// 1. ON CONNECTION Event
// This event triggers when a client connects to the server
wss.on('connection', async (ws) => {
  connected = true;
  
  // Increment the connection count
  // Only allow one connection at a time
  connectionCount++;
  if (connectionCount > 1) {
    wss.close();
    return;
  }

  console.log('Client connected');
  try {
    // Open the 'synapse' file which contains the camera frames 
    // sent by the C++ client (which gets them from the server on the Raspberry Pi)
    const fifoStream = await fs.open('/tmp/synapse', 'r');
    const buffer = Buffer.alloc(1024 * 1024);
    let sending = false;
    while (true) {
      // Add a delay of 33ms - approximately 30fps
      await new Promise(resolve => setTimeout(resolve, 33));
      
      // If WebSocket is not ready, continue to the next iteration
      if (ws.readyState !== WebSocket.OPEN) {
        continue;
      }

      // Read the frame from the FIFO file
      const { bytesRead } = await fifoStream.read(buffer, 0, buffer.length);
  
      if (bytesRead === 0) {
        // No data to send to the synapse
        sending = false;
      }

      // If data read, slice buffer to the length of data read and send to client
      const data = buffer.slice(0, bytesRead);
      const base64Data = data.toString('base64'); // Convert data to base64

      if (!sending && bytesRead != 0) {
        sending = true;
        console.log("Sending data to the client");
      }

      // If WebSocket is still open, send data
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(base64Data);
      } else {
        break;
      }
    }

    // Close the stream
    fifoStream.close();

  } catch (error) {
    console.error('Error reading from FIFO:', error);
  }

  // 2. ON CLOSE Event
  // This event triggers when a client disconnects from the server
  ws.on('close', async () => {
    await fifoStream.close();
    console.log('Client disconnected');
    connected = false;
    connectionCount--;
  });
});

// 3. ON ERROR Event
// This event triggers when an error occurs in the server
wss.on('error', (error) => {
  console.error('WebSocket error:', error);
});

// Start listening on the specified port
server.listen(8080, () => {
  console.log('Server listening on http://localhost:8080');
});
