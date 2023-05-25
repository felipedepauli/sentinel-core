import React from 'react';
import axios from 'axios';

import iconMap from './IconMap.js';
import { Button } from 'antd';



class CommandButton extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      isPressed: true,
    };
  }
  sendCommand(command) {
    axios.post('http://localhost:5000/api/command', {
      command: command
    })
    .then(function (response) {
      console.log(response);
    })
    .catch(function (error) {
      console.log(error);
    });

    if (this.props.onClick) {
      this.props.onClick();
    }
  }

  render() {
    const { isPressed } = this.state;
    const { icon, toggleIcon, text } = this.props;
    const iconToRender = isPressed && toggleIcon ? toggleIcon : icon;

    return (
      <Button size="large" onClick={() => this.sendCommand(this.props.command)} aria-label={text}>
        {iconMap[iconToRender] || text}
      </Button>
    );
  }
}

export default CommandButton


