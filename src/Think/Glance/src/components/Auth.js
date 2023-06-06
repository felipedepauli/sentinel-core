import axios from 'axios'; // Não esqueça de instalar isso via npm ou yarn
import { Button } from 'antd';
import { BsPersonBoundingBox } from "react-icons/bs"; // Não esqueça de instalar isso via npm ou yarn
import React from 'react';

const AuthButton = ({ setPersonInfo }) => {
  const authenticate = () => {
    setPersonInfo({
      name: 'Looking For',
      age: "",
      status: "",
      description: 'Wait authentication...'
    });

    axios.get('http://localhost:8081/memory/auth')
      .then((response) => {
        console.log(response.data); // Isto deve imprimir o objeto da pessoa no console do navegador
          setPersonInfo({
            age: response.data.id.age,
            name: response.data.id.name,
            description: response.data.id.description,
            status: response.data.id.status
          })
      })
      .catch((error) => {
        console.error(error);
        setPersonInfo({
          age: "",
          name: "No detection!",
          description: "",
          status: ""
        })
      });
      console.log("Executei a parada aqui")
  }

  return (
    <Button size="large" icon={<BsPersonBoundingBox size={27}/>} onClick={authenticate}>
    </Button>
  )
};

export default AuthButton;
