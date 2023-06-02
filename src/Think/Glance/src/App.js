import React, { useEffect, useState, useRef } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';
import { Button, Image } from 'antd';
import CommandButton from './components/CommandButtons.js'
import CustomModal from './components/Modal.js'
import Auth from "./components/Auth.js"
import PersonInfoCard from './components/PersonInfoCard.js';

// Importing css style
import './App.css';

// This component is responsible for rendering the frames received via WebSocket
const FrameRenderer = () => {
  // Create a ref to access the canvas
  const canvasRef = useRef(null);

  // Function to draw an image on the canvas from base64 data
  const drawImage = (base64Data) => {
    const img = new Image();
    img.src = `data:image/jpeg;base64,${base64Data}`;
    img.onload = () => {
      if (canvasRef.current) {
        const ctx = canvasRef.current.getContext('2d');
        // Adjust canvas width and height to match the image
        canvasRef.current.width = img.width;
        canvasRef.current.height = img.height;
        ctx.drawImage(img, 0, 0, img.width, img.height);
      }
    };
  };

  // WebSocket URL
  const socketUrl = 'ws://localhost:8080/web';
  const { sendMessage, lastMessage, readyState } = useWebSocket(socketUrl);

  // Effect to draw image whenever a new message arrives
  useEffect(() => {
    if (lastMessage) {
      const base64Data = lastMessage.data;
      drawImage(base64Data);
    }
  }, [lastMessage]);

  return <canvas ref={canvasRef} style={{ width: 640, height: 480 }} />;
};

// Main application component
const App = () => {
  // Create states for the person info, drone status and modal visibility
  const [personInfo, setPersonInfo] = useState({ name: 'Nobody', age: "", status: "", description: '...' });
  const [droneStarted, setDroneStarted] = useState(false);
  const [isModalOpen, setIsModalOpen] = useState(false);

  // Function to toggle the drone status
  const toggleDrone = () => {
    setDroneStarted(!droneStarted);
  }

  return (
    <div className="main">
		<div className="program_name">
      <div className='program_logo'>
        <Image
          width={230}
          src="/Logo_.png"
          />
      </div>

      		<h1>Sentinel Eyes</h1>
		</div>
		<div className="program_image">	
				<FrameRenderer />
		</div>
    <div className="program_desc">
        <PersonInfoCard personInfo={personInfo} />
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
                <div>
                  <Auth setPersonInfo={setPersonInfo} />
                </div>
                <div>
                  <div className="disattached">
                      <Button style={{ backgroundColor: '#fffb8f', borderColor: '#262626', color: '#613400' }} onClick={() => setIsModalOpen(true)}>
                        Cadastrar
                      </Button>
                </div>
                <CustomModal
                  isOpen={isModalOpen}
                  onClose={() => setIsModalOpen(false)}
                  onSubmit={(event, file) => {
                    event.preventDefault();
                    const formData = new FormData(event.target);
                    formData.append('photo', file);
                  }}
                />
                </div>
          	</div>
            {/* BsPersonBoundingBox */}
            {/* ImTarget */}
		</div>

        
      </div>
  );
};

export default App;