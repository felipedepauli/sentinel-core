import React, { useEffect, useState, useRef } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';
import { Card, Button, Row, Col } from 'antd';
import CommandButton from './components/CommandButtons.js'

import './App.css';

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

  const socketUrl = 'ws://localhost:8080';
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
  const [personInfo, setPersonInfo] = useState({ name: 'Nobody', description: '...' });
  const [droneStarted, setDroneStarted] = useState(false);

  const authenticate = () => {
    setPersonInfo({
      name: 'Looking For',
      description: 'Wait authentication...'
    });
  
  }

  const toggleDrone = () => {
    setDroneStarted(!droneStarted);
  }

  return (
    <div class="main">
		<div className="program_name">
      		<h1>Sentinel Eyes</h1>
		</div>
		<div className="program_image">	
				<FrameRenderer />
		</div>
		<div className="program_desc">
        <Card className="program_desc__block" title={personInfo.name}>
          <p>{personInfo.description}</p>
        </Card>
		</div>
		<div className='program_panel'>
          
        <div className='controller__section controller__section--left'>
          <CommandButton command={droneStarted ? "stopDrone" : "startDrone"} text="Toggle drone" icon="TbDrone" toggleIcon="TbDroneOff" onClick={toggleDrone}/>
        </div>

          	<div className='controller__section controller__section--center'>
				<div className="controller__section--center__top">
					<CommandButton command="riseUp" text="Throttle Up" icon="BsFillArrowUpCircleFill"/>
				</div>
				<div className="controller__section--center__middle">
					<CommandButton command="spinLeft" text="Turn Left" icon="BsFillArrowLeftCircleFill"/>
					<CommandButton command="floating" text="Stop" icon="BsStopCircleFill"/>
					<CommandButton command="spinRight" text="Turn Right" icon="BsFillArrowRightCircleFill"/>
				</div>
				<div className="controller__section--center__bottom">
					<CommandButton command="fallDown" text="Throttle Down" icon="BsFillArrowDownCircleFill"/>
				</div>
			</div>

          	<div className='controller__section controller__section--right'>
					<CommandButton command="auth" text="Authenticate" icon="BsPersonBoundingBox" onClick={authenticate}/>
          	</div>
            {/* BsPersonBoundingBox */}
            {/* ImTarget */}
		</div>
        
      </div>
  );
};

export default App;
