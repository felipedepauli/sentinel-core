import React from 'react';
import axios from 'axios';

class CommandButton extends React.Component {
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
  }

  render() {
    return (
      <button onClick={() => this.sendCommand(this.props.command)}>
        {this.props.label}
      </button>
    );
  }
}

export default CommandButton
