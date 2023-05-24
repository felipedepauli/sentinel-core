import React, { useEffect, useRef } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';

const FrameRenderer = () => {
  const canvasRef = useRef(null);

  const drawImage = (base64Data) => {
    const img = new Image();
    img.src = `data:image/jpeg;base64,${base64Data}`;
    img.onload = () => {
      if (canvasRef.current) {
        const ctx = canvasRef.current.getContext('2d');
        ctx.drawImage(img, 0, 0);
      }
    };
  };

  const socketUrl = 'ws://localhost:8081';
  const { sendMessage, lastMessage, readyState } = useWebSocket(socketUrl);

  useEffect(() => {
    if (lastMessage) {
      // Os dados já estão em base64, não precisamos converter novamente
      const base64Data = lastMessage.data;
      drawImage(base64Data);
    }
  }, [lastMessage]);

  return <canvas ref={canvasRef} width="640" height="480" />;
};

const App = () => {
  return (
    <div>
      <h1>Imagem da câmera do drone</h1>
      <FrameRenderer />
    </div>
  );
};

export default App;
