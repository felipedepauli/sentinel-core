import React, { useEffect, useRef } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';
import { Card, Button, Row, Col } from 'antd';
import CommandButton from './components/CommandButtons.js'

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

  const socketUrl = 'http://192.168.25.56:5000';
  const { sendMessage, lastMessage, readyState } = useWebSocket(socketUrl);

  useEffect(() => {
    if (lastMessage) {
      const base64Data = lastMessage.data;
      drawImage(base64Data);
    }
  }, [lastMessage]);

  return <canvas ref={canvasRef} style={{ width: '100%', height: '100%' }} />;
};

const App = () => {
  const personInfo = {
    name: 'John Doe',
    description: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.'
  };

  const handleButton1Click = () => {
    // chama a função/API correspondente
  };

  const handleButton2Click = () => {
    // chama a função/API correspondente
  };

  const handleButton3Click = () => {
    // chama a função/API correspondente
  };

  return (
    <div>
      <h1>Imagem da câmera do drone</h1>
      <Row gutter={16}>
        <Col span={14}>
          <FrameRenderer />
        </Col>
        <Col span={10}>
          <Card title={personInfo.name}>
            <p>{personInfo.description}</p>
          </Card>
        </Col>
      </Row>
      <Row justify="center" style={{ marginTop: '16px' }}>
        <CommandButton label="^" command="throttle_up" style={{ gridColumn: '2', gridRow: '1' }} />
      </Row>
      <Row justify="center" style={{ marginTop: '16px' }}>
        <CommandButton label="<" command="turn_left" style={{ gridColumn: '1', gridRow: '2' }} />
        <CommandButton label=">" command="turn_right" style={{ gridColumn: '3', gridRow: '2' }} />
      </Row>
      <Row justify="center" style={{ marginTop: '16px' }}>
        <CommandButton label="v" command="throttle_down" style={{ gridColumn: '2', gridRow: '3' }} />
      </Row>
    </div>
  );
};

export default App;