import React, { useEffect, useRef } from 'react';

const VideoPlayer = ({ wsURL }) => {
  const imgRef = useRef(null);

  useEffect(() => {
    const ws = new WebSocket(wsURL);

    ws.addEventListener('message', (event) => {
      const base64Data = event.data;
      if (imgRef.current) {
        imgRef.current.src = `data:image/jpeg;base64,${base64Data}`;
      }
    });

    return () => {
      ws.close();
    };
  }, [wsURL]);

  return <img ref={imgRef} alt="Video stream" />;
};

export default VideoPlayer;
